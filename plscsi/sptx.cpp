/**
**  sptx.cpp
**  Saturday. September 21, 2002
**
**  Talk thru a Win NT/ 2K/ XP DeviceIoControl connection to a device.
**
**  Grep views include:
**
**      grep "^[A-Za-z_]"   // routines
**      grep "[Ee]rr|fpr"   // errors traced
**/

/**
**  Link with standard C libraries.
**/

#include <limits.h> /* INT_MAX ... */
#include <stddef.h> /* offsetof ... */
#include <stdio.h> /* FILE ... */
#include <stdlib.h> /* calloc ... */
#include <string.h> /* memset ... */

/**
**  Link with the *.c* of ../plscsi/.
**/

#include "plscsi.h"

/**
**  Define some units.
**/

#define K       1000
#define M       (K * K)
#define G       (M * K)

#define Ki      1024 /* x1024 = x400 = 1 << 10 */

/**
**  Link with a Windows DDK.
**/

#ifdef SPTX

#define WIN32_LEAN_AND_MEAN WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN WIN32_EXTRA_LEAN

#include <windows.h> /* ULONG_PTR ... */
#include <winioctl.h> /* CTL_CODE ... */
#include "ntddscsi.h" /* SCSI_PASS_THROUGH_DIRECT ... */
#include "ntiocaps.h" /* IO_SCSI_CAPABILITIES ... */


#define PAGE_SIZE 0x1000 /* x1000 = "Intel 386" page size per <ddk/ntddk.h> */

/**
**  Describe a Win NT/ 2K/ XP DeviceIoControl connection to device.
**
**  Support SPT and SPTD.  SPT double buffers, SPTD does not.
**
**  Often SPT and SPTD support different DataTransferLength's.  For
**  example, SPTD might work for 0 and for 2 thru x2:0000, while SPT
**  works for 0 and 1 and up to ((x2:0004 - sizeof SptxCommand) = x1:FDCC).
**
**  Often the offsets of SPTD equal those of SPT, especially if:
**      (sizeof (PVOID)) == (sizeof (ULONG_PTR)) == 4
**/

#define MAX_CDB_LENGTH 0xFF
#define MAX_SENSE_LENGTH 0xFF

typedef union SptxHeader SptxHeader;

union SptxHeader
    {
    SCSI_PASS_THROUGH_DIRECT    sptd;
    SCSI_PASS_THROUGH           spt;
    };

typedef struct SptxCommand SptxCommand;

struct SptxCommand
    {
    SptxHeader  sph; /* "sph" = "SCSI_PASS_THROUGH* header" */
    UCHAR       ucMoreCdbChars[MAX_CDB_LENGTH]; /* 16 more than necessary */
    DWORD       dwBeforeSense; /* exist to dword-align ucSense */
    UCHAR       ucSense[MAX_SENSE_LENGTH];
    DWORD       dwBeforeData; /* exist to dword-align ucData */
    UCHAR       ucData[1];
/*  UCHAR       ucData[...]; // dynamically sized */
    };

struct Sptx /* connection */
    {
    SptxCommand *   theSptxCommand;
    int             theMaxSptLength;
    int             theMinSptdLength;

    FILE *          theErrFile;

    HANDLE          theHandle;
    int             theMaxLength;
    int             theAlignmentMask;

    int             theMaxSenseLength;
    int             theMinSeconds;

    int             theExitInt;
    int             theLength;
    char const *    theSenseChars;
    int             theSenseLength;

    int             theReadNameInt;
    };

/**
**  Substitute for <stdlib.h> perror, when the "No error" message that
**  the perror of Windows prints is less informative than desired.
**/

