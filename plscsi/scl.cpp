/**
**  scl.cpp
**  Saturday. September 21, 2002
**
**  Parse and interpret a Scsi command line.
**
**  Grep views include:
**
**          grep "^[A-Za-z_]"               // subroutines
**/

/**
**  Link with standard C libraries.
**/

#include <stdio.h> /* FILE ... */
#include <stdlib.h> /* calloc ... */
#include <string.h> /* memset ... */

/**
**  Maybe link with more libraries.
**/

#if (_WIN32 || __MSDOS__)
#include <fcntl.h> /* O_BINARY ... */
#include <io.h>
#endif

#ifdef DJGPP /* Don't need this whole mumbo jumbo in 32 bit mode */
#define farcalloc calloc
#else
	#ifdef __MSDOS__
	#include <alloc.h> /* farcalloc ... */
	#endif
#endif


/**
**  Link with the *.c* of ../plscsi/.
**/

#include "plscsi.h"

/**
**  Define some units.
**/

#define Ki      1024 /* x1024 = x400 = 1 << 10 */

/**
**  Choose a default command.
**/

#define SCL_CDB "\x12\0\0\0\x24\0"
#define SCL_IN_LENGTH 0x24
#define SCL_OUT_LENGTH 0

#define SCL_SENSE_LENGTH 0xE /* offsetof SK ASC ASCQ < xE */
#define SCL_SECONDS -1 /* negative means max */

/**
**  Describe a Scsi command line.
**/

typedef struct ScsiCommandLine ScsiCommandLine;

struct ScsiCommandLine
    {
    struct CommandLine *    theCommandLine;
    int                     theHelpBit;
    int                     theVerboseBit;
    /**/
    int                     theLoopBit;
    int                     theWhicheverBit;
    char const * *          theNames;
    int                     theNamesLength;
    char const *            theName;
    Scsi *                  theScsi;
    FILE *                  theNullFile;
    /**/
    int                     theCdbGuessedBit;
    char const *            theCdbChars;
    int                     theCdbLength;
    INT                     theInLength;
    INT                     theOutLength;
    INT                     theDataLength;
    int                     theMaxSenseLength;
    INT                     theMinSeconds;
    INT                     theMinNanoSeconds;
    /**/
    INT                     theAlignLength;
    INT                     theAlignOffset;
    char const *            theFromFileName;
    char *                  theOutChars;
    char *                  theInChars;
    char const *            theToFileName;
    int                     theCompareBit;
//  int                     theSurveyBit;
    /**/
    int                     thePleaseBit;
    int                     theYesBit;
    int                     theNoBit;
    };

/**
**  Construct a new ScsiCommandLine.
**/

ScsiCommandLine * newScsiCommandLine(void)
    {
    ScsiCommandLine * scl = (ScsiCommandLine *) calloc(1, sizeof (ScsiCommandLine));
    if (scl == NULL) return NULL;

    CommandLine * cl = newCommandLine();
    if (cl == NULL) return NULL;

    Scsi * scsi = newScsi();
    if (scsi == NULL) return NULL;

    scl->theCommandLine = cl;
    scl->theHelpBit = 0;
    scl->theVerboseBit = 0;

    scl->theLoopBit = 0;
    scl->theWhicheverBit = 0;
    scl->theNames = NULL;
    scl->theNamesLength = 0;
    scl->theName = NULL;
    scl->theScsi = scsi;
    scl->theNullFile = NULL;

    scl->theCdbGuessedBit = 0;
    scl->theCdbChars = NULL;
    scl->theCdbLength = -1;
    scl->theInLength = -1;
    scl->theOutLength = -1;
    scl->theDataLength = -1;
    scl->theMaxSenseLength = -1;
    scl->theMinSeconds = -1;
    scl->theMinNanoSeconds = -1;

    scl->theAlignLength = -1;
    scl->theAlignOffset = -1;
    scl->theFromFileName = NULL;
    scl->theOutChars = NULL;
    scl->theInChars = NULL;
    scl->theToFileName = NULL;
    scl->theCompareBit = 0;
//  scl->theSurveyBit = 0;

    scl->thePleaseBit = 0;
    scl->theYesBit = 0;
    scl->theNoBit = 0;
    return scl;
    }

/**
**  Re/start the parse of the main args.
**/

void sclSet(ScsiCommandLine * scl, char const * const * argv, int argc)
    {
    clSet(scl->theCommandLine, argv, argc);
    }

/**
**  Re/start the parse of args from a string.
**/

void sclSetFromString(ScsiCommandLine * scl, char const * st)
    {
    clSetFromString(scl->theCommandLine, st);
    }

/**
**  Get the help bit.
**/

int sclGetHelpBit(ScsiCommandLine * scl)
    {
    return scl->theHelpBit;
    }

/**
**  Get the verbose bit.
**/

int sclGetVerboseBit(ScsiCommandLine * scl)
    {
    return scl->theVerboseBit;
    }

/**
**  Get the yes bit.
**/

int sclGetYesBit(ScsiCommandLine * scl)
    {
    return scl->theYesBit;
    }

/**
**  Get the no bit.
**/

int sclGetNoBit(ScsiCommandLine * scl)
    {
    return scl->theNoBit;
    }

/**
**  Convert to a concise -xyz switch from an equivalent verbose
**  --xxx switch and return the concise switch, else return -1.
**/

