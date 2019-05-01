/**
**  sgio.cpp
**  Saturday. September 21, 2002
**
**  Talk thru a Linux ioctl SG_IO connection to a device, since Linux
**  kernel 2.4 i.e. since <scsi/sg.h> version 3.
**/

/**
**  Link with standard C libraries.
**/

#include <errno.h> /* errno ... */
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
**  Link with Linux.
**
**  Also compile in Microsoft Windows if "winsg.h" is available there.
**/

#ifdef SGIO

#ifdef _WIN32
#include "winsg.h"
#else

#include <unistd.h>     /* close ... */
#include <fcntl.h>      /* open O_RDWR ... */
#include <sys/ioctl.h>  /* ioctl ... */

#include <scsi/sg.h>    /* SG_... sg_... */

#endif

#define MAX_DEV_HD  20  /* hda -> hdt */
#define MAX_DEV_SG  17  /* sg0 -> sg16 */
#define MAX_DEV_SCD 16  /* scd0 -> scd15 */

/**
**  Describe a Linux ioctl SG_IO connection to a device.
**/

#define MAX_CDB_LENGTH 0xFF
#define MAX_SENSE_LENGTH 0xFF

typedef struct SgioCommand SgioCommand;

struct SgioCommand
    {
    sg_io_hdr_t     theSgioHdr;
    char            theCdbChars[MAX_CDB_LENGTH];
    char            theSenseChars[MAX_SENSE_LENGTH];
    };

struct Sgio /* connection */
    {
    SgioCommand     theSgioCommand;

    FILE *          theErrFile;

    int             theOpenMode;
    int             theFileDescriptor;
    int             theSgVersionNum;

    int             theMaxSenseLength;
    int             theMinMilliSeconds;

    int             theExitInt;
    int             theLength;
    char const *    theSenseChars;
    int             theSenseLength;

    int             theReadNameInt;
    };

/**
**  Construct a new Sgio connection.
**
**  Associate the given Scsi connection with the new Sgio connection.
**
**  Arrange for sgioSay to fail until sgioLimitSense and sgioLimitTime
**  are both called.
**
**  Return null to fail, else don't.  Do fail if scsi is null.
**/

Sgio * newSgio(void)
    {

    /* Alloc and zero a new struct. */

    Sgio * sgio = (Sgio *) calloc(1, sizeof (Sgio));
    if (sgio == NULL) return NULL;

    /* Choose defaults, say closed. */

    sgio->theErrFile = stderr;

//  sgio->theOpenMode = O_RDWR; /* O_RDONLY often works */
//  sgio->theOpenMode = O_RDONLY|O_NONBLOCK;
    sgio->theOpenMode = O_RDWR|O_NONBLOCK;
    sgio->theFileDescriptor = -1;
    sgio->theSgVersionNum = 0; /* 0 = unknown */

    sgio->theMaxSenseLength = -1;
    sgio->theMinMilliSeconds = -1;

    sgio->theExitInt = -1;
    sgio->theLength = -1;
    sgio->theSenseChars = NULL;
    sgio->theSenseLength = -1;

    sgio->theReadNameInt = 0;

    /* Succeed. */

    return sgio;
    }

/**
**  Redirect the stderr of this Sgio connection.
**/

void sgioSetErr(Sgio * sgio, FILE * fi)
    {
    if (sgio == NULL) return;
    if (fi == NULL) fi = stderr;
    sgio->theErrFile = fi;
    }

/**
**  Close this Sgio connection.
**
**  Gracefully do nothing if sgio is null.
**/

void sgioClose(Sgio * sgio)
    {
    if (sgio == NULL) return;
    int fd = sgio->theFileDescriptor;
    if (0 <= fd)
        {
        int closeInt = close(fd);
        if (closeInt != 0)
            {
            FILE * fi = sgio->theErrFile;
            (void) fprintf(fi, "// sgioClose.close: %s\n", strerror(errno));
            }
        sgio->theFileDescriptor = fd = -1;
        }
    }

/**
**  Open this Sgio connection with a new name.
**
**  Return zero to succeed, else don't.
**/