static void win_fperror(FILE * fi, char const * string)
    {

    /* Sample the MS errno. */

    DWORD dwGetLastError =  GetLastError();

    /* Convert to zstring error message. */

    char msgChars[1234] = { '\0' };
    DWORD dwFlags =         FORMAT_MESSAGE_FROM_SYSTEM;
    LPCVOID lpSource =      NULL; /* immaterial for FORMAT_MESSAGE_FROM_SYSTEM */
    DWORD dwMessageId =     dwGetLastError;
    DWORD dwLanguageId =    0; /* 0 = as you please */
    LPTSTR lpBuffer =       &msgChars[0];
    DWORD nSize =           (sizeof msgChars); /* tbd: long enough? */
    va_list * Arguments =   NULL;
    DWORD dwFormatMessage = FormatMessage(
        dwFlags, lpSource, dwMessageId, dwLanguageId, lpBuffer, nSize, Arguments);

    /* Provide a default error message. */

    int iLastError = dwGetLastError;
    if (!(0 < dwFormatMessage))
        {
        (void) sprintf(&msgChars[0], "(no more specific error message available)");
        }

    /* Print the string arg, and then the message, to stderr. */

    (void) fprintf(fi, "%s: x%lX (%ld) %s", string,
        (long) iLastError, (long) iLastError, &msgChars[0]);
    if (msgChars[strlen(&msgChars[0]) - 1] != '\n')
        {
        (void) fprintf(fi, "\n");
        }
    }

/**
**  Construct a new Sptx connection.
**
**  Plan to double-buffer at most the maxSptLength, but never more than
**  the minSptdLength, bytes of data.  Substitute a default if the
**  given maxSptLength is negative.
**
**  Return null to fail, else don't.
**/

Sptx * newSptx(int maxSptLength)
    {

    /* Limit the double-buffer byte length. */
    /* Win KB says SPT below 16 KiB, SPTD above 16 KiB, no comment at 16 KiB. */

    if (maxSptLength < 0) maxSptLength = 16 * Ki;

    /* Plan to say SPTD only when asked to copy more. */

    int minSptdLength = (maxSptLength + 1);

    /* Alloc and zero and remember a new struct. */

    Sptx * sptx = (Sptx *) calloc(1, sizeof (Sptx));
    if (sptx == NULL) return NULL;

    int length = (sizeof (SptxCommand) - (sizeof sptx->theSptxCommand->ucData));
    SptxCommand * sptxCommand
        = (SptxCommand *) calloc(1, length + maxSptLength);
    if (sptxCommand == NULL)
        {
        free(sptx); sptx = NULL;
        return NULL;
        }

    sptx->theSptxCommand = sptxCommand;
    sptx->theMaxSptLength = maxSptLength;
    sptx->theMinSptdLength = minSptdLength;

    /* Choose defaults, say closed. */

    sptx->theErrFile = stderr;

    sptx->theHandle = INVALID_HANDLE_VALUE;
    sptx->theMaxLength = -1;
    sptx->theAlignmentMask = -1;

    sptx->theMaxSenseLength = -1;
    sptx->theMinSeconds = -1;

    sptx->theExitInt = -1;
    sptx->theLength = -1;
    sptx->theSenseChars = NULL;
    sptx->theSenseLength = -1;

    sptx->theReadNameInt = 'C';

    /* Succeed. */

    return sptx;
    }

/**
**  Redirect the stderr of this Sptx connection.
**/

void sptxSetErr(Sptx * sptx, FILE * fi)
    {
    if (sptx == NULL) return;
    if (fi == NULL) fi = stderr;
    sptx->theErrFile = fi;
    }

/**
**  Try to grow to allow double-buffer of maxSptLength, if need be.
**/

static void sptxHintMaxLength(Sptx * sptx, int maxSptLength)
    {
    if (sptx == NULL) return;
    if (maxSptLength < 0) return;

    /* Maybe grow, never shrink. */

    int wasMaxSptLength = sptx->theMaxSptLength;
    if (maxSptLength <= wasMaxSptLength) return;

    SptxCommand * sptxCommand = sptx->theSptxCommand;
    if (sptxCommand == NULL) return;

    int length = (sizeof (SptxCommand) - (sizeof sptxCommand->ucData));
    sptxCommand = (SptxCommand *) realloc(sptxCommand, length + maxSptLength);
    if (sptxCommand != NULL)
        {

        /* Acknowledge growth. */

        sptx->theSptxCommand = sptxCommand;
        sptx->theMaxSptLength = maxSptLength;

        /* Zero the new data. */

        if (wasMaxSptLength < maxSptLength) /* rarely false */
            {
            unsigned char * uchars = &sptxCommand->ucData[0];
            (void) memset(&uchars[wasMaxSptLength], '\0', (maxSptLength - wasMaxSptLength));
            }
        }
    }