static int fromVerboseSwitch(char const * arg)
    {
    int switchInt = -1;
    if (arg != NULL)
        {
        if (strcmp("--align", arg) == 0)            switchInt = 'a';
//      //////////////////////////////////////////////////////// b
        else if (strcmp("--compare", arg) == 0)     switchInt = 'c';
        else if (strcmp("--data", arg) == 0)        switchInt = 'd';
//      //////////////////////////////////////////////////////// e
        else if (strcmp("--from", arg) == 0)        switchInt = 'f';
//      //////////////////////////////////////////////////////// g
        else if (strcmp("--help", arg) == 0)        switchInt = 'h';
        else if (strcmp("--in", arg) == 0)          switchInt = 'i';
//      //////////////////////////////////////////////////////// j
//      //////////////////////////////////////////////////////// k
        else if (strcmp("--loop", arg) == 0)        switchInt = 'l';
//      //////////////////////////////////////////////////////// m
        else if (strcmp("--no", arg) == 0)          switchInt = 'n';
        else if (strcmp("--out", arg) == 0)         switchInt = 'o';
        else if (strcmp("--please", arg) == 0)      switchInt = 'p';
        else if (strcmp("--quiet", arg) == 0)       switchInt = 'q';
//      //////////////////////////////////////////////////////// r
//      else if (strcmp("--survey", arg) == 0)      switchInt = 's';
        else if (strcmp("--to", arg) == 0)          switchInt = 't';
//      //////////////////////////////////////////////////////// u
        else if (strcmp("--verbose", arg) == 0)     switchInt = 'v';
        else if (strcmp("--whichever", arg) == 0)   switchInt = 'w';
        else if (strcmp("--x", arg) == 0)           switchInt = 'x';
        else if (strcmp("--X", arg) == 0)           switchInt = 'X';
        else if (strcmp("--yes", arg) == 0)         switchInt = 'y';
//      //////////////////////////////////////////////////////// z
        }
    return switchInt;
    }

/**
**  Consume another switch and return its bit mask, else return -1.
**
**  Consume either a concise -xyz switch or an equivalent verbose
**  --xxx.
**/

static int sclReadSwitch(ScsiCommandLine * scl)
    {
    CommandLine * cl = scl->theCommandLine;
    int switchInt = clReadSwitch(cl);
    if (switchInt == -1)
        {
        char const * arg = clPeek(cl);
        switchInt = fromVerboseSwitch(arg);
        if (0 <= switchInt)
            {
            clRead(cl);
            }
        }
    return switchInt; /* fail */
    }

/**
**  Consume a Cdb in hex and return zero or positive length.
**
**  Return the length consumed, else negative.
**/

static int sclReadCdb(ScsiCommandLine * scl)
    {
    CommandLine * cl = scl->theCommandLine;
    scl->theCdbChars = NULL;
    int length = 0;
    for (;;)
        {
        int ii = clReadXX(cl);
        if (ii < 0) break;

        ++length;
        char * chars = (char *) realloc((void *) scl->theCdbChars, length * sizeof *scl->theCdbChars);
        if (chars == NULL) return -1;
        chars[length - 1] = (char) ii;
        scl->theCdbChars = chars;
        }
    return length;
    }

/**
**  Consume a platform-specific option: a name, maybe a value, maybe
**  more values.
**/

static int sclReadXyz(ScsiCommandLine * scl)
    {
    CommandLine * cl = scl->theCommandLine;
    char const * arg = clPeek(cl);
    if (arg == NULL) return -1;

    if (strcmp("sense", arg) == 0)
        {
        clRead(cl);
        INT ii = clReadInt(cl);
        int maxSenseLength = (int) ii;
        if ((maxSenseLength < 0) || (maxSenseLength != ii)) return -1;
        scl->theMaxSenseLength = maxSenseLength;
        return 0;
        }

    if (strcmp("time", arg) == 0)
        {
        clRead(cl);
        INT ii = clReadInt(cl);
        if (ii < 0) return -1;
        scl->theMinSeconds = ii;
        scl->theMinNanoSeconds = 0;
        ii = clReadInt(cl);
        if (0 <= ii) scl->theMinNanoSeconds = ii;
        return 0;
        }

    if (scsiSwallowArg(scl->theScsi, arg) == 0)
        {
        clRead(cl);
        return 0;
        }

    return -1;
    }

/**
**  Parse part or all theArgs as a Scsi command line.
**
**  Return zero if ok, else negative trouble.
**/

int sclParse(ScsiCommandLine * scl)
    {
    CommandLine * cl = scl->theCommandLine;
    char const * arg = NULL;
    INT ii = 0;
    int cdbLength = 0;
    for (;;)
        {
        int switchInt = sclReadSwitch(scl);
        switch (switchInt)
            {

            /* Read a known switch. */

            case 'a': // --align $offset [ $length ]
                ii = clReadInt(cl);
                if (ii < 0) return -1;
                scl->theAlignOffset = ii;
                ii = clReadInt(cl);
                if (0 < ii) scl->theAlignLength = ii;
                break;

            case 'c': // --compare
                scl->theCompareBit = 1;
                break;

            case 'd': // --data $length
                ii = clReadInt(cl);
                if (ii < 0) return -1;
                scl->theDataLength = ii;
                break;

            case 'f': // --from $fileName
                arg = clRead(cl);
                if (arg == NULL) return -1;
                scl->theFromFileName = arg;
                break;

            case 'h': // --help
                scl->theHelpBit = 1;
                break;

            case 'i': // --in $length
                ii = clReadInt(cl);
                if (ii < 0) return -1;
                scl->theInLength = ii;
                break;

            case 'l': // --loop
                scl->theLoopBit = 1;
                scl->theNamesLength = 0;
                scl->theWhicheverBit = 0;
                break;

            case 'n': // --no
                scl->theNoBit = 1;
                break;

            case 'p': // --please
                scl->thePleaseBit = 1;
                break;

            case 'o': // --out $length
                ii = clReadInt(cl);
                if (ii < 0) return -1;
                scl->theOutLength = ii;
                break;

            case 'q': // --quiet
                scl->theVerboseBit = 0; /* 0 not 1 */
                break;

//          case 's': // --survey
//              scl->theSurveyBit = 1;
//              break;

            case 't': // --to $fileName
                arg = clRead(cl);
                if (arg == NULL) return -1;
                scl->theToFileName = arg;
                break;

            case 'v': // --verbose
                scl->theVerboseBit = 1;
                break;

            case 'w': // --whichever
                scl->theLoopBit = 0;
                scl->theNamesLength = 0;
                scl->theWhicheverBit = 1;
                break;

            case 'x': // --x $cdb
                cdbLength = sclReadCdb(scl);
                if (cdbLength < 0) return -1;
                scl->theCdbLength = cdbLength;
                break;

            case 'X': // --X $name [ $value ]
                ii = sclReadXyz(scl);
                if (ii < 0) return -1;
                break;

            case 'y': // --yes
                scl->theYesBit = 1;
                break;

            default:
                if (0 <= switchInt) return -1;
                break;
            }

        /* Read "--". */

        if (switchInt < 0)
        if (clReadComment(cl) < 0)
            {
            char const * arg = clRead(cl);
            if (arg == NULL) break;

            /* Reject "--xyz". */

            if ((arg[0] == '-') && (arg[1] == '-'))
                {
                return -1;
                }

            /* Remember anything else. */

            scl->theLoopBit = 0;

            int length = ++scl->theNamesLength;
            scl->theNames = (char const * * ) realloc((void *) scl->theNames, length * sizeof *scl->theNames);
            if (scl->theNames == NULL) return -1;
            scl->theNames[length - 1] = arg;

            scl->theWhicheverBit = scl->theWhicheverBit; /* no change */
            }
        }
    return 0; /* succeed */
    }

