/**
**  winaspi.cpp
**  Saturday. September 21, 2002
**
**  Talk thru an Aspi connection of Microsoft Windows.
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
**  Link with Microsoft Windows and the Adaptec SDK.
**/

#ifdef WINASPI

#include <windows.h>

#include "wnaspi32.h"

typedef SRB_Abort SRB;

char const * theAspiLibName = "WNASPI32.DLL";
HINSTANCE hWnaspi32;

DWORD (*pfnGetASPI32SupportInfo)(void);
DWORD (*pfnSendASPI32Command)(LPSRB);

BOOL (*pfnGetASPI32Buffer)(PASPI32BUFF);
BOOL (*pfnFreeASPI32Buffer)(PASPI32BUFF);
BOOL (*pfnTranslateASPI32Address)(PDWORD, PDWORD);

int theAspiMaxHa = -1;

/**
**  Describe a Windows Aspi connection to a device.
**/

#define MAX_CDB_LENGTH      (sizeof ((SRB_ExecSCSICmd *) 0)->CDBByte) /* x10 */
#define MAX_SENSE_LENGTH    0xFF

typedef struct SRB_ExecSCSICmd_Max SRB_ExecSCSICmd_Max;

struct SRB_ExecSCSICmd_Max
    {
    SRB_ExecSCSICmd     theCoreChars;
    char                theMaxSenseChars[MAX_SENSE_LENGTH + 2]; /* SENSE_LEN+2 more than necessary */
    };

typedef union AspiCommand AspiCommand;

union AspiCommand
    {
    SRB                     theSRB;
    SRB_HAInquiry           theSRB_HAInquiry;
    SRB_GDEVBlock           theSRB_GDEVBlock;
    SRB_ExecSCSICmd         theSRB_ExecSCSICmd;
    SRB_ExecSCSICmd_Max     theSRB_ExecSCSICmd_Max;
    SRB_Abort               theSRB_Abort;
    SRB_BusDeviceReset      theSRB_BusDeviceReset;
    SRB_GetDiskInfo         theSRB_GetDiskInfo;
    };

struct Aspi /* connection */
    {
    AspiCommand     theAspiCommand;

    FILE *          theErrFile;

    int             theHa;
    int             theTarget;
    int             theLun;

    int             theMayCountDataFlag;
    int             theMaxLength;
    int             theAlignmentMask;
    int             theMaxHa;
    int             theMaxTarget;
    int             theMaxLun;

    int             theMaxSenseLength;
    int             theMinMilliSeconds;

    int             theExitInt;
    int             theLength;
    char const *    theSenseChars;
    int             theSenseLength;
    };

/**
**  Substitute for <stdlib.h> perror, when the "No error" message that
**  the perror of Windows prints is less informative than desired.
**/