/**
**  Close this Sptx connection.
**/

void sptxClose(Sptx * sptx)
    {
    if (sptx == NULL) return;
    HANDLE hDevice = sptx->theHandle;
    if (hDevice != INVALID_HANDLE_VALUE)
        {
        BOOL fCloseHandle = CloseHandle(hDevice);
        if (!fCloseHandle)
            {
            FILE * fi = sptx->theErrFile;
            win_fperror(fi, "// sptxClose.CloseHandle");
            }
        sptx->theHandle = INVALID_HANDLE_VALUE;
        }
    }

/**
**  Open this Sptx connection.
**
**  Return zero to succeed, else don't.
**/

int sptxOpen(Sptx * sptx, char const * name)
    {
    if (sptx == NULL) return -1;
    if (name == NULL) return -1;

    /* Close always. */

    sptxClose(sptx);

    /* Open a "\\\\.\\$letter:" name else die. */

    LPCTSTR lpFileName =            name;
    DWORD dwDesiredAccess =         (GENERIC_WRITE | GENERIC_READ);
    DWORD dwShareMode =             (FILE_SHARE_WRITE | FILE_SHARE_READ);
    LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;
    DWORD dwCreationDispostion =    OPEN_EXISTING;
    DWORD dwFlagsAndAttributes =    0;
    HANDLE hTemplateFile =          NULL;

    HANDLE hCreateFile = CreateFile(
        lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
        dwCreationDispostion, dwFlagsAndAttributes, hTemplateFile);

    sptx->theHandle = hCreateFile;

    if (hCreateFile == INVALID_HANDLE_VALUE)
        {
        FILE * fi = sptx->theErrFile;
        win_fperror(fi, "// sptxOpen.CreateFile");
        return -1;
        }

    /* Supply default hints. */

    sptx->theMaxLength = 0; /* 0 = unknown */
    sptx->theAlignmentMask = (sizeof (long) - 1); /* calloc may align more */

    /* Erase the entire speech. */

    IO_SCSI_CAPABILITIES ioScsiCapabilities;
    IO_SCSI_CAPABILITIES * isc = &ioScsiCapabilities;
    (void) memset(isc, '\0', sizeof *isc);

    /* Call DeviceIoControl (and wait indefinitely). */

    DWORD dwIoControlCode =         IOCTL_SCSI_GET_CAPABILITIES; /* often x41010 */
    LPVOID lpInBuffer =             isc;
    DWORD nInBufferSize =           sizeof *isc;
    LPOVERLAPPED lpOverlapped =     NULL; /* trust CreateFile of hDevice was without FILE_FLAG_OVERLAPPED */
    /**/
    LPVOID lpOutBuffer =            lpInBuffer;
    DWORD nOutBufferSize =          nInBufferSize;
    DWORD dwBytesReturned =         0;
    LPDWORD lpBytesReturned =       &dwBytesReturned;
    /**/
    BOOL fDeviceIoControl = DeviceIoControl(hCreateFile, dwIoControlCode,
        lpInBuffer, nInBufferSize,
        lpOutBuffer, nOutBufferSize,
        lpBytesReturned,
        lpOverlapped);

    /* Quietly succeed now if no info available e.g. Win2K Usb. */

    if (!fDeviceIoControl)
        {
//      FILE * fi = sptx->theErrFile;
//      win_fperror(fi, "// sptxOpen.DeviceIoControl.IOCTL_SCSI_GET_CAPABILITIES");
        return 0;
        }

    /* Fetch results. */

    int pageSize = PAGE_SIZE; /* often x1000 = 4096 = 4 * KiB */

    ULONG ulMaximumTransferLength =         isc->MaximumTransferLength;
    ULONG ulMaximumPhysicalPages =          isc->MaximumPhysicalPages;
    ULONG ulSupportedAsynchronousEvents =   isc->SupportedAsynchronousEvents;
    ULONG ulAlignmentMask =                 isc->AlignmentMask;
    BOOLEAN fTaggedQueuing =                isc->TaggedQueuing;
    BOOLEAN fAdapterScansDown =             isc->AdapterScansDown;
    BOOLEAN fAdapterUsesPio =               isc->AdapterUsesPio;

    /* Limit the sptxSay dataChars length. */

    int maxContiguousLength = ulMaximumTransferLength;
    int maxDiscontiguousLength = (pageSize * (ulMaximumPhysicalPages - 1));
    int maxLength = maxContiguousLength;
    if (maxDiscontiguousLength < maxContiguousLength)
        {
        maxLength = maxDiscontiguousLength;
        }
    sptx->theMaxLength = maxLength;

    /* Limit the sptxSay dataChars alignment. */

    sptx->theAlignmentMask = ulAlignmentMask;

    /* Discard the remaining details. */

    (void) ulSupportedAsynchronousEvents;
    (void) fTaggedQueuing;
    (void) fAdapterScansDown;
    (void) fAdapterUsesPio;

    /* Otherwise succeed. */

    return 0;
    }