/**
**  Print some data.
**
**  Convert lossily to printable US-Ascii x20..7E.
**
**  Conserve the length of the chars, not their values.
**/

void sclDataPrint(FILE * fi, char const * dataChars, int dataLength)
    {
    if (fi != NULL)
    if (dataChars != NULL)
        {
        while (0 < dataLength--)
            {
            char cc = *dataChars++;
            cc &= 0x7F;
            if (!((0x20 <= cc) && (cc <= 0x7E)))
                {
                cc ^= 0x40;
                }
            (void) fputc(cc, fi);
            }
        }
    }

/**
**  Print a one-line summary of op x12 Inquiry data.  Print an empty
**  line if less than x24 bytes were available and copied In.
**
**  An example output line is:
**
**      @ LG       DVD-ROM DRN8080B 1.11
**/

void sclInquiryPrintln(FILE * fi, char const * dataChars, INT dataLength)
    {
    if (fi != NULL)
    if (dataChars != NULL)
    if (0x23 < dataLength)
        {

        int additionalLength = (dataChars[4] & 0xFF);
        int availableLength = (4 + (sizeof dataChars[4]) + additionalLength);
        int supposedLength = availableLength;
        if ((additionalLength == 0) || (0xFF < availableLength)) supposedLength = 0x24;

        int visibleLength = ((dataLength < supposedLength) ? ((int) dataLength) : supposedLength);
        if (0x23 < visibleLength)
            {
            sclDataPrint(fi, &dataChars[2], 1); /* version of Scsi */
            (void) fputc(' ', fi);
            sclDataPrint(fi, &dataChars[8], 8); /* vendorIdentification */
            (void) fputc(' ', fi);
            sclDataPrint(fi, &dataChars[0x10], 0x10); /* productIdentification */
            (void) fputc(' ', fi);
            sclDataPrint(fi, &dataChars[0x20], 4); /* productRevisionLevel */
            if (supposedLength != availableLength)
                {
                (void) fprintf(fi, " maybe");
                }
            }
        }
    (void) fprintf(fi, "\n");
    }

/**
**  Print a one-line summary of the op x12 Inquiry data of a device name.
**
**  An example output line is:
**
**      set PLSCSI=1.0.0     // @ LG       DVD-ROM DRN8080B 1.11
**/

static void sclGuessPrintln(ScsiCommandLine * scl, FILE * fi)
    {
#if (_WIN32 || __MSDOS__)
    (void) fprintf(fi, "set PLSCSI=%s", scl->theName);
#else
    (void) fprintf(fi, "export PLSCSI='%s'", scl->theName);
#endif

    int length = (int) strlen(scl->theName);
    while ((length % 8) != 7) { (void) fprintf(fi, " "); ++length; }
    (void) fprintf(fi, " // ");

    INT printLength = scsiGetLength(scl->theScsi, scl->theDataLength);
    sclInquiryPrintln(fi, scl->theInChars, printLength);
    }

/**
**  Print a one-line summary of op x03 RequestSense data.  Print
**  nothing if less than xE bytes were available and copied In.
**
**  Example output lines include the following, from the x up to the //:
**
**      x 0                             // No Sense Available
**      x 2 3A                          // No Media
**      x 6 28                          // Transition to Ready
**      x 6 29                          // Transition from Reset
**      x 2 04 03                       // Manual Intervention Required
**      x 3 11 76543210                 // Read of Lba x76543210 Failed Ecc Check
**      x 2 04 04 ........ 00C000       // Format 75% Complete
**
**  Tbd: Think harder about whether senseChars[x 01 08..0B 12..FF]
**  are ever meaningful.
**/

void sclSensePrint(FILE * fi, char const * senseChars, int senseLength)
    {

    /* If the additionalLength was copied In ... */

    if (fi != NULL)
    if (senseChars != NULL)
    if (0x07 < senseLength)
        {

        /* Fetch fields. */

        int valid = (senseChars[0] & 0x80);
        int responseCode = (senseChars[0] & 0x7F);
        int sk = (senseChars[2] & 0x0F);

        int information = (senseChars[3] & 0xFF);
        information = ((information << 8) | (senseChars[4] & 0xFF));
        information = ((information << 8) | (senseChars[5] & 0xFF));
        information = ((information << 8) | (senseChars[6] & 0xFF));

        /* Count sense bytes. */

        int additionalLength = (senseChars[7] & 0xFF);
        int availableLength = (7 + (sizeof senseChars[7]) + additionalLength);
        int supposedLength = availableLength;
        if ((additionalLength == 0) || (0xFF < availableLength)) supposedLength = 0xE;

        int visibleLength = ((senseLength < supposedLength) ? senseLength : supposedLength);
        if ((responseCode != 0x70) && (responseCode != 0x71))
            {
            visibleLength = 1;
            }

        /* If the ASCQ was copied In ... */

        if ((0xD < visibleLength) && (visibleLength <= 0xFF))
            {
            int asc = (senseChars[0xC] & 0xFF);
            int ascq = (senseChars[0xD] & 0xFF);

            /* Distinguish deferred errors from current. */

            if (responseCode != 0x70) /* if not current */
                {
                (void) fprintf(fi, "deferred ");
                }

            /* Print SK ASC ASCQ in hex, but strip trailing x00. */

            (void) fprintf(fi, "x %lX", (long) sk);
            if ((asc != 0) || (ascq != 0))
                {
                (void) fprintf(fi, " %02lX", (long) asc);
                if (ascq != 0)
                    {
                    (void) fprintf(fi, " %02lX", (long) ascq);
                    }
                }

            /* Print "INFORMATION" if "VALID". */

            if (valid != 0)
                {
                (void) fprintf(fi, " %08lX", (long) information);
                }

            /* Print SKSV i.e. "SENSE-KEY SPECIFIC" valid. */

            if (0x11 < visibleLength)
                {
                int sksv = (senseChars[0xF] & 0x80);
                int sks = (senseChars[0xF] & 0x7F);
                sks = ((sks << 8) | (senseChars[0x10] & 0xFF));
                sks = ((sks << 8) | (senseChars[0x11] & 0xFF));
                if (sksv != 0)
                    {
                    if (valid == 0)
                        {
                        (void) fprintf(fi, " ........");
                        }
                    (void) fprintf(fi, " %06lX", (long) sks);
                    }
                }
            }
        }
    }