static void win_fperror(FILE * fi, char const * string)
    {
    if (fi == NULL) return;
    if (string == NULL) return;

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
**  Startup.
**
**  Return negative to fail, else don't.
**/

static int aspiStartup(Aspi * aspi)
    {
    static int theAspiStartupInt = 0;

    /* Maybe quickly & gracefully do nothing. */

    if (theAspiStartupInt != 0) return theAspiStartupInt;
    theAspiStartupInt = -1;

    /* Require that the structs of "wnaspi32.h" be packed. */

    static int aspiCdbOffset = offsetof(SRB_ExecSCSICmd, CDBByte);
    if (aspiCdbOffset != 0x30)
        {
        FILE * fi = aspi->theErrFile;
        (void) fprintf(fi, "// aspiStartup: struct SRB_ExecSCSICmd not packed\n");
        return -1;
        }

    /* Link each routine. */

    hWnaspi32 = LoadLibrary(theAspiLibName);
    if (hWnaspi32 == NULL)
        {
        FILE * fi = aspi->theErrFile;
        win_fperror(fi, "// aspiStartup.LoadLibrary");
        (void) fprintf(fi, "// \"%s\" was the \"specified module\"\n", theAspiLibName);
        return -1;
        }

    /* Link each required routine. */

    FARPROC fp;

    fp = GetProcAddress(hWnaspi32, "GetASPI32SupportInfo");
    if (fp == NULL)
        {
        FILE * fi = aspi->theErrFile;
        win_fperror(fi, "// aspiStartup.GetProcAddress.GetASPI32SupportInfo");
        return -1;
        }
    pfnGetASPI32SupportInfo = (DWORD (*)(void)) fp;

    fp = GetProcAddress(hWnaspi32, "SendASPI32Command");
    pfnSendASPI32Command = (DWORD (*)(LPSRB)) fp;
    if (fp == NULL)
        {
        FILE * fi = aspi->theErrFile;
        win_fperror(fi, "// aspiStartup.GetProcAddress.SendASPI32Command");
        return -1;
        }

    /* Link each optional routine. */

    fp = GetProcAddress(hWnaspi32, "GetASPI32Buffer");
    pfnGetASPI32Buffer = (BOOL (*)(PASPI32BUFF)) fp;

    fp = GetProcAddress(hWnaspi32, "FreeASPI32Buffer");
    pfnFreeASPI32Buffer = (BOOL (*)(PASPI32BUFF)) fp;

    fp = GetProcAddress(hWnaspi32, "TranslateASPI32Address");
    pfnTranslateASPI32Address = (BOOL (*)(PDWORD, PDWORD)) fp;

    /* Startup Aspi and decide the max ha of "$ha.$target.$lun". */

    int rc = (pfnGetASPI32SupportInfo)();
    int rcSrbStatus = ((rc >> 8) & 0xFF);
    int rcHaCount = ((rc >> 0) & 0xFF);
    if (rcSrbStatus != SS_COMP) /* x01 = SS_COMP */
        {
        FILE * fi = aspi->theErrFile;
        (void) fprintf(fi, "// aspiStartup.GetASPI32SupportInfo = x%lX (%ld)\n",
            (long) rc, (long) rc);
        return -1;
        }

    theAspiMaxHa = (rcHaCount - 1);

    /* Succeed, no matter theAspiMaxHa. */

    theAspiStartupInt = 1;
    return 0;
    }

/**
**  Guess hints always, then ask for substitute hints.
**
**  Return zero if ok, else negative.
**/

int aspiExamine(Aspi * aspi, int ha)
    {
    if (aspi == NULL) return -1;
    if (ha != (BYTE) ha) return -1;

    /* Guess results. */

    aspi->theMayCountDataFlag = 0;
    aspi->theMaxLength = -1; /* -1 = unknown */
    aspi->theAlignmentMask = (sizeof (long) - 1);
    aspi->theMaxHa = theAspiMaxHa;
    aspi->theMaxTarget = (8 - 1);
    aspi->theMaxLun = (8 - 1);

    /* Fail if link failed. */

    if (aspiStartup(aspi) < 0) return -1;

    /* Erase an entire speech. */

    AspiCommand aspiCommand;
    AspiCommand * ac = &aspiCommand;
    SRB * srb = &ac->theSRB;
    SRB_HAInquiry * hs = &ac->theSRB_HAInquiry; /* "hs" = "HAI Srb" */

    (void) memset(ac, '\0', sizeof *ac);
    hs->SRB_Cmd = SC_HA_INQUIRY;

    /* Examine a particular haId. */

    hs->SRB_HaId = (BYTE) ha;

    /* Call Aspi. */

    DWORD dwReturned = (pfnSendASPI32Command)(srb);

    /* Totally disregard the value returned only if it is consistent. */

    DWORD hsStatus = hs->SRB_Status;
    if (dwReturned != hsStatus)
        {
        FILE * fi = aspi->theErrFile;
        (void) fprintf(fi, "// aspiExamine.SendASPI32Command = x%lX (%ld) != x%lX (%ld)\n",
            (long) dwReturned, (long) dwReturned, (long) hsStatus, (long) hsStatus);
        }

    /* Fail else fall through. */

    if (hsStatus != SS_COMP) /* x01 = SS_COMP */
        {
        FILE * fi = aspi->theErrFile;
        (void) fprintf(fi, "// aspiExamine.SRB_Status = x%lX (%ld)\n",
            (long) hsStatus, (long) hsStatus);
        return -1;
        }

    /* Fetch results. */

    WORD hsAlignmentMask =    * (WORD *) &hs->HA_Unique[0x00];
    BYTE hsMayCountData =     * (BYTE *) &hs->HA_Unique[0x02];
    BYTE hsTargetCount =      * (BYTE *) &hs->HA_Unique[0x03];
    DWORD hsMaxLength =       * (DWORD *) &hs->HA_Unique[0x04];

    /* Guess how to limit data bytes copied per Cdb. */

    aspi->theMaxLength = -1; /* -1 = unknown */
    if ((64 * Ki) < hsMaxLength)
        {
        aspi->theMaxLength = hsMaxLength;
        }

    /* Guess how to align data. */

    aspi->theAlignmentMask = hsAlignmentMask;

    /* Guess whether we may count data or not. */

    aspi->theMayCountDataFlag = 0;
    if (hsMayCountData == 0x01)
        {
        aspi->theMayCountDataFlag = 1;
        }

    /* Limit the range of Aspi device names to try by default. */

    aspi->theMaxHa = theAspiMaxHa;

    aspi->theMaxTarget = (8 - 1);
    if (hsTargetCount == 16)
        {
        aspi->theMaxTarget = (hsTargetCount - 1);
        }

    aspi->theMaxLun = (8 - 1);

    /* Succeed. */

    return 0;
    }

/**
**  Construct a new Aspi connection.
**
**  Return null to fail, else don't.
**/

Aspi * newAspi(void)
    {

    /* Alloc and zero a new struct. */

    Aspi * aspi = (Aspi *) calloc(1, sizeof (Aspi));
    if (aspi == NULL) return NULL;

    /* Choose defaults, say closed. */

    aspi->theErrFile = stderr;

    aspi->theHa = 0;
    aspi->theTarget = 0;
    aspi->theLun = -1;

    aspi->theMayCountDataFlag = -1;
    aspi->theMaxLength = -1;
    aspi->theAlignmentMask = -1;
    aspi->theMaxHa = -1;
    aspi->theMaxTarget = -1;
    aspi->theMaxLun = -1;

    aspi->theMaxSenseLength = -1;
    aspi->theMinMilliSeconds = -1;

    aspi->theExitInt = -1;
    aspi->theLength = -1;
    aspi->theSenseChars = NULL;
    aspi->theSenseLength = -1;

    /* Succeed. */

    return aspi;
    }

/**
**  Redirect the stderr of this Aspi connection.
**/

void aspiSetErr(Aspi * aspi, FILE * fi)
    {
    if (aspi == NULL) return;
    if (fi == NULL) fi = stderr;
    aspi->theErrFile = fi;
    }

/**
**  Open this Aspi connection with a new name.
**
**  Return zero to succeed, else don't.
**/

int aspiOpen(Aspi * aspi, char const * name)
    {
    if (aspi == NULL) return -1;
    if (name == NULL) return -1;

    /* Try to open "$ha.$target.$lun". */

    int exitInt = -1;

    int ha = 0;
    int target = 0;
    int lun = 0;
    char etcChar = '\0';
    if (sscanf(name, "%d.%d.%d%c", &ha, &target, &lun, &etcChar) == 3)
        {
        exitInt = 0;
        }

    /* Accept only names that SendASPI32Command can express. */

    if (exitInt == 0)
        {
        exitInt = -1;
        if ((0 <= ha) && (ha <= 0xFF))
        if ((0 <= target) && (target <= 0xFF))
        if ((0 <= lun) && (lun <= 0xFF))
            {
            exitInt = 0;
            }
        }

    /* Commit to a name else die. */

    if (exitInt != 0) return exitInt;

    aspi->theHa = ha;
    aspi->theTarget = target;
    aspi->theLun = lun;

    /* Fail if link failed. */

    if (aspiStartup(aspi) < 0) return -1;

    /* Try to get hints. */

    (void) aspiExamine(aspi, aspi->theHa);

    /* Succeed. */

    return 0;
    }

/**
**  Limit the count of sense bytes copied In by aspiSay.
**
**  Return zero if ok, else negative.
**/

int aspiLimitSense(Aspi * aspi, int maxSenseLength)
    {
    if (aspi == NULL) return -1;

    if ((0 <= maxSenseLength) && (maxSenseLength <= MAX_SENSE_LENGTH))
        {
        aspi->theMaxSenseLength = maxSenseLength;
        return 0;
        }

    return -1;
    }

/**
**  Convert to ms from (s, ns) else return negative.
**/

static int toMilliSeconds(int s, int ns)
    {
    if ((0 <= ns) && (ns < G))
        {
        int ms = ((ns + M - 1) / M);
        if ((0 <= s) && (s <= ((INT_MAX - ms) / K)))
            {
            int ms = (s * K);
            return ms;
            }
        }
    return -1;
    }

/**
**  Specify the min time to wait before cancelling aspiSay.
**
**  Let negative time mean allow the max possible time.
**
**  Return zero if ok, else negative.
**/

int aspiLimitSeconds(Aspi * aspi, int s, int ns)
    {
    if (aspi == NULL) return -1;

    /* Round up to whole seconds, let negative mean max. */

    int ms = toMilliSeconds(s, ns);
    if (ms < 0) ms = ((28 * 60 * 60) * 1000); /* 28 * 60 * 60 s = 28 hours = 1.125 days */

    /* Set a positive limit and succeed, else fail. */

    if (0 < ms)
        {
        aspi->theMinMilliSeconds = ms;
        return 0;
        }
    return -1;
    }

/**
**  Decide the parameters for SendASPI32Command.
**
**  Return zero if ok, else negative.
**/

static int aspiSetCommand(Aspi * aspi,
        char const * cdbChars, int cdbLength, char * dataChars, int maxLength, int direction)
    {
    if (aspi == NULL) return -1;

    /* Fetch implicit args. */

    int maxSenseLength = aspi->theMaxSenseLength;

    /* Constrain args. */

    if ((cdbChars == NULL) && (cdbLength != 0)) return -1;
    if ((cdbLength < 0) || (MAX_CDB_LENGTH < cdbLength)) return -1;
    if ((dataChars == NULL) && (maxLength != 0)) return -1;
    if ((maxSenseLength < 0) || (MAX_SENSE_LENGTH < maxSenseLength)) return -1;

    if (cdbLength != (BYTE) cdbLength) return -1;
    if (maxSenseLength != (BYTE) maxSenseLength) return -1;

    /* Erase the entire speech. */

    AspiCommand * ac = &aspi->theAspiCommand;
    (void) memset(ac, '\0', sizeof *ac);

    SRB_ExecSCSICmd * es = &ac->theSRB_ExecSCSICmd; /* es = escSrb = SRB_ExecSCSICmd */
    es->SRB_Cmd = SC_EXEC_SCSI_CMD; /* x02 = SC_EXEC_SCSI_CMD */

    /* Ask to copy all the Cdb Out. */

    (void) memmove(&es->CDBByte[0], cdbChars, cdbLength);
    es->SRB_CDBLen = (BYTE) cdbLength;

    /* Ask to copy all the Data In or Out. */

    switch (direction)
        {

        case X2_DATA_OUT:
            es->SRB_BufPointer = (BYTE *) dataChars;
            es->SRB_BufLen = maxLength;
            es->SRB_Flags = SRB_DIR_OUT; /* x10 = SRB_DIR_OUT */
            break;

        case X1_DATA_IN:
            es->SRB_BufPointer = (BYTE *) dataChars;
            es->SRB_BufLen = maxLength;
            es->SRB_Flags = SRB_DIR_IN; /* x08 = SRB_DIR_IN */
            break;

        /* Ask to copy No Data in the last of many possible ways. */

        case X0_DATA_NOT:
        default:
            es->SRB_BufPointer = (BYTE *) dataChars; /* maybe NULL */
            es->SRB_BufPointer = NULL;
            es->SRB_BufLen = 0;
            es->SRB_BufLen = maxLength; /* often 0 */
            es->SRB_Flags = SRB_DIR_OUT|SRB_DIR_IN; /* x18 = SRB_DIR_OUT|SRB_DIR_IN */
            es->SRB_Flags = SRB_DIR_OUT; /* x10 = SRB_DIR_OUT */
            es->SRB_Flags = SRB_DIR_IN; /* x08 = SRB_DIR_IN */
            es->SRB_Flags = 0;
            break;
        }

    /* Ask to count data bytes, if possible. */

    if (aspi->theMayCountDataFlag)
        {
        es->SRB_Flags |= SRB_ENABLE_RESIDUAL_COUNT; /* x04 = ... EN ... RESID */
        }

    /* Ask to copy some Sense data In, if available. */

    es->SRB_SenseLen = ((BYTE) maxSenseLength);

    /* Leave much zeroed. */

    ; /* es->SRB_HaId es->SRB_Target es->SRB_Lun */
    ; /* es->SRB_PostProc */
    ; /* es->SRB_Status es->SRB_HaStat es->SRB_TargStat */
    ; /* es->SRB_Hdr_Rsvd es->SRB_Rsvd1 es->SRB_Rsvd2 es->SRB_Rsvd3 */

    return 0;
    }

/**
**  Call SendASPI32Command but wait at least ms before asking to cancel.
**
**  Return zero if all ok, else negative.
**/

static int aspiSayForMilliSeconds(Aspi * aspi, int ms)
    {
    if (aspi == NULL) return -1;
    if (ms < 0) return -1;

    /* Acquire an event to wait on. */

    LPSECURITY_ATTRIBUTES lpEventAttributes = NULL;
    BOOL bManualReset = FALSE;
    BOOL bInitialState = FALSE;
    LPTSTR lpName = NULL;
    HANDLE hEvent = CreateEvent(lpEventAttributes, bManualReset, bInitialState, lpName);
    if (hEvent == NULL)
        {
        FILE * fi = aspi->theErrFile;
        win_fperror(fi, "// aspiSayForMilliSeconds.CreateEvent");
        return -1;
        }

    /* Set the device address. */

    AspiCommand * ac = &aspi->theAspiCommand;
    SRB_ExecSCSICmd * es = &ac->theSRB_ExecSCSICmd; /* es = escSrb = SRB_ExecSCSICmd */

    es->SRB_HaId = (BYTE) aspi->theHa;
    es->SRB_Target = (BYTE) aspi->theTarget;
    es->SRB_Lun = (BYTE) aspi->theLun;

    if (es->SRB_HaId != aspi->theHa) return -1;
    if (es->SRB_Target != aspi->theTarget) return -1;
    if (es->SRB_Lun != aspi->theLun) return -1;

    /* Send the Srb and wait for the event. */

    es->SRB_PostProc = (void *) hEvent;
    es->SRB_Flags |= SRB_EVENT_NOTIFY; /* x40 = SRB_EVENT_NOTIFY */
    SRB * srb = ((SRB *) es);
    DWORD dwReturned = (pfnSendASPI32Command)(srb);

    /* Totally disregard the value returned only if it is consistent. */

    DWORD srbStatus = srb->SRB_Status;
    if (dwReturned != srbStatus)
        {
        if ((dwReturned == SS_PENDING) && (srbStatus != SS_PENDING))
            {
            ; /* common = new value fetched after value returned */
            }
        else /* rare */
            {
            FILE * fi = aspi->theErrFile;
            (void) fprintf(fi, "// aspiSayForMilliSeconds.SendASPI32Command = x%lX (%ld) != x%lX (%ld)\n",
                (long) dwReturned, (long) dwReturned, (long) srbStatus, (long) srbStatus);
            ; /* leave exitInt unchanged */
            }
        }

    /* Limit how long Aspi may run. */
    /* Choose to think hEvent is free or not. */

    int exitInt = 0;
    if (srb->SRB_Status == SS_PENDING) /* x00 = SS_PENDING */
        {
        DWORD dwMilliseconds = ms;
        DWORD dwWfso = WaitForSingleObject(hEvent, dwMilliseconds);
        if (dwWfso != WAIT_OBJECT_0)
            {
            FILE * fi = aspi->theErrFile;
            if (dwWfso == WAIT_FAILED)
                {
                win_fperror(fi, "// aspiSayForMilliSeconds.WaitForSingleObject");
                }
            (void) fprintf(fi, "// aspiSayForMilliSeconds.WaitForSingleObject = x%lX (%ld)\n",
                (long) dwWfso, (long) dwWfso);
            exitInt = -1;
            }
        }

    /* Release the event acquired above. */

    BOOL fCh = CloseHandle(hEvent);
    if (fCh == 0)
        {
        FILE * fi = aspi->theErrFile;
        win_fperror(fi, "// aspiSayForMilliSeconds.CloseHandle");
        (void) fprintf(fi, "// aspiSayForMilliSeconds.CloseHandle = x%lX (%ld)\n",
            (long) fCh, (long) fCh);
        ; /* leave exitInt unchanged */
        }

    /* Proceed. */

    return exitInt;
    }

/**
**  Speak a sentence of Scsi through an Aspi connection.
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

int aspiSay(Aspi * aspi,
        char const * cdbChars, int cdbLength, char * dataChars, int maxLength, int direction)
    {
    if (aspi == NULL) return -1;

    /* Fail by default. */
    /* And say the data length and sense length are unknown. */

    aspi->theExitInt = -1;
    aspi->theLength = -1;
    aspi->theSenseChars = NULL;
    aspi->theSenseLength = -1;

    /* Remember what to say. */

    int mayCountDataFlag = aspi->theMayCountDataFlag;
    int maxSenseLength = aspi->theMaxSenseLength;
    int exitInt = aspiSetCommand(aspi,
        cdbChars, cdbLength, dataChars, maxLength, direction);
    if (exitInt != 0) return exitInt;

    /* Say it. */

    int ms = aspi->theMinMilliSeconds;
    if (ms < 0) return -1;
    int aspiInt = aspiSayForMilliSeconds(aspi, ms);

    /* Fetch more results. */

    AspiCommand * ac = &aspi->theAspiCommand;
    SRB_ExecSCSICmd * es = &ac->theSRB_ExecSCSICmd; /* es = escSrb = SRB_ExecSCSICmd */

    int esStatus = es->SRB_Status;
    int esHaStat = es->SRB_HaStat;
    int esTargStat = es->SRB_TargStat;
    int esBufLen = es->SRB_BufLen;
    int esSenseLen = es->SRB_SenseLen;

    /* Guess no error reported with error means no error. */
    /* Win98 Aspi requires this. */

    if (aspiInt == 0)
    if (esStatus == SS_COMP) /* x01 = SS_COMP */
        {
        esHaStat = HASTAT_OK; /* x00 = HASTAT_OK */
        esTargStat = 0x00; /* x00 = STATUS_GOOD */
        }

    /* Accept x12 HASTAT_DO_DU as a not-specific complaint of residue. */
    /* Tbd: list where x12 HASTAT_DO_DU occurs. */

    if (aspiInt == 0)
    if (esStatus == SS_ERR) /* x04 = SS_ERR */
    if (esHaStat == HASTAT_DO_DU) /* x12 = HASTAT_DO_DU */
        {
        esHaStat = HASTAT_OK; /* x00 = HASTAT_OK */
        if ((!mayCountDataFlag) || ((maxLength - esBufLen) == 0))
            {
            esBufLen = 0;
            mayCountDataFlag = 1;
            }
        }

    /* Guess timeout nonsensically reported with sense means no timeout. */
    /* Tbd: list where sense with timeout occurs. */

    if (aspiInt == 0)
    if (esStatus == SS_ERR) /* x04 = SS_ERR */
    if (esHaStat == HASTAT_SEL_TO) /* x11 = HASTAT_SEL_TO */
    if (esTargStat == 0x02) /* x02 = STATUS_CHKCOND */
        {
        esHaStat = HASTAT_OK; /* x00 = HASTAT_OK */
        }

    /* Say whether the (esStatus, esHaStat, esTargStat) is meaningful. */
    /* Disregard (esHaStat, esTargStat) with SS_COMP e.g. Win98 Aspi. */

    int meaningful = 0;
    if (aspiInt == 0)
        {
        if (esStatus == SS_COMP) /* x01 = SS_COMP */
            {
            meaningful = 1;
            }
        if (esStatus == SS_ERR) /* x04 = SS_ERR */
        if (esHaStat == HASTAT_OK) /* x00 = HASTAT_OK */
        if (esTargStat == 0x02) /* x02 = STATUS_CHKCOND */
            {
            meaningful = 1;
            }
        }

    /* Share & assess the results. */

    exitInt = -1;
    if (meaningful)
        {
        exitInt = 0;
        if (mayCountDataFlag)
            {
            exitInt = -1;
            int residue = esBufLen;
            if ((0 <= residue) && (residue <= maxLength))
                {
                aspi->theLength = (maxLength - residue);
                exitInt = residue;
                }
            }
        if (esStatus == SS_ERR) /* x04 = SS_ERR */
        if (esHaStat == HASTAT_OK) /* x00 = HASTAT_OK e.g. exclude x14 = HASTAT_PHASE_ERR */
        if (esTargStat == 0x02) /* x02 = STATUS_CHKCOND */
            {
            int senseLength = esSenseLen;
            if (0 < exitInt) exitInt = -X0100_RESIDUE;
            exitInt = -((-exitInt) | X0002_SENSE);

            if ((0 <= senseLength) && (senseLength <= maxSenseLength))
                {
                char const * senseChars = (char const *) &es->SenseArea[0];
                aspi->theSenseChars = senseChars;
                aspi->theSenseLength = senseLength;
                }
            else
                {
                exitInt = -1; /* sense not intelligible */
                }
            }
        }

    /* Proceed. */

    aspi->theExitInt = exitInt;
    if (exitInt == -1)
        {
        FILE * fi = aspi->theErrFile;
        (void) fprintf(fi,
            "// aspiSay: x %lX %02lX:%02lX:%02lX %lX %lX (aspi status BufLen SenseLen)\n",
            (long) aspiInt,
            (long) es->SRB_Status,
            (long) es->SRB_HaStat,
            (long) es->SRB_TargStat,
            (long) es->SRB_BufLen,
            (long) es->SRB_SenseLen);
        }
    return exitInt;
    }