/**
**  Limit the count of sense bytes copied In by sptxSay.
**
**  Return zero if ok, else negative.
**/

int sptxLimitSense(Sptx * sptx, int maxSenseLength)
    {
    if (sptx == NULL) return -1;

    if ((0 <= maxSenseLength) && (maxSenseLength <= MAX_SENSE_LENGTH))
        {
        sptx->theMaxSenseLength = maxSenseLength;
        return 0;
        }

    return -1;
    }

/**
**  Convert to s from (s, ns) else return negative.
**/

static int toSeconds(int s, int ns)
    {
    if ((0 <= ns) && (ns < G))
        {
        int sOfNs = ((ns + G - 1) / G);
        if ((0 <= s) && (s <= (INT_MAX - sOfNs)))
            {
            s += sOfNs;
            return s;
            }
        }
    return -1;
    }

/**
**  Specify the min time to wait before cancelling sptxSay.
**
**  Let negative time mean allow the max possible time.
**
**  Return zero if ok, else negative.
**/

int sptxLimitSeconds(Sptx * sptx, int s, int ns)
    {
    if (sptx == NULL) return -1;

    /* Round up to whole seconds, let negative mean max. */

    s = toSeconds(s, ns);
    if (s < 0) s = (28 * 60 * 60); /* 28 * 60 * 60 s = 28 hours = 1.125 days */

    /* Set a positive limit and succeed, else fail. */

    if (0 < s)
        {
        sptx->theMinSeconds = s;
        return 0;
        }
    return -1;
    }

/**
**  Decide the parameters for IOCTL_SCSI_PASS_THROUGH_DIRECT.
**
**  Return zero if ok, else negative.
**
**  See also sptSetCommand.
**/