/**
**  Print data bytes as both hex and Ascii.
**
**  Think repeated <stdio.h> printf %02X and %c.
**
**  For example:
**
**      x 00000000 00:01:02:03 04:05:06:07 08:09:0A:0B 0C:0D:0E:0F "@ABCDEFGHIJKLMNO"
**      x 00000010 10:11 .. .. .. .. .. .. .. .. .. .. .. .. .. .. "PQ"
**
**  To fit on to legacy displays, print 79 chars or less per line.
**
**  To make visible the offset of each byte within the x10 byte line,
**  print the bytes in delimited quads.
**
**  To make the offsets of each field of a structure appear the
**  same over time, print the offset of each line, rather than its
**  address.  Regret that this design choice makes less obvious the
**  misalignment of a struct.
**
**  To make the value of each byte more visible, print x2E '.' for
**  less than every byte outside of the x20..7E printable Ascii subset.
**
**  To print more bytes in less lines, print an ellipsis "..." in place
**  of the middle of a run of identical lines.
**/

void sclHexPrintln(FILE * fi, char const * dataChars, INT dataLength)
    {
    if ((fi != NULL) && (dataChars != NULL) && (0 <= dataLength))
        {
        char const * tailChars = dataChars;
        INT tailLength = dataLength;

        /* Repeatedly ... */

        INT baseAddress = TO_INT(tailChars);
        char const * wereTailChars = NULL;
        for (;;)
            {

            /* Count duplicate lines. */

            if (wereTailChars != NULL)
                {
                char const * sameChars = tailChars;
                INT sameLength = 0x10;
                while (sameLength <= tailLength)
                    {
                    // DJGPP chokes if we don't cast to (void*) 
                    if (MEMCMP((void*)wereTailChars, (void*)sameChars, 0x10) != 0) break;
                    sameChars = (const char *) TO_VOID_P(TO_INT(sameChars) + 0x10);
                    sameLength += 0x10;
                    }
                sameLength -= 0x10;

                /* Print an ellipsis in place of all but the last duplicate. */

                if (0x20 <= sameLength)
                    {
                    sameLength -= 0x10;
//                  (void) fprintf(fi, "... x%lX more of the same bytes ...\n", (long) sameLength);
                    (void) fprintf(fi, "...\n");
                    tailChars = (const char *) TO_VOID_P(TO_INT(tailChars) + sameLength);
                    tailLength -= sameLength;
                    }
                }

            wereTailChars = tailChars;

            /* Count bytes to print in this line. */

            int lineLength = 0x10; /* bytes that exist */
            int padLength = 0; /* bytes that do not */
            if (tailLength < lineLength)
                {
                padLength = lineLength - ((int) tailLength);
                lineLength = ((int) tailLength);
                }

            /* Break before printing an empty line. */

            if (lineLength == 0)
                {
                break;
                }

            /* Print hex address or offset. */

            INT printAddress = (TO_INT(tailChars) - baseAddress);
            (void) fprintf(fi, "x %08lX", (long) printAddress);

            /* Print (and copy) bytes that exist. */

/*          (void) fprintf(fi, " "); */
            int index;
            for (index = 0; index < lineLength; ++index)
                {
                char ch = tailChars[index];
                int xx = (ch & 0xFF);
                if ((index % 4) == 0)
                    {
                    (void) fprintf(fi, " %02X", xx);
                    }
                else
                    {
                    (void) fprintf(fi, ":%02X", xx);
                    }
                }

            /* Print bytes that do not exist. */

            for (index = 0; index < padLength; ++index)
                {
                (void) fprintf(fi, " ..");
                }
/*          (void) fprintf(fi, " "); */

            /* Again print the bytes that exist, but as Ascii. */

            (void) fprintf(fi, " \"");
            sclDataPrint(fi, &tailChars[0], lineLength);
            (void) fprintf(fi, "\"\n");

            /* Account for the bytes printed. */

            tailChars = (const char *) TO_VOID_P(TO_INT(tailChars) + lineLength);
            tailLength -= lineLength;
            }
        }
    }

/**
**  Print a Cdb in hex.
**
**  Print any non-standard length as unstructured data.  But print the
**  standard lengths in the standard ways:
**
**      XX XX:XX:XX XX XX // 6
**      XX XX XX:XX:XX:XX XX XX:XX XX // 10
**      XX XX XX:XX:XX:XX XX XX:XX:XX:XX XX // 12
**/