/**
**  Count the data bytes copied In or Out by aspiSay.
**
**  Guess the elseLength if the actual length of data bytes copied is
**  unknown.  Also if the aspi is null.
**/

int aspiGetLength(Aspi * aspi, int elseLength)
    {
    if (aspi == NULL) return elseLength;
    int length = aspi->theLength;
    if (length < 0) length = elseLength;
    return length;
    }

/**
**  Copy some or all of the sense bytes copied by aspiSay.
**
**  Guess the elseLength were copied if the length copied is unknown,
**  except guess 0 if aspi is null.
**
**  Copy the min of the charsLength with the actual or guessed copy
**  length.
**
**  Return the count of bytes copied.
**/

int aspiGetSense(Aspi * aspi, char * chars, int charsLength, int elseLength)
    {
    if (aspi == NULL) return 0;

    /* Guess the elseLength if the length of sense bytes copied is unknown. */

    int length = aspi->theSenseLength;
    if (length < 0) length = elseLength;

    /* Copy the min of the charsLength with the actual or guessed copy length. */

    int copyLength = length;
    if (charsLength < copyLength) copyLength = charsLength;
    if ((chars != NULL) && (0 < copyLength))
        {
        (void) memmove(chars, aspi->theSenseChars, copyLength);
        }

    /* Return the count of bytes copied. */

    return copyLength;
    }