static int sptdSetCommand(Sptx * sptx,
        char const * cdbChars, int cdbLength, char * dataChars, int maxLength, int direction)
    {
    if (sptx == NULL) return -1;

    /* Fetch implicit args. */

    int maxSenseLength = sptx->theMaxSenseLength;

    /* Constrain args. */

    if ((cdbChars == NULL) && (cdbLength != 0)) return -1;
    if ((cdbLength < 0) || (MAX_CDB_LENGTH < cdbLength)) return -1;
    if ((dataChars == NULL) && (maxLength != 0)) return -1;
    if ((maxSenseLength < 0) || (MAX_SENSE_LENGTH < maxSenseLength)) return -1;

    if (cdbLength != (UCHAR) cdbLength) return -1;
    if (maxSenseLength != (UCHAR) maxSenseLength) return -1;

    /* Erase the entire speech. */

    SptxCommand * sc = sptx->theSptxCommand;
    (void) memset(sc, '\0', sizeof *sc);

    /* Tag the speech. */

    SCSI_PASS_THROUGH_DIRECT * sph = &sc->sph.sptd; /* sph = sptx header */
    sph->Length = sizeof *sph;

    /* Ask to copy all the Cdb Out. */

    (void) memmove(&sph->Cdb[0], cdbChars, cdbLength);
    sph->CdbLength = (UCHAR) cdbLength;

    /* Ask to copy all the Data In or Out. */

    switch (direction)
        {

        case X2_DATA_OUT:
            sph->DataBuffer = dataChars;
            sph->DataTransferLength = maxLength;
            sph->DataIn = SCSI_IOCTL_DATA_OUT; /* 0 = SCSI_IOCTL_DATA_OUT */
            break;

        case X1_DATA_IN:
            sph->DataBuffer = dataChars;
            sph->DataTransferLength = maxLength;
            sph->DataIn = SCSI_IOCTL_DATA_IN; /* 1 = SCSI_IOCTL_DATA_IN */
            break;

        /* Ask to copy No Data in the last of many possible ways. */

        case X0_DATA_NOT:
        default:
            sph->DataBuffer = NULL;
            sph->DataBuffer = dataChars; /* maybe NULL */
            sph->DataTransferLength = 0;
            sph->DataTransferLength = maxLength; /* often 0 */
            sph->DataIn = SCSI_IOCTL_DATA_UNSPECIFIED; /* 2 = SCSI_IOCTL_DATA_UNSPECIFIED */
            sph->DataIn = SCSI_IOCTL_DATA_IN; /* 1 = SCSI_IOCTL_DATA_IN */
            sph->DataIn = SCSI_IOCTL_DATA_OUT; /* 0 = SCSI_IOCTL_DATA_OUT */
            break;
        }

    /* Ask to copy some Sense data In, if available. */

    sph->SenseInfoOffset = offsetof(SptxCommand, ucSense);
    sph->SenseInfoLength = (UCHAR) maxSenseLength;

    /* Leave much zeroed. */

    ; /* sph->TimeOutValue */
    ; /* sph->PathId sph->TargetId sph->Lun */
    ; /* sph->ScsiStatus */

    return 0;
    }

/**
**  Decide the parameters for IOCTL_SCSI_PASS_THROUGH.
**
**  Return zero if ok, else negative.
**
**  See also sptdSetCommand.
**/

static int sptSetCommand(Sptx * sptx,
        char const * cdbChars, int cdbLength, char * dataChars, int maxLength, int direction)
    {
    if (sptx == NULL) return -1;

    /* Fetch implicit args. */

    int maxSenseLength = sptx->theMaxSenseLength;

    /* Constrain args. */

    if ((cdbChars == NULL) && (cdbLength != 0)) return -1;
    if ((cdbLength < 0) || (MAX_CDB_LENGTH < cdbLength)) return -1;
    if ((dataChars == NULL) && (maxLength != 0)) return -1;
    if ((maxSenseLength < 0) || (MAX_SENSE_LENGTH < maxSenseLength)) return -1;

    if (cdbLength != (UCHAR) cdbLength) return -1;
    if (maxSenseLength != (UCHAR) maxSenseLength) return -1;

    /* Erase the entire speech. */

    SptxCommand * sc = sptx->theSptxCommand;
    (void) memset(sc, '\0', sizeof *sc);

    /* Tag the speech. */

    SCSI_PASS_THROUGH * sph = &sc->sph.spt; /* sph = sptx header */
    sph->Length = sizeof *sph;

    /* Ask to copy all the Cdb Out. */

    (void) memmove(&sph->Cdb[0], cdbChars, cdbLength);
    sph->CdbLength = (UCHAR) cdbLength;

    /* Ask to copy all the Data In or Out. */

    switch (direction)
        {

        case X2_DATA_OUT:
            sph->DataBufferOffset = offsetof(SptxCommand, ucData);
            sph->DataTransferLength = maxLength;
            sph->DataIn = SCSI_IOCTL_DATA_OUT; /* 0 = SCSI_IOCTL_DATA_OUT */
            break;

        case X1_DATA_IN:
            sph->DataBufferOffset = offsetof(SptxCommand, ucData);
            sph->DataTransferLength = maxLength;
            sph->DataIn = SCSI_IOCTL_DATA_IN; /* 1 = SCSI_IOCTL_DATA_IN */
            break;

        /* Ask to copy No Data in the last of many possible ways. */

        case X0_DATA_NOT:
        default:
            sph->DataBufferOffset = 0;
            sph->DataBufferOffset = offsetof(SptxCommand, ucData);
            sph->DataTransferLength = 0;
            sph->DataTransferLength = maxLength; /* often 0 */
            sph->DataIn = SCSI_IOCTL_DATA_UNSPECIFIED; /* 2 = SCSI_IOCTL_DATA_UNSPECIFIED */
            sph->DataIn = SCSI_IOCTL_DATA_IN; /* 1 = SCSI_IOCTL_DATA_IN */
            sph->DataIn = SCSI_IOCTL_DATA_OUT; /* 0 = SCSI_IOCTL_DATA_OUT */
            break;
        }

    /* Ask to copy some Sense data In, if available. */

    sph->SenseInfoOffset = offsetof(SptxCommand, ucSense);
    sph->SenseInfoLength = (UCHAR) maxSenseLength;

    /* Leave much zeroed. */

    ; /* sph->TimeOutValue */
    ; /* sph->PathId sph->TargetId sph->Lun */
    ; /* sph->ScsiStatus */

    return 0;
    }

