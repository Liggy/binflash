/**
**  plscsi.h
**  Saturday. September 21, 2002
**
**  Help link together the *.c* of ../plscsi/.
**
**  See http://members.aol.com/plscsi/
**/

/**
**  Compile at most once.
**/

#ifndef PLSCSI_H
#define PLSCSI_H PLSCSI_H

/**
**  Link as C, not C++.
**/

#ifdef __cplusplus
extern "C" {
#endif

/**
**  Link with standard C libraries.
**/

#include <stdio.h> /* FILE ... */

/**
**  Name what changes for DOS because int's there hold 16 bits, not 32.
**/

#ifdef __MSDOS__

typedef unsigned char EXIT_INT;
typedef long INT;
typedef unsigned long UINT;

#define INT_M1 (~ (INT) 0)
#define INT_MOST_NEG (1L << 31)
#define INT_MOST_POS (~INT_MOST_NEG)

#define CALLOC farcalloc

#define FREAD wideFRead
#define FWRITE wideFWrite
#define MEMCMP wideMemCmp
#define MEMMOVE wideMemMove
#define MEMSET wideMemSet

#define TO_VOID_P wideToVoidP
#define TO_INT wideToInt

#else

typedef int EXIT_INT;
typedef int INT;
typedef unsigned int UINT;

#define INT_M1 (-1)
#define INT_MOST_NEG INT_MIN
#define INT_MOST_POS INT_MAX

#define CALLOC calloc

#define FREAD fread
#define FWRITE fwrite
#define MEMCMP memcmp
#define MEMMOVE memmove
#define MEMSET memset

#define TO_VOID_P(II) ((void *) (II))
#if defined(__amd64__) || defined(amd64)
#define TO_INT(VO) ((INT)(long) (VO))
#else
#define TO_INT(VO) ((INT) (VO))
#endif

#endif

/**
**  List some different ways to copy data.
**/

#define X0_DATA_NOT     0x0
#define X1_DATA_IN      0x1 /* read = copy IN to host from device */
#define X2_DATA_OUT     0x2 /* write = copy OUT from host to device */

/**
**  Distinguish one negative return code from another, in accord with
**  an anonymous convention seen used in the Scsi storage industry.
**/

#define X2000_DIFFERENCE    0x2000 /* data copied In != data copied Out */
#define X0100_RESIDUE       0x0100 /* copied less than max data In or Out */
#define X0002_SENSE         0x0002 /* did copy In sense data */

/**
**  Choose which .cpp to link.
**/

#ifdef __APPLE__
#define STUC STUC
#endif

#ifndef _WIN32
#ifndef __MSDOS__
#ifndef __APPLE__
#define SGIO SGIO
#endif
#endif
#endif

#ifdef _WIN32
#define WINASPI WINASPI
#define SPTX SPTX
// #define SGIO SGIO
#endif

#ifdef __MSDOS__
#define DOSASPI DOSASPI
#endif

/**
**  Often treat DOSASPI and WINASPI the same.
**/

#ifdef DOSASPI
#ifndef XXXASPI
#define XXXASPI XXXASPI
#endif
#endif

#ifdef WINASPI
#ifndef XXXASPI
#define XXXASPI XXXASPI
#endif
#endif

/**
**  Name some struct's.
**/

typedef struct Aspi Aspi;
typedef struct CommandLine CommandLine;
typedef struct Scsi Scsi;
typedef struct ScsiCommandLine ScsiCommandLine;
typedef struct Sgio Sgio;
typedef struct Sptx Sptx;
typedef struct Stuc Stuc;

/**
**  Link with "dosaspi.cpp" or "winaspi.cpp".
**/

#ifdef XXXASPI
extern Aspi * newAspi(void);
extern void aspiSetErr(Aspi *, FILE *);
extern int aspiOpen(Aspi *, char const *);
extern int aspiLimitSense(Aspi *, int);
#ifdef WINASPI
extern int aspiLimitSeconds(Aspi *, INT, INT);
#endif
extern INT aspiSay(Aspi *, char const *, int, char *, INT, int);
extern INT aspiGetLength(Aspi *, INT);
extern int aspiGetSense(Aspi *, char *, int, int);
extern int aspiReadName(Aspi *, char *, int);
extern int aspiSwallowArg(Aspi *, char const *);
#endif

/**
**  Link with "cl.cpp".
**/

extern CommandLine * newCommandLine(void);
extern void clSet(CommandLine *, char const * const *, int);
extern void clSetFromString(CommandLine *, char const *);
extern char const * clPeek(CommandLine *);
extern char const * clRead(CommandLine *);
extern int clReadSwitch(CommandLine *);
extern int clReadComment(CommandLine *);
extern INT clReadInt(CommandLine *);
extern int clReadXX(CommandLine *);

/**
**  Link with "scl.cpp".
**/

extern ScsiCommandLine * newScsiCommandLine(void);

extern void sclSet(ScsiCommandLine *, char const * const *, int);
extern void sclSetFromString(ScsiCommandLine *, char const *);
extern int sclGetHelpBit(ScsiCommandLine *);
extern int sclGetVerboseBit(ScsiCommandLine *);
extern int sclGetYesBit(ScsiCommandLine *);
extern int sclGetNoBit(ScsiCommandLine *);
extern int sclParse(ScsiCommandLine *);

extern void sclDataPrint(FILE *, char const *, int);
extern void sclInquiryPrintln(FILE *, char const *, int);
extern void sclSensePrintln(FILE *, char const *, int);

extern void sclGuess(ScsiCommandLine *);
extern int sclAllotData(ScsiCommandLine *);
extern int sclFrom(ScsiCommandLine *);
extern int sclTo(ScsiCommandLine *);
extern int sclCompare(ScsiCommandLine *, int);
extern int sclLimitSay(ScsiCommandLine *);
extern INT sclSay(ScsiCommandLine *);
extern int sclOpenNext(ScsiCommandLine *);
extern void sclClose(ScsiCommandLine *);

/**
**  Link with "scsi.cpp".
**/

extern Scsi * newScsi(void);
extern void scsiSetErr(Scsi *, FILE *);
extern void scsiClose(Scsi *);
extern void scsiBinTrace(FILE*,const char*, int, bool=false, bool=false);
extern int scsiOpen(Scsi *, char const *, const char * = 0);
extern int scsiLimitSense(Scsi *, int);
extern int scsiLimitSeconds(Scsi *, INT, INT);
extern INT scsiSay(Scsi *, char const *, int, char *, INT, int);
extern INT scsiGetLength(Scsi *, INT);
extern int scsiGetSense(Scsi *, char *, int, int);
extern int scsiReadName(Scsi *, char *, int);
extern int scsiSwallowArg(Scsi *, char const *);

/**
**  Link with "stuc.cpp".
**/

#ifdef STUC
extern Stuc * newStuc(void);
extern void stucSetErr(Stuc *, FILE *);
extern void stucClose(Stuc *);
extern int stucOpen(Stuc *, char const *);
extern int stucLimitSense(Stuc *, int);
extern int stucLimitSeconds(Stuc *, INT, INT);
extern INT stucSay(Stuc *, char const *, int, char *, INT, int);
extern INT stucGetLength(Stuc *, INT);
extern int stucGetSense(Stuc *, char *, int, int);
extern int stucReadName(Stuc *, char *, int);
extern int stucSwallowArg(Stuc *, char const *);
#endif

/**
**  Link with "sgio.cpp".
**/

#ifdef SGIO
extern Sgio * newSgio(void);
extern void sgioSetErr(Sgio *, FILE *);
extern void sgioClose(Sgio *);
extern int sgioOpen(Sgio *, char const *);
extern int sgioLimitSense(Sgio *, int);
extern int sgioLimitSeconds(Sgio *, int, int);
extern int sgioSay(Sgio *, char const *, int, char *, int, int);
extern int sgioGetLength(Sgio *, int);
extern int sgioGetSense(Sgio *, char *, int, int);
extern int sgioReadName(Sgio *, char *, int);
extern int sgioSwallowArg(Sgio *, char const *);
#endif

/**
**  Link with "sptx.cpp".
**/

#ifdef SPTX
extern Sptx * newSptx(int);
extern void sptxSetErr(Sptx *, FILE *);
extern void sptxClose(Sptx *);
extern int sptxOpen(Sptx *, char const *);
extern int sptxLimitSense(Sptx *, int);
extern int sptxLimitSeconds(Sptx *, int, int);
extern int sptxSay(Sptx *, char const *, int, char *, int, int);
extern int sptxGetLength(Sptx *, int);
extern int sptxGetSense(Sptx *, char *, int, int);
extern int sptxReadName(Sptx *, char *, int);
extern int sptxSwallowArg(Sptx *, char const *);
#endif

/**
**  Link with "wideint.cpp".
**/

extern UINT wideFRead(void *, UINT, UINT, FILE *);
extern UINT wideFWrite(void const *, UINT, UINT, FILE *);
extern int wideMemCmp(void *, void *, UINT);
extern void * wideMemMove(void *, void *, UINT);
extern void * wideMemSet(void *, char, UINT);
extern INT wideToInt(const void *);
extern void * wideToVoidP(INT);

/**
**  Finish linking as C, not C++.
**/

#ifdef __cplusplus
}
#endif

/**
**  Finish compiling at most once.
**/

#endif /* PLSCSI_H */

/* end of file */