void sclCdbPrintln(FILE * fi, char const * chars, int length)
    {

    /* Print any non-standard length as unstructured data. */

    switch (length)
        {
        case 6:
        case 10:
        case 12:
            break;
        default:
            sclHexPrintln(fi, chars, length);
            return;
        }

    /* Print the head. */

    (void) fprintf(fi, "x 00000000");

    /* Print the body. */

    int index = 0;
    (void) fprintf(fi, " %02X", (chars[index++] & 0xFF));
    if (length != 6)
        {
        (void) fprintf(fi, " %02X", (chars[index++] & 0xFF));
        }
    (void) fprintf(fi, " %02X", (chars[index++] & 0xFF));
    if (length != 6)
        {
        (void) fprintf(fi, ":%02X", (chars[index++] & 0xFF));
        }
    (void) fprintf(fi, ":%02X", (chars[index++] & 0xFF));
    (void) fprintf(fi, ":%02X", (chars[index++] & 0xFF));
    (void) fprintf(fi, " %02X", (chars[index++] & 0xFF));
    if (length != 6)
        {
        (void) fprintf(fi, " %02X", (chars[index++] & 0xFF));
        if (length != 10)
            {
            (void) fprintf(fi, ":%02X", (chars[index++] & 0xFF));
            (void) fprintf(fi, ":%02X", (chars[index++] & 0xFF));
            }
        (void) fprintf(fi, ":%02X", (chars[index++] & 0xFF));
        }
    (void) fprintf(fi, " %02X", (chars[index++] & 0xFF));

    for (; index < 0x10; ++index)
        {
        (void) fprintf(fi, " ..");
        }

    /* Print the tail. */

    (void) fprintf(fi, " \"");
    sclDataPrint(fi, &chars[0], length);
    (void) fprintf(fi, "\"\n");
    }

/**
**  Print before speaking Scsi.
**/

static void sclBeforePrintln(ScsiCommandLine * scl, FILE * fi)
    {

    /* Print the Cdb. */

    if (scl->theVerboseBit)
        {
        int cdbLength = scl->theCdbLength;
        if (0 <= cdbLength)
            {
            sclCdbPrintln(fi, scl->theCdbChars, cdbLength);
            }
        }

    /* Print the data copied Out. */

    if (scl->theVerboseBit)
        {
        INT inLength = scl->theInLength;
        INT outLength = scl->theOutLength;
        INT dataLength = scl->theDataLength;
        if ((0 < outLength) || ((0 < inLength) && (inLength < dataLength)))
            {
            sclHexPrintln(fi, scl->theInChars, dataLength);
            }
        }
    }

/**
**  Print a one-line summary of op x03 RequestSense data.
**/

static int sclSensePrintln(ScsiCommandLine * scl, FILE * fi)
    {
    char senseChars[0x100];
    (void) memset(&senseChars[0], '\0', sizeof senseChars);
    Scsi * scsi = scl->theScsi;
    int senseLength = scsiGetSense(scsi, &senseChars[0], sizeof senseChars, 0);
    if (0 < senseLength)
        {
        if (scl->theVerboseBit)
            {
            sclHexPrintln(fi, &senseChars[0], senseLength);
            }
        (void) fprintf(fi, "// ");
        sclSensePrint(fi, &senseChars[0], senseLength);
        (void) fprintf(fi, " sense");

        INT dataLength = scsiGetLength(scsi, -1);
        if (0 <= dataLength)
            {
            INT residue = (scl->theDataLength - dataLength);
            if (residue != 0)
                {
                (void) fprintf(fi, " // ");
                if ((-9 <= residue) && (residue <= 9))
                    {
                    (void) fprintf(fi, "%ld", (long) residue);
                    }
                else
                    {
                    (void) fprintf(fi, "x%lX (%ld)", (long) residue, (long) residue);
                    }
                (void) fprintf(fi, " residue");
                }
            }

        (void) fprintf(fi, "\n");
        return 0;
        }
    return -1;
    }

/**
**  Print after speaking Scsi.
**
**  Print verbose data copied In no matter if auto-sensed or not,
**  because sclBeforePrintln will have copied verbose data Out no
**  matter if auto-sensed or not.
**
**  Print sense data copied In after other data copied In to help
**  people focus on the sense data, since the data copied In with sense
**  data is commonly, though not reliably, meaningless.
**/

static void sclAfterPrintln(ScsiCommandLine * scl, FILE * fi)
    {
    if (scl->theVerboseBit)
        {
        INT inLength = scl->theInLength;
        INT outLength = scl->theOutLength;
        INT dataLength = scl->theDataLength;
        if ((0 < inLength) || ((0 < outLength) && (outLength < dataLength)))
            {
            sclHexPrintln(fi, scl->theInChars, dataLength);
            }
        }
    (void) sclSensePrintln(scl, stderr);
    }

/**
**  Allot zeroed bytes with a specified alignment.
**
**  Return the aligned address within the memory allotted, else null.
**
**  Run slowly enough to let alignLength be other than a power of two.
**
**  Trust noone passes the result returned to stdlib.free.
**/

static void * newAligned(UINT length, UINT alignLength)
    {
    UINT allocLength = (length + alignLength);
    void * vo = NULL;
    UINT allocOffset = 0;
    UINT allocWaste = 0;
    if (length <= allocLength)
        {
        vo = CALLOC(1, allocLength);
        if (vo != NULL)
            {
            if (0 < alignLength)
                {
                allocOffset = (TO_INT(vo) % alignLength);
                allocWaste = ((alignLength - allocOffset) % alignLength);
                vo = TO_VOID_P(TO_INT(vo) + allocWaste);
                }
            return vo; /* succeed */
            }
        }
    return NULL; /* fail */
    }

/**
**  Leak some carefully mis/aligned memory.
**
**  Return the mis/aligned address within the memory allotted, else null.
**
**  Trust noone passes the result returned to stdlib.free.
**/

static void * newAlignedNot(UINT length, UINT alignLength, UINT alignOffset)
    {
    UINT allocLength = (length + alignOffset);
    void * vo = NULL;
    if (length <= allocLength)
        {
        vo = newAligned(allocLength, alignLength);
        vo = TO_VOID_P(TO_INT(vo) + alignOffset);
        return vo; /* succeed */
        }
    return NULL; /* fail */
    }

/**
**  Leak some chars
**/

static char * newChars(char const * chars, int length)
    {
    char * allottedChars = (char *) calloc(1, length);
    if (allottedChars != NULL)
        {
        (void) memmove(allottedChars, chars, length);
        }
    return allottedChars;
    }

/**
**  Guess what's meant by the missing arguments.
**
**  Return zero if ok, else don't.
**/