/**
**  Read the first else the next well-known device path name.
**
**  That is, read names from the list:
**
**      0.0.0 0.0.1 .. 0.1.0 .. 1.0.0 .. 255.255.255
**
**  Except do not exceed theMaxHa, theMaxTarget, theMaxLun.
**
**  Copy a name of up to charsLength to the chars.
**
**  Return the positive length of chars assigned, else zero to say the
**  name was too long (or if chars is null), else negative if no more
**  names exist (or if aspi is null).
**/

int aspiReadName(Aspi * aspi, char * chars, int charsLength)
    {
    if (aspi == NULL) return -1;

    /* Startup if need be. */

    if (aspiStartup(aspi) < 0) return -1;
    if (aspi->theMaxHa < 0) (void) aspiExamine(aspi, aspi->theHa);
    if (aspi->theMaxHa < 0) return -1;

    /* Buffer a name. */

    char name[] = "255.255.255";

    /* Run til break. */

    for (;;)
        {

        /* Read a "$ha.$target.$lun" name. */

        ++aspi->theLun;
        if (aspi->theMaxLun < aspi->theLun)
            {
            aspi->theLun = 0;
            ++aspi->theTarget;
            if (aspi->theMaxTarget < aspi->theTarget)
                {
                aspi->theTarget = 0;
                ++aspi->theHa;

                /* Else read past the last name. */

                if (aspi->theMaxHa < aspi->theHa) return -1;

                /* Try to get hints once per Ha. */

                (void) aspiExamine(aspi, aspi->theHa);
                if (aspi->theMaxHa < aspi->theHa) return -1;
                }
            }

        int ok = 0;
        if ((0 <= aspi->theHa) && (aspi->theHa<= aspi->theMaxHa))
        if ((0 <= aspi->theTarget) && (aspi->theTarget<= aspi->theMaxTarget))
        if ((0 <= aspi->theLun) && (aspi->theLun <= aspi->theMaxLun))
            {
            ok = 1;
            }
        if (!ok)
            {
            FILE * fi = aspi->theErrFile;
            (void) fprintf(fi, "aspiReadName: impossible name read");
            return -1;
            }

        /* Print the "$ha.$target.$lun" name. */

        (void) sprintf(&name[0], "%d.%d.%d",
            aspi->theHa, aspi->theTarget, aspi->theLun);

        break;
        }

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
**  Return nonzero if zs1 ends with zs2, ignoring case, else zero.
**/

static int zsEndsWithIgnoringCase(char const * zs1, char const * zs2)
    {
    int length1 = (int) strlen(zs1);
    int length2 = (int) strlen(zs2);
    if (length2 <= length1)
        {
        if (_memicmp(&zs1[length1 - length2], zs2, length2) == 0)
            {
            return 1;
            }
        }
    return 0;
    }

/**
**  Leak a copy of a NUL-terminated string.
**/

static char * newZString(char const * zs)
    {
    int length = (int) (strlen(zs) + 1);
    char * chars = (char *) calloc(1, length);
    if (chars == NULL) return NULL;
    (void) memmove(&chars[0], zs, length);
    chars[length] = '\0'; /* rarely changes */
    return chars;
    }

/**
**  Accept a -X option of a Scsi command line.
**
**  Return zero if ok, else negative.
**/

int aspiSwallowArg(Aspi * aspi, char const * arg)
    {
    if (aspi == NULL) return -1;
    if (arg == NULL) return -1;

    if (stricmp(arg, "ASPI") == 0)
        {
        return 0;
        }

    if (zsEndsWithIgnoringCase(arg, ".dll"))
        {
        char const * libName = newZString(arg);
        if (libName != NULL)
            {
            theAspiLibName = libName;
            return 0;
            }
        }

    return -1;
    }

#endif /* WINASPI */

/* end of file */