int sgioOpen(Sgio * sgio, char const * name)
    {
    if (sgio == NULL) return -1;
    if (name == NULL) return -1;

    /* Close always. */

    sgioClose(sgio);

    /* Open a "/dev/sg$number" name else die. */

    int mode = sgio->theOpenMode;
    int fd = open(name, mode);
    sgio->theFileDescriptor = fd;

    if (fd < 0)
        {
        FILE * fi = sgio->theErrFile;
        (void) fprintf(fi, "// sgioOpen.open: %s\n", strerror(errno));
        return -1;
        }

    /* Get hints else die. */

    int versionNum = 0;
    int ioctlInt = ioctl(fd, SG_GET_VERSION_NUM, &versionNum);

    if (ioctlInt < 0)
        {
        FILE * fi = sgio->theErrFile;
        (void) fprintf(fi, "// sgioOpen.ioctl: %s\n", strerror(errno));
        return -1;
        }

    /* Get a compatible version else die. */

    if (!(30000 <= versionNum)) return -1;

    /* Otherwise succeed. */

    return 0;
    }

/**
**  Limit the count of sense bytes copied In by sgioSay.
**
**  Return zero if ok, else negative.  Do succeed if closed.
**/

int sgioLimitSense(Sgio * sgio, int maxSenseLength)
    {
    if (sgio == NULL) return -1;

    if ((0 <= maxSenseLength) && (maxSenseLength <= MAX_SENSE_LENGTH))
        {
        sgio->theMaxSenseLength = maxSenseLength;
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
**  Specify the min time to wait before cancelling sgioSay.
**
**  Let negative time mean allow the max possible time.
**
**  Return zero if ok, else negative.
**/

int sgioLimitSeconds(Sgio * sgio, int s, int ns)
    {
    if (sgio == NULL) return -1;

    /* Round up to whole seconds, let negative mean max. */

    int ms = toMilliSeconds(s, ns);
    if (ms < 0) ms = ((28 * 60 * 60) * 1000); /* 28 * 60 * 60 s = 28 hours = 1.125 days */

    /* Set a positive limit and succeed, else fail. */

    if (0 < ms)
        {
        sgio->theMinMilliSeconds = ms;
        return 0;
        }
    return -1;
    }

/**
**  Remember what to say via ioctl SG_IO.
**
**  Return zero if ok, else negative.
**/

static int sgioSetCommand(Sgio * sgio,
        char const * cdbChars, int cdbLength, char * dataChars, int maxLength, int direction)
    {
    if (sgio == NULL) return -1;

    /* Fetch implicit args. */

    int maxSenseLength = sgio->theMaxSenseLength;

    /* Constrain args. */

    if ((cdbChars == NULL) && (cdbLength != 0)) return -1;
    if ((cdbLength < 0) || (MAX_CDB_LENGTH < cdbLength)) return -1;
    if ((dataChars == NULL) && (maxLength != 0)) return -1;
    if ((maxSenseLength < 0) || (MAX_SENSE_LENGTH < maxSenseLength)) return -1;

    if (UCHAR_MAX < cdbLength) return -1; /* UCHAR_MAX = max cmd_len */
    if (UCHAR_MAX < maxSenseLength) return -1; /* UCHAR_MAX = max mx_sb_len */

    /* Erase the entire speech. */

    SgioCommand * sc = &sgio->theSgioCommand;
    (void) memset(sc, '\0', sizeof *sc);

    sg_io_hdr_t * sih = &sc->theSgioHdr;
    sih->interface_id = 'S';

    /* Ask to copy all the Cdb Out. */

    sih->cmdp = (unsigned char *) &sc->theCdbChars[0];
    (void) memmove(sih->cmdp, cdbChars, cdbLength);
    sih->cmd_len = ((unsigned char) cdbLength);

    /* Ask to copy all the Data In or Out. */

    switch (direction)
        {

        case X2_DATA_OUT:
            sih->dxferp = dataChars;
            sih->dxfer_len = maxLength;
            sih->dxfer_direction = SG_DXFER_TO_DEV; /* Out to device */
            break;

        case X1_DATA_IN:
            sih->dxferp = dataChars;
            sih->dxfer_len = maxLength;
            sih->dxfer_direction = SG_DXFER_FROM_DEV; /* In from device */
            break;

        /* Ask to copy No Data in the last of many possible ways. */

        case X0_DATA_NOT:
        default:
            sih->dxferp = NULL;
            sih->dxferp = dataChars; /* maybe NULL */
            sih->dxfer_len = 0;
            sih->dxfer_len = maxLength; /* often zero */
            sih->dxfer_direction = SG_DXFER_NONE;
            break;
        }

    /* Ask to copy some Sense data In, if available. */

    sih->sbp = (unsigned char *) &sc->theSenseChars[0];
    sih->mx_sb_len = ((unsigned char) maxSenseLength);

    /* Leave much zeroed. */

    ; /* sih->iovec_count sih->flags sih->pack_id sih->usr_ptr */
    ; /* sih->status sih->masked_status sih->msg_status */
    ; /* sih->sb_len_wr sih->host_status sih->drive_status */
    ; /* sih->resid sih->duration sih->info */

    return 0;
    }

/**
**  Call ioctl SG_IO but wait at least ms before asking to cancel.
**
**  Return the int that the ioctl returned.
**/

static int sgioSayForMilliSeconds(Sgio * sgio, int ms)
    {
    if (sgio == NULL) return -1;
    if (ms < 0) return -1;

    SgioCommand * sc = &sgio->theSgioCommand;
    sg_io_hdr_t * sih = &sc->theSgioHdr;
    sih->timeout = ms;

    int fd = sgio->theFileDescriptor;
    int ioctlInt = ioctl(fd, SG_IO, sih);

    return ioctlInt;
    }

/**
**  Speak a sentence of Scsi through an Sgio connection.
**
**  That is, copy the cdbLength of the cdbChars Out, copy between zero
**  and the maxLength of the dataChars In or Out, copy status In, and
**  copy sense bytes In if available.
**
**  Return zero if all was ok, positive to say specifically how many
**  less than all data bytes were copied, else negative.
**
**  Also return zero if all was ok except that the number of data bytes
**  copied is unknown.
**
**  Do succeed if closed.
**/

int sgioSay(Sgio * sgio,
        char const * cdbChars, int cdbLength, char * dataChars, int maxLength, int direction)
    {
    if (sgio == NULL) return -1;
    if (sgio->theFileDescriptor < 0) return 0;

    /* Fail by default. */
    /* And say the data length and sense length are unknown. */

    sgio->theExitInt = -1;
    sgio->theLength = -1;
    sgio->theSenseChars = NULL;
    sgio->theSenseLength = -1;

    /* Remember what to say. */

    int maxSenseLength = sgio->theMaxSenseLength;
    int exitInt = sgioSetCommand(sgio,
        cdbChars, cdbLength, dataChars, maxLength, direction);
    if (exitInt != 0) return exitInt;

    /* Say it. */

    int ms = sgio->theMinMilliSeconds;
    if (ms < 0) return -1;
    int ioctlInt = sgioSayForMilliSeconds(sgio, ms);

    /* Fetch more results. */

    SgioCommand * sc = &sgio->theSgioCommand;
    sg_io_hdr_t * sih = &sc->theSgioHdr;

    int sihInfo = sih->info;
    int sihResid = sih->resid;
    int sihSbLenWr = sih->sb_len_wr;

    /* Complain of utter failure. */

    exitInt = -1;
    if (ioctlInt < 0)
        {
        FILE * fi = sgio->theErrFile;
        (void) fprintf(fi, "// sgioSay.ioctl: %s\n", strerror(errno));
        }
    else
        {

        /* Count and complain of data bytes Not copied. */

        exitInt = -1;
        int residue = sihResid;
        if ((0 <= residue) && (residue <= maxLength))
            {
            sgio->theLength = (maxLength - residue);
            exitInt = residue;
            }

        /* Complain of sense bytes copied. */

        if ((sihInfo & SG_INFO_OK_MASK) != SG_INFO_OK)
            {
            int senseLength = sihSbLenWr;
            if (0 < exitInt) exitInt = -X0100_RESIDUE;
            exitInt = -((-exitInt) | X0002_SENSE);

            /* Count sense bytes copied. */

            if ((0 <= senseLength) && (senseLength <= maxSenseLength))
                {
                char const * senseChars = &sc->theSenseChars[0];
                sgio->theSenseChars = senseChars;
                sgio->theSenseLength = senseLength;
                }
            else
                {
                exitInt = -1; /* sense not intelligible */
                }
            }
        }

    /* Proceed. */

    sgio->theExitInt = exitInt;
    if (exitInt == -1)
        {
        FILE * fi = sgio->theErrFile;
        (void) fprintf(fi,
            "// sgioSay x %lX %lX %lX %lX (ioctl sihInfo sihResid sihSbLenWr)\n",
            (long) ioctlInt, (long) sihInfo, (long) sihResid, (long) sihSbLenWr);
        }
    return exitInt;
    }

/**
**  Count the data bytes copied In or Out by sgioSay.
**
**  Guess the elseLength if the actual length of data bytes copied is
**  unknown.  Also if the sgio is null.
**/

int sgioGetLength(Sgio * sgio, int elseLength)
    {
    if (sgio == NULL) return elseLength;
    int length = sgio->theLength;
    if (length < 0) length = elseLength;
    return length;
    }

/**
**  Copy some or all of the sense bytes copied by sgioSay.
**
**  Guess the elseLength were copied if the length copied is unknown,
**  except guess 0 if sgio is null.
**
**  Copy the min of the charsLength with the actual or guessed copy
**  length.
**
**  Return the count of bytes copied.
**/

int sgioGetSense(Sgio * sgio, char * chars, int charsLength, int elseLength)
    {
    if (sgio == NULL) return 0;

    /* Guess the elseLength if the length of sense bytes copied is unknown. */

    int length = sgio->theSenseLength;
    if (length < 0) length = elseLength;

    /* Copy the min of the charsLength with the actual or guessed copy length. */

    int copyLength = length;
    if (charsLength < copyLength) copyLength = charsLength;
    if ((chars != NULL) && (0 < copyLength))
        {
        (void) memmove(chars, sgio->theSenseChars, copyLength);
        }

    /* Return the count of bytes copied. */

    return copyLength;
    }

/**
**  Read the first else the next well known device path name.
**
**  UPDATED: Recent Linux kernel handle standard devices as SGIO ones
**
**  Copy a name of up to charsLength to the chars.
**
**  Return the positive length of chars assigned, else zero to say the
**  name was too long (or if chars is null), else negative if no more
**  names exist (or if sgio is null).
**/

int sgioReadName(Sgio * sgio, char * chars, int charsLength)
    {
    if (sgio == NULL) return -1;

    /* Close always. */

    sgioClose(sgio);

    /* Return negative after the last name. */

    int readNameInt = sgio->theReadNameInt;
    if ((MAX_DEV_HD+MAX_DEV_SG+MAX_DEV_SCD) <= readNameInt) return -1;

    /* Read a "/dev/sg$number" name. */

    sgio->theReadNameInt = (readNameInt + 1);

    /* Print the name. */

    char name[] = "/dev/hda876543210";
    if (readNameInt < MAX_DEV_HD)
    {
    	name[7] += readNameInt;
    	name[8] = 0;
    }
    else if ((readNameInt - MAX_DEV_HD) < MAX_DEV_SG)
    	(void) sprintf(&name[0], "/dev/sg%ld", (long) (readNameInt - MAX_DEV_HD));
    else 
    	(void) sprintf(&name[0], "/dev/scd%ld", (long) (readNameInt - MAX_DEV_HD - MAX_DEV_SG));

    /* Copy the name if it fits. */

    int copyLength = (strlen(name) + 1);

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

int sgioSwallowArg(Sgio * sgio, char const * arg)
    {
    if (sgio == NULL) return -1;
    if (arg == NULL) return -1;

    if (strcmp(arg, "SGIO") == 0)
        {
        return 0;
        }

    return -1;
    }

#endif /* SGIO */

/* end of file */