void sclGuess(ScsiCommandLine * scl)
    {

    /* Guess the Cdb. */

    if (scl->theCdbLength < 0)
        {
        char cdbChars[] = SCL_CDB;
        int cdbLength = ((sizeof cdbChars) - 1);
        (void) cdbChars;
        scl->theCdbChars = SCL_CDB;
        scl->theCdbLength = cdbLength;
        if (scl->theInLength < 0) scl->theInLength = SCL_IN_LENGTH;
        if (scl->theOutLength < 0) scl->theOutLength = SCL_OUT_LENGTH;
        scl->theCdbGuessedBit = 1;
        }

    /* Guess in & out lengths. */

    if (scl->theInLength < 0) scl->theInLength = 0;

    if (scl->theOutLength < 0) scl->theOutLength = 0;

    /* Guess the data length. */

    if (scl->theDataLength < 0)
        {
        scl->theDataLength = (scl->theInLength + scl->theOutLength);
        }
    }

/**
**  Ping the device.  That is, try the default command once.
**
**  Often the default command is -x 12 0 0 0 24 0 -i x24
**  i.e. an op x12 Inquiry for up to x24 bytes.
**/

INT sclPing(ScsiCommandLine * scl)
    {
    Scsi * scsi = scl->theScsi;
    if (scsi == NULL) return -1;

    char cdbChars[] = SCL_CDB;
    int cdbLength = ((sizeof cdbChars) - 1);
    int inLength = SCL_IN_LENGTH;
    int outLength = SCL_OUT_LENGTH;
    int maxLength = (inLength + outLength);
    int direction = X0_DATA_NOT;
    if (0 < inLength) direction = X1_DATA_IN;
    if (0 < outLength) direction = X2_DATA_OUT;

    long longs[1 + ((SCL_IN_LENGTH + SCL_OUT_LENGTH) / sizeof (long))];
    char * dataChars = (char *) &longs[0];
    (void) memset(dataChars, '\0', maxLength);

    INT sayInt = scsiSay(scsi, cdbChars, cdbLength, dataChars, maxLength, direction);
    return sayInt;
    }

/**
**  Construct mis/aligned data in/out buffers.
**
**  Return zero if ok, else don't.
**/

int sclAllotData(ScsiCommandLine * scl)
    {

    /* Choke if theDataLength is negative. */

    INT dataLength = scl->theDataLength;
    if (dataLength < 0) return -1;

    /* Choose alignment. */

    INT alignOffset = scl->theAlignOffset;
    if (alignOffset < 0) scl->theAlignOffset = alignOffset = 0;

    INT alignLength = scl->theAlignLength;
    if (alignLength < 0) scl->theAlignLength = alignLength = (4 * Ki);

    /* Decide from where to copy data Out. */

    char * outChars = (char *) newAlignedNot(dataLength, alignLength, alignOffset);
    if (outChars == NULL) return -1;
    scl->theOutChars = outChars;

    /* Decide to where to copy data In. */
    /* Copy in over top of the outChars, if need be. */

    char * inChars = (char *) newAlignedNot(dataLength, alignLength, alignOffset);
    if (inChars == NULL) inChars = outChars;
    scl->theInChars = inChars;

    /* Succeed. */

    return 0;
    }

/**
**  Read, in from a file, the data to be copied Out.
**
**  Return zero if ok, else don't.
**/

int sclFrom(ScsiCommandLine * scl)
    {

    /* Always decide all the data (often was all '\0'). */

    char * outChars = scl->theOutChars;
    INT dataLength = scl->theDataLength;
    (void) MEMSET(outChars, '\xAE', dataLength); /* xAE = ('.' | x80) */

    /* Maybe read some of it from a file. */

    char const * fromFileName = scl->theFromFileName;
    if (fromFileName != NULL)
        {
        FILE * fi = stdin;
        if (fromFileName[0] != '\0') fi = fopen(fromFileName, "rb");
        if (fi == NULL)
            {
            (void) fprintf(stderr, "scl.sclFrom.fopen: %s\n", fromFileName);
            return -1;
            }

#if (_WIN32 || __MSDOS__)
        setmode(fileno(fi), O_BINARY);
#endif

        INT ioLength = (INT) FREAD(outChars, 1, dataLength, fi);
        if (ioLength < 0) return -1;

        /* Copy the file repeatedly til full. */

        char * tailChars = outChars;
        INT tailLength = dataLength;
        INT copyLength = ioLength;
        for (;;)
            {
            tailChars = (char *) TO_VOID_P(TO_INT(tailChars) + copyLength);
            tailLength -= copyLength;
            if (tailLength <= 0) break;

            copyLength = ioLength;
            if (tailLength < copyLength) copyLength = tailLength;
            (void) MEMMOVE(tailChars, outChars, copyLength);
            ioLength += copyLength;
            }

        if (fromFileName[0] != '\0') (void) fclose(fi);
        }

    /* Succeed. */

    return 0;
    }

/**
**  Write, out to a file, the data copied In.
**
**  Return zero if ok, else don't.
**/

int sclTo(ScsiCommandLine * scl)
    {
    char const * toFileName = scl->theToFileName;
    if (toFileName != NULL)
        {
        FILE * fi = stdout;
        if (toFileName[0] != '\0') fi = fopen(toFileName, "wb");
        if (fi == NULL)
            {
            (void) fprintf(stderr, "scl.sclTo.fopen: %s\n", toFileName);
            return -1;
            }

#if (_WIN32 || __MSDOS__)
        setmode(fileno(fi), O_BINARY);
#endif

        INT ioLength = (INT) FWRITE(scl->theInChars, 1, scl->theDataLength, fi);
        if (ioLength < 0) return -1;

        if (toFileName[0] != '\0') (void) fclose(fi);
        }
    return 0;
    }

/**
**  Speak a sentence of Scsi through a Scsi connection.
**
**  Return zero if all was ok, positive to say specifically how many
**  less than all data bytes were copied, else negative.
**
**  Also return zero if all was ok except that the number of data bytes
**  copied is unknown.
**/