/**
**  Speak a sentence of Scsi through an Sptx connection.
**
**  That is, copy the cdbLength of the cdbChars Out, copy between zero
**  and the maxLength of the dataChars In or Out, copy status In, and
**  copy sense bytes In if available.
**
**  Return zero if all was ok, positive to say specifically how many
**  less than all data bytes were copied, else negative.  Also return
**  zero if all was ok except that the number of data bytes copied is
**  unknown.
**/

int sptxSay(Sptx * sptx,
        char const * cdbChars, int cdbLength, char * dataChars, int maxLength, int direction)
    {
    if (sptx == NULL) return -1;
    if (sptx->theHandle == INVALID_HANDLE_VALUE) return -1;

    /* Fail by default. */
    /* And say the data length and sense length are unknown. */

    sptx->theExitInt = -1;
    sptx->theLength = -1;
    sptx->theSenseChars = NULL;
    sptx->theSenseLength = -1;

    /* Speak SPT below theMinSptdLength, else SPTD. */

    DWORD dwIoControlCode = IOCTL_SCSI_PASS_THROUGH_DIRECT; /* often x4D014 */
    if (maxLength < sptx->theMinSptdLength)
        {
        sptxHintMaxLength(sptx, maxLength);
        if (sptx->theMaxSptLength < maxLength) return -1;
        dwIoControlCode = IOCTL_SCSI_PASS_THROUGH; /* often x4D004 */
        }

    /* Fill the DeviceIoControl struct. */

    int maxSenseLength = sptx->theMaxSenseLength;
    int exitInt;
    if (dwIoControlCode == IOCTL_SCSI_PASS_THROUGH)
        {
        exitInt = sptSetCommand(sptx,
            cdbChars, cdbLength, dataChars, maxLength, direction);
        }
    else
        {
        exitInt = sptdSetCommand(sptx,
            cdbChars, cdbLength, dataChars, maxLength, direction);
        }
    if (exitInt != 0) return exitInt;

    /* Decide what to copy Out via DeviceIoControl. */

    SptxCommand * sc = sptx->theSptxCommand;
    SCSI_PASS_THROUGH * spth = &sc->sph.spt;
    SCSI_PASS_THROUGH_DIRECT * sptdh = &sc->sph.sptd;

    LPVOID lpInBuffer =             sc;
    DWORD nInBufferSize =           (sizeof (SptxCommand) - (sizeof sptx->theSptxCommand->ucData));
    LPOVERLAPPED lpOverlapped =     NULL; /* trust CreateFile of hDevice was without FILE_FLAG_OVERLAPPED */
    if (dwIoControlCode == IOCTL_SCSI_PASS_THROUGH)
        {
        nInBufferSize += maxLength; /* rarely needed if not X2_DATA_OUT */
        }

    /* Decide what to copy In via DeviceIoControl. */

    LPVOID lpOutBuffer =            lpInBuffer;
    DWORD nOutBufferSize =          nInBufferSize; /* rarely needed if not X1_DATA_IN */
    DWORD dwBytesReturned =         0;
    LPDWORD lpBytesReturned =       &dwBytesReturned;

    /* Limit time. */

    int s = sptx->theMinSeconds;
    if (s < 0) return -1;

    if (dwIoControlCode != IOCTL_SCSI_PASS_THROUGH)
        {
        sptdh->TimeOutValue = s;
        }
    else
        {
        spth->TimeOutValue = s;
        }

    /* Copy before to decide any data the Cdb does not copy In. */
    /* Rarely needed if not X2_DATA_OUT. */

    if (dwIoControlCode == IOCTL_SCSI_PASS_THROUGH)
        {
        (void) memmove(&sc->ucData[0], dataChars, maxLength);
        }

    /* Speak thru DeviceIoControl. */

    HANDLE hCreateFile = sptx->theHandle;
    BOOL fDeviceIoControl = DeviceIoControl(hCreateFile, dwIoControlCode,
        lpInBuffer, nInBufferSize,
        lpOutBuffer, nOutBufferSize,
        lpBytesReturned,
        lpOverlapped);

    /* Fetch more results. */

    UCHAR sphScsiStatus = sptdh->ScsiStatus;
    ULONG sphDataTransferLength = sptdh->DataTransferLength;
    UCHAR sphSenseInfoLength = sptdh->SenseInfoLength;
    if (dwIoControlCode == IOCTL_SCSI_PASS_THROUGH)
        {
        sphScsiStatus = spth->ScsiStatus;
        sphDataTransferLength = spth->DataTransferLength;
        sphSenseInfoLength = spth->SenseInfoLength;
        }

    /* Copy after to collect any data the Cdb does copy In. */
    /* Rarely needed if not X1_DATA_IN. */

    if (dwIoControlCode == IOCTL_SCSI_PASS_THROUGH)
        {
        (void) memmove(dataChars, &sc->ucData[0], maxLength);
        }

    /* Complain of utter failure. */

    if (!fDeviceIoControl)
        {
        exitInt = -1;
        FILE * fi = sptx->theErrFile;
        win_fperror(fi, "// sptxSay.DeviceIoControl");
        }
    else
        {

        /* Count and complain of data bytes Not copied. */

        exitInt = -1;
        int length = (int) sphDataTransferLength;
        if ((0 <= length) && (length <= maxLength))
            {
            sptx->theLength = length;
            exitInt = (maxLength - length);
            }

        /* Complain of sense bytes copied. */

        if (sphScsiStatus == 0x02) /* x02 = STATUS_CHKCOND */
            {
            int senseLength = sphSenseInfoLength;
            if (0 < exitInt) exitInt = -X0100_RESIDUE;
            exitInt = -((-exitInt) | X0002_SENSE);

            /* Count sense bytes copied. */

            if ((0 <= senseLength) && (senseLength <= maxSenseLength))
                {
                char const * senseChars = (char const *) &sc->ucSense[0];
                sptx->theSenseChars = senseChars;
                sptx->theSenseLength = senseLength;
                }
            else
                {
                exitInt = -1; /* sense not intelligible */
                }
            }
        }

    /* Proceed. */

    sptx->theExitInt = exitInt;
    if (exitInt == -1)
        {
        FILE * fi = sptx->theErrFile;
        (void) fprintf(fi,
            "// sptxSay x %lX %lX %lX %lX (ioctl status length senseLength)\n",
            (long) fDeviceIoControl,
            (long) sphScsiStatus,
            (long) sphDataTransferLength,
            (long) sphSenseInfoLength);
        }
    return exitInt;
    }

