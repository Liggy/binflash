/**
**  scsistub.cpp
**  Saturday. September 21, 2002
**
**  Connect the command-line to the Scsi-over-whatever devices named,
**  but hard-code the Scsi command to try, rather than parsing it from
**  the command line.
**/

/**
**  Decide what to say.
**/

#define CDB "\x12\0\0\0\x24\0"
#define IN_LENGTH 0x24
#define OUT_LENGTH 0

#define SENSE_LENGTH 0xE /* offsetof SK ASC ASCQ < xE */
#define SECONDS -1 /* negative means max */

/**
**  Link with standard C libraries.
**/

#include <stdio.h> /* FILE ... */
#include <stdlib.h> /* calloc ... */
#include <string.h> /* memmove ... */

/**
**  Maybe link with more libraries.
**/

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
**  Stand in place of "scsi.cpp".
**/

// #define SCSI_STUB SCSI_STUB

#ifdef SCSI_STUB

/**
**  Print a result.
**/

static void inquiryPrintln(char const * name, char const * dataChars, INT dataLength)
    {
    FILE * fi = stdout;
    (void) fprintf(fi, "%s\t// ", name);
    if (0x23 < dataLength)
        {
        int availableLength = (4 + (sizeof dataChars[4]) + (dataChars[4] & 0xFF));
        if (dataChars[4] == '\0') availableLength = 0x24;
        if (0x23 < availableLength)
            {
            char chars[0x1C];
            (void) memmove(&chars[0], &dataChars[8], 0x1C); chars[0x1C] = '\0';
            (void) fprintf(fi, "%c %s",
                ((dataChars[2] & 0x3F) | 0x40), /* version of Scsi */
                &chars[0] /* vendorIdentification:productIdentification:productRevisionLevel */
                );
            }
        }
    (void) fprintf(fi, "\n");
    }

/**
**  Run from the command line.
**/

int main(int argc, char const * const * argv)
    {
    --argc; ++argv;

    char cdbChars[] = CDB;
    int cdbLength = ((sizeof cdbChars) - 1);
    INT maxLength = (OUT_LENGTH + IN_LENGTH);
    int direction = ((0 < IN_LENGTH) ? X1_DATA_IN : X2_DATA_OUT);
    if (maxLength == 0) direction = X0_DATA_NOT;

    char * dataChars = (char *) CALLOC(1, maxLength);

    for (int argi = 0; argi < argc; ++argi)
        {
        char const * name = argv[argi];

        #ifdef SGIO
        Sgio * sgio = newSgio();
        if (sgioOpen(sgio, name) == 0)
            {
            (void) sgioLimitSense(sgio, SENSE_LENGTH);
            (void) sgioLimitSeconds(sgio, SECONDS, 0);
            int exitInt = sgioSay(sgio, &cdbChars[0], cdbLength, dataChars, maxLength, direction);
            if (exitInt == 0)
                {
                inquiryPrintln(name, dataChars, maxLength);
                }
            continue;
            }
        #endif

        #ifdef XXXASPI
        Aspi * aspi = newAspi();
        if (aspiOpen(aspi, name) == 0)
            {
            (void) aspiLimitSense(aspi, SENSE_LENGTH);
            #ifdef WINASPI
            (void) aspiLimitSeconds(aspi, SECONDS, 0);
            #endif
            INT exitInt = aspiSay(aspi, &cdbChars[0], cdbLength, dataChars, maxLength, direction);
            if (exitInt == 0)
                {
                inquiryPrintln(name, dataChars, maxLength);
                }
            continue;
            }
        #endif

        #ifdef SPTX
        int maxSptLength = -1; /* -1 = don't care */
        Sptx * sptx = newSptx(maxSptLength);
        if (sptxOpen(sptx, name) == 0)
            {
            (void) sptxLimitSense(sptx, SENSE_LENGTH);
            (void) sptxLimitSeconds(sptx, SECONDS, 0);
            int exitInt = sptxSay(sptx, &cdbChars[0], cdbLength, dataChars, maxLength, direction);
            if (exitInt == 0)
                {
                inquiryPrintln(name, dataChars, maxLength);
                }
            continue;
            }
        #endif

        }

    return 0;
    }

#endif /* SCSI_STUB */

/* end of file */