INT sclCompare(ScsiCommandLine * scl, INT exitInt)
    {

    /* Compare. */

    Scsi * scsi = scl->theScsi;
    INT compareLength = scsiGetLength(scsi, -1);
    if (compareLength < 0)
        {
        if (exitInt == 0)
            {
            compareLength = scl->theDataLength;
            }
        }

    /* Compare. */

    char * outChars = scl->theOutChars;
    char * inChars = scl->theInChars;
    int delta = MEMCMP(outChars, inChars, compareLength);

    /* Complain if always the same. */

    if (outChars == inChars)
        {
        (void) fprintf(stderr, "// plscsi.sclCompare: can't compare without the data out\n");
        delta = -1;
        }

    /* Complain if different this time. */

    if (delta != 0)
        {
        if (0 < exitInt) exitInt = -X0100_RESIDUE;
        exitInt = -((-exitInt) | X2000_DIFFERENCE);
        }

    /* Proceed. */

    return exitInt;
    }

/**
**  Speak a sentence of Scsi to a virtual loopback device.
**/

static INT sclSayLoop(ScsiCommandLine * scl)
    {
    int cdbLength = scl->theCdbLength;
    INT inLength = scl->theInLength;

    /* Copy In one echo of the Cdb. */

    if ((0 < cdbLength) && (0 < inLength))
        {
        char const * cdbChars = scl->theCdbChars;
        char * inChars = scl->theInChars;
        while (0 < inLength)
            {
            int copyLength = cdbLength;
            if (inLength < cdbLength) copyLength = (int) inLength;
            (void) memmove(inChars, cdbChars, copyLength);
            inChars += copyLength; inLength -= copyLength;

            /* Then continue the Cdb. */

            if (0 < inLength)
                {
                char cc = cdbChars[cdbLength - 1];
                int delta = 0;
                if (1 < cdbLength) delta = (cc - cdbChars[cdbLength - 2]);
                for (int index = 0; index < inLength; ++index)
                    {
                    cc = (char) (cc + delta);
                    inChars[index] = cc;
                    }
                inLength = 0;
                break;
                }
            }
        }

    /* Proceed. */

    INT outLength = scl->theOutLength;
    INT exitInt = (outLength + inLength);
    return exitInt;
    }

/**
**  Determine the length of a Scsi Cdb, given cb0.
**/

static int toLengthFromCdb0(int cdb0, int elseLength)
    {
    switch (cdb0 & 0xE0) /* "group" */
        {
        case 0x00: /* x00 .. x1F */
            return 6;
        case 0xA0: /* xA0 .. xBF */
            return 12;
        case 0x20: /* x20 .. x3F */
        case 0x40: /* x40 .. x5F */
            return 10;
        case 0x60: /* x60 .. x7F */ /* SCSI2 reserved */
        case 0x80: /* x80 .. x9F */ /* SCSI2 reserved */
        case 0xC0: /* xC0 .. xDF */ /* SCSI2 vendor-specific */
        case 0xE0: /* xE0 .. xFF */ /* SCSI2 vendor-specific */
        default:
            return elseLength;
        }
    }

/**
**  Sometimes require people to say --please or -p.
**
**  Decide to require --please or not without regard to the device or
**  to the platform, in order to try and keep the command line syntax
**  the same across devices and across platforms.
**/

int sclLimitSay(ScsiCommandLine * scl)
    {

    /* Accept an explicit --please. */

    if (scl->thePleaseBit) return 0;

    /* Accept --loop in place of --please. */

    if (scl->theLoopBit) return 0;

    /* Reject cdbLength not Scsi, to avoid Ho != Do for CommandOut. */

    char const * cdbChars = scl->theCdbChars;
    int cdbLength = scl->theCdbLength;
    if ((cdbLength <= 0) || (toLengthFromCdb0(cdbChars[0] & 0xFF, -1) != cdbLength))
        {
        (void) fprintf(stderr, "// say --please (or -p) to say -x ... of a not standard length\n");
        return -1;
        }

    /* Reject op x04 Format because recovery is slow, maybe impossible. */

    if (0 < cdbLength)
        {
        int cdb0 = (cdbChars[0] & 0xFF);
        if (cdb0 == 0x04) /* x04 = Format */
            {
            (void) fprintf(stderr, "// say --please (or -p) to say x04 Format\n");
            return -1;
            }
        }

    /* Reject op x1A ModeSense6 for 1 to 7 bytes. */
    /* Win2K kernel crashes if Di in 1..7 for op x1A translated to op x5A. */

    if (4 < cdbLength)
        {
        int cdb0 = (cdbChars[0] & 0xFF);
        int cdb4 = (cdbChars[4] & 0xFF);
        if (cdb0 == 0x1A) /* x1A = ModeSense6 */
        if ((0x01 <= cdb4) && (cdb4 <= 0x07))
            {
            (void) fprintf(stderr, "// say --please (or -p) to say x1A ModeSense6 for 1 to 7 bytes\n");
            return -1;
            }
        }

    /* Accept anything else. */

    return 0;
    }

/**
**  Speak a sentence of Scsi through a Scsi connection.
**
**  Return zero if all was ok, positive to say specifically how many
**  less than all data bytes were copied, else negative.
**
**  Also return zero if all was ok except that the number of data bytes
**  copied is unknown.
**/

INT sclSay(ScsiCommandLine * scl)
    {

    /* Fetch more args. */

    Scsi * scsi = scl->theScsi;

    /* Decide the data to be copied out. */

    INT exitInt = sclFrom(scl);
    if (exitInt == 0)
        {
        char * inChars = scl->theInChars;
        char const * outChars = scl->theOutChars;
        INT dataLength = scl->theDataLength;
        // DJGPP chokes if we don't cast to (void*)
        (void) MEMMOVE(inChars, (void*)outChars, dataLength);

        /* Say something. */

        char const * cdbChars = scl->theCdbChars;
        int cdbLength = scl->theCdbLength;
        INT inLength = scl->theInLength;
        INT outLength = scl->theOutLength;

        if (exitInt == 0)
            {
            exitInt = -1;
            if (0 <= cdbLength)
            if ((outLength == 0) || (inLength == 0))
                {
                sclBeforePrintln(scl, stderr);
                if (scl->theLoopBit)
                    {
                    exitInt = sclSayLoop(scl);
                    }
                else if (0 < outLength)
                    {
                    exitInt = scsiSay(scsi, cdbChars, cdbLength, inChars, outLength, X2_DATA_OUT);
                    }
                else if (0 < inLength)
                    {
                    exitInt = scsiSay(scsi, cdbChars, cdbLength, inChars, inLength, X1_DATA_IN);
                    }
                else
                    {
                    exitInt = scsiSay(scsi, cdbChars, cdbLength, inChars, 0, X0_DATA_NOT);
                    }
                sclAfterPrintln(scl, stderr);
                }
            }

        /* Compare the data copied Out with the data copied In. */

        if (scl->theCompareBit)
            {
            exitInt = sclCompare(scl, exitInt);
            }

        /* Print some of the data copied In if the Cdb was guessed. */

        if (exitInt == 0)
        if (scl->theCdbGuessedBit)
            {
            sclGuessPrintln(scl, stdout);
            }

        /* Capture the data copied In. */

        int toExitInt = sclTo(scl);
        if (exitInt == 0) exitInt = toExitInt;
        }

    /* Proceed. */

    return exitInt;
    }