/**
**  Count the data bytes copied In or Out by sptxSay.
**
**  Guess the elseLength if the actual length of data bytes copied is
**  unknown.  Also if the sptx is null.
**/

int sptxGetLength(Sptx * sptx, int elseLength)
    {
    if (sptx == NULL) return elseLength;
    int length = sptx->theLength;
    if (length < 0) length = elseLength;
    return length;
    }

/**
**  Copy some or all of the sense bytes copied by sptxSay.
**
**  Guess the elseLength were copied if the length copied is unknown,
**  except guess 0 if sptx is null.
**
**  Copy the min of the charsLength with the actual or guessed copy
**  length.
**
**  Return the count of bytes copied.
**/

int sptxGetSense(Sptx * sptx, char * chars, int charsLength, int elseLength)
    {
    if (sptx == NULL) return 0;

    /* Guess the elseLength if the length of sense bytes copied is unknown. */

    int length = sptx->theSenseLength;
    if (length < 0) length = elseLength;

    /* Copy the min of the charsLength with the actual or guessed copy length. */

    int copyLength = length;
    if (charsLength < copyLength) copyLength = charsLength;
    if ((chars != NULL) && (0 < copyLength))
        {
        (void) memmove(chars, sptx->theSenseChars, copyLength);
        }

    /* Return the count of bytes copied. */

    return copyLength;
    }

/**
**  Read the first else the next well-known device path name.
**
**  That is, read names from the list:
**
**      C: .. Z:
**
**  Copy a name of up to charsLength to the chars.
**
**  Return the positive length of chars assigned, else zero to say the
**  name was too long (or if chars is null), else negative if no more
**  names exist (or if sptx is null).
**/

int sptxReadName(Sptx * sptx, char * chars, int charsLength)
    {
    if (sptx == NULL) return -1;

    /* Erase the name always. */

    if ((chars != NULL) && (0 < charsLength)) chars[0] = '\0';

    /* Close always. */

    sptxClose(sptx);

    /* Return negative after the last name. */

    int readNameInt = sptx->theReadNameInt;
    if ('Z' < readNameInt) return -1;

    /* Read a "\\\\.\\$letter:" name. */

    sptx->theReadNameInt = (readNameInt + 1);

    /* Print the name. */

    char name[] = "\\\\.\\Z:";
    (void) sprintf(&name[0], "\\\\.\\%c:", (char) readNameInt);

    /* Copy the name if it fits. */

    int copyLength = (int) (strlen(name) + 1);

    if (chars != NULL)
    if (copyLength <= charsLength)
        {
        (void) memmove(chars, &name[0], copyLength);
        return copyLength;
        }

    /* Else copy nothing and return zero. */

    return 0;
    }

/**
**  Accept a -X option of a Scsi command line.
**
**  Return zero if ok, else negative.
**/

int sptxSwallowArg(Sptx * sptx, char const * arg)
    {
    if (sptx == NULL) return -1;
    if (arg == NULL) return -1;

    if (stricmp(arg, "SPT") == 0)
        {
        sptx->theMinSptdLength = INT_MAX;
        return 0;
        }
    if (stricmp(arg, "SPTD") == 0)
        {
        sptx->theMinSptdLength = 0;
        return 0;
        }
    if (stricmp(arg, "SPTX") == 0)
        {
        sptx->theMinSptdLength = (sptx->theMaxSptLength + 1);
        return 0;
        }

    return -1;
    }

#endif /* SPTX */

/**
**  Tbd:
**
**  Measure the cost of the many SPT copies: (1) data to struct, (2)
**  user to kernel, (3) kernel to user, (4) struct to data.
**
**  Add getter methods for: theMaxLength the AlignmentMask
**  Don't just toss: ulSupportedAsynchronousEvents
**  Don't just toss: fTaggedQueuing fAdapterScansDown fAdapterUsesPio
**
**  Add destructor to free all that was calloc'ed.
**/

/* end of file */