/**
**  Open a Scsi connection and limit sense and time per command.
**
**  Return zero to succeed, else don't.
**/

static int sclOpenLimited(ScsiCommandLine * scl, char const * name)
    {
    Scsi * scsi = scl->theScsi;

    /* Open. */

    int exitInt = scsiOpen(scsi, name);
    if (exitInt == 0)
        {

        /* Limit sense, but see failure only if asked to limit sense. */

        if (exitInt == 0)
            {
            int maxSenseLength = scl->theMaxSenseLength;
            if (maxSenseLength < 0)
                {
                maxSenseLength = SCL_SENSE_LENGTH;
                (void) scsiLimitSense(scsi, maxSenseLength);
                }
            else
                {
                exitInt = scsiLimitSense(scsi, maxSenseLength);
                }
            }

        /* Limit time, but see failure only if asked to limit time. */

        if (exitInt == 0)
            {
            INT minSeconds = scl->theMinSeconds;
            INT minNanoSeconds = scl->theMinNanoSeconds;
            if ((minSeconds < 0) || (minNanoSeconds < 0))
                {
                minSeconds = SCL_SECONDS; minNanoSeconds = 0;
                (void) scsiLimitSeconds(scsi, minSeconds, minNanoSeconds);
                }
            else
                {
                exitInt = scsiLimitSeconds(scsi, minSeconds, minNanoSeconds);
                }
            }
        }
    return exitInt;
    }

/**
**  Open a null FILE once.
**/

static FILE * sclGetNullFile(ScsiCommandLine * scl)
    {
    if (scl == NULL) return NULL;
    FILE * nullFile = scl->theNullFile;
    if (nullFile == NULL)
        {
#if (_WIN32 || __MSDOS__)
        scl->theNullFile = nullFile = fopen("NUL", "w");
#else
        scl->theNullFile = nullFile = fopen("/dev/null", "w");
#endif
        }
    return nullFile;
    }

/**
**  Read the first else the next well-known device path name
**  which responds ok to -x 12 0 0 0 24 0 -i x24 i.e. an op x12 Inquiry
**  for up to x24 bytes.
**
**  While searching, temporarily discard err output.
**
**  Copy a name of up to charsLength to the chars.
**
**  Return the positive length of chars assigned, else zero to say the
**  name was too long (or if chars is null), else negative if no more
**  names exist (or if scsi is null).
**
**  Leave the scsi open if a device was found.
**/

static int sclOpenPingable(ScsiCommandLine * scl, char * chars, int charsLength)
    {
    Scsi * scsi = scl->theScsi;
    if (scsi == NULL) return -1;
    if (chars == NULL) return 0;

    /* Open a null FILE once. */

    FILE * nullFile = sclGetNullFile(scl);

    /* Temporarily discard err output. */

    FILE * fi = stderr;
    scsiSetErr(scsi, nullFile);

    /* For each possible name ... */

    int exitInt = -1;
    for (;;)
        {
        int readInt = scsiReadName(scsi, chars, charsLength);
        exitInt = readInt;
        if (readInt <= 0) break;

        /* Try the name. */

        int openInt = sclOpenLimited(scl, &chars[0]);
        if (openInt == 0)
            {
            INT sayInt = sclPing(scl);
            if (sayInt == 0) break;
            }
        }

    /* Close past the end. */

    if (exitInt < 0)
        {
        scsiClose(scsi);
        }

    /* Restore stderr. */

    scsiSetErr(scsi, fi);

    /* Proceed. */

    return exitInt;
    }

/**
**  Open, in turn, each of the names from the command line.
**
**  Also, in turn, open each well-known device name, if --whichever.
**
**  Return 0 if ok, else don't.
**/

int sclOpenNext(ScsiCommandLine * scl)
    {

    /* Maybe open a loop back at most once. */

    if (scl->theLoopBit)
        {
        if (scl->theName == NULL)
            {
            scl->theName = "--loop";
            return 0;
            }
        return -1;
        }

    /* Open each of theNames in turn. */

    while (0 < scl->theNamesLength)
        {
        char const * name = *scl->theNames;
        ++scl->theNames; --scl->theNamesLength;
        scl->theName = name;
        int exitInt = sclOpenLimited(scl, name);
        if (exitInt == 0) return 0;
        (void) fprintf(stderr, "// plscsi.sclOpenNext: \"%s\" not opened\n", name);
        }

    /* Maybe open each well-known device name in turn. */

    if (!scl->theWhicheverBit) return -1;

    char chars[123];
    (void) memset(&chars[0], '\0', sizeof chars);
    int openInt = sclOpenPingable(scl, &chars[0], sizeof chars);
    if (openInt < 1) return -1;
    scl->theName = newChars(&chars[0], (int) strlen(&chars[0]) + 1);
    if (scl->theName == NULL) return -1;
    return 0;
    }

/**
**  Close.
**/

void sclClose(ScsiCommandLine * scl)
    {
    scsiClose(scl->theScsi);
    }

/**
**  Tbd:
**
**  Think harder about whether senseChars[x 01 08..0B 12..FF]
**  are ever meaningful.
**
**  Think harder about the tracing of negative residue.
**/

/* end of file */
