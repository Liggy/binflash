/******************************************************************************\
| Revision history:                                                            |
|                                                                              |
| 0.90 - 20040828 - First public beta version                                  |
| 0.91 - 20040828 - Change verbose to int / add verbose output to -scan        |
|                 - Buffer size in GetDriveInfo from 0x60 to 0x80 Bytes (Bug)  |
| 0.92 - 20040828 - Add homepage to WelcomeScreen                              |
|      - 20040830 - Interface rewritten / new error handling                   |
| 0.93 - 20040910 - Implement device scan with callback routine                |
| 0.95 - 20040916 - Add bitsetting functions (no 0.94 for syncing)             |
| 1.00 - 20041013 - No change. Release version 1.00                            |
| 1.01 - 20041016 - No change. Release version 1.01                            |
| 1.02 - 20041019 - No change. Release version 1.02                            |
| 1.03 - 20041116 - No change. Release version 1.03                            |
| 1.10 - 20041225 - No change. Release version 1.10                            |
| 1.11 - 20050121 - No change. Release version 1.11                            |
| 1.12 - 20050310 - No change. Release version 1.12                            |
| 1.13 - 20050312 - No change. Release version 1.13                            |
| 1.14 - 20050327 - No change. Release version 1.14                            |
| 1.15 - 20050331 - No change. Release version 1.15                            |
| 1.16 - 20050412 - No change. Release version 1.16                            |
| 1.17 - 20050524 - No change. Release version 1.17                            |
| 1.18 - 20050802 - Add yesno to MessageCallback                               |
| 1.20 - 20051003 - Change drivetype from int to enum                          |
| 1.21 - 20051016 - No change. Release version 1.21                            |
| 1.21a- 20051022 - No change. Release version 1.21a                           |
| 1.22 - 20051029 - No change. Release version 1.22                            |
| 1.23 - 20051214 - No change. Release version 1.23                            |
| 1.24 - 20051214 - No change. Release version 1.24                            |
| 1.25 - 20060210 - Convert to Visual Studio 2005, remove LG stuff             |
| 1.26 - 20060218 - No change. Release version 1.26                            |
| 1.27 - 20060326 - No change. Release version 1.27                            |
| 1.28 - 20060422 - No change. Release version 1.28                            |
| 1.29 - 20060525 - No change. Release version 1.29                            |
| 1.30 - 20060625 - No change. Release version 1.30                            |
| 1.31 - 20060630 - Removed drive list. Was never called                       |
|        20060722 - Added okcancel to MessageCallback                          |
|        20060722 - Added getch function for Linux/Mac                         |
| 1.32 - 20060927 - No change. Release version 1.32                            |
| 1.33 - 20061004 - No change. Release version 1.33                            |
| 1.34 - 20061026 - Added identification routine                               |
| 1.35 - 20061126 - No change. Release version 1.35                            |
| 1.36 - 20061215 - No change. Release version 1.36                            |
| 1.37 - 20061231 - Remove references to Herrie's homepage                     |
| 1.38 - 20070310 - No change. Release version 1.38                            |
| 1.39 - 20070404 - No change. Release version 1.39                            |
| 1.40 - 20070813 - No change. Release version 1.40                            |
| 1.41 - 20070816 - No change. Release version 1.41                            |
| 1.42 - 20071016 - No change. Release version 1.42                            |
| 1.43 - 20071207 - No change. Release version 1.43                            |
| 1.44 - 20080212 - Mention -s option for -identify                            |
| 1.45 - 20080609 - No change. Release version 1.45                            |
| 1.46 - 20081215 - Add support for flashing files containing multiple         |
|                   firmwares                                                  |
| 1.46a- 20080609 - No change. Release version 1.46a                           |
| 1.46b- 20090216 - No change. Release version 1.46b                           |
| 1.47 - 20090425 - No change. Release version 1.47                            |
| 1.47a- 20090519 - Fix getchar calls on Linux/Mac and probably DOS            |
| 1.47b- 20090830 - No error output for E_CANCELLED                            |
| 1.48 - 20090907 - No change. Release version 1.48                            |
| 1.49 - 20091214 - No change. Release version 1.49                            |
| 1.49b- 20091218 - No change. Release version 1.49b                           |
| 1.50 - 20100119 - No change. Release version 1.50                            |
| 1.51 - 20100828 - No change. Release version 1.51                            |
| 1.52 - 20100908 - No change. Release version 1.52                            |
| 1.53 - 20101215 - No change. Release version 1.53                            |
| 1.54 - 20110123 - No change. Release version 1.54                            |
| 1.55 - 20110220 - No change. Release version 1.55                            |
| 1.57 - 20110615 - No change. Release version 1.57                            |
| 1.58 - 20111121 - No change. Release version 1.58                            |
| 1.59 - 20111206 - No change. Release version 1.59                            |
| 1.61 - 20120110 - No change. Release version 1.61                            |
| 1.62 - 20120204 - No change. Release version 1.62                            |
| 1.63 - 20130103 - No change. Release version 1.63                            |
| 1.64 - 20130519 - No change. Release version 1.64                            |
\******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "plscsi/plscsi.h"

#include "types.h"
#include "drive.h"

#include "necinterface.h"
#include "binflash.h"


// Emulate getch for Linux / Mac
#if (_WIN32 || __MSDOS__)
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>

int getch()
{
  struct termios oldt, newt;
  int ch;

  tcgetattr(STDIN_FILENO, &oldt);
  newt=oldt;
  newt.c_lflag&=~(ICANON|ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  ch=getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

  return ch;
}
#endif

int getcharflush()
{
   int retval,dummy;
   retval=getchar();
   while (((dummy=getchar())!=EOF) && (dummy!='\n'));

   return retval;
}


// Binflash version
static const char* BinVersion="1.64";


// Global drive handle
NECDRIVE drive(MessageCallback);

static char homepage[]="http://binflash.cdfreaks.com";

// Name of the executable
static char* ExecutableName;

// Static variable to check for debug option
#ifdef _DEBUG
bool debug=false;
#endif

// How much output do we want?
int verbosity=0;


/******************************************************************************\
| Function: WelcomeScreen                                                      |
|                                                                              |
| Purpose:                                                                     |
| Print welcome screen and copyright note                                      |
|                                                                              |
| Input:                                                                       |
| None                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| none                                                                         |
\******************************************************************************/
void WelcomeScreen()
{
   printf(
      "Binflash - %s version - (C) by Liggy and Herrie"
#ifdef _DEBUG
      " - DEBUG BUILD"
#endif
      "\n",drive.GetManufacturer());
#ifdef _DEBUG
   printf("Created: %s - %s\n",__DATE__,__TIME__);
#endif
    printf("Visit %s\n",homepage);
   printf("\n");
}


/******************************************************************************\
| Function: DisplayVersionInfo                                                 |
|                                                                              |
| Purpose:                                                                     |
| Print version information                                                    |
|                                                                              |
| Input:                                                                       |
| None                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| none                                                                         |
\******************************************************************************/
void DisplayVersionInfo()
{
   printf("Binflash version v%s\n",BinVersion);
   printf("Drive interface v%s\n",drive.GetDriveVersion());
   printf("%s interface v%s\n",drive.GetManufacturer(),drive.GetInterfaceVersion());
   printf("%s internal v%s\n",drive.GetManufacturer(),drive.GetInternalVersion());
}


/******************************************************************************\
| Function: DisplayHowto                                                       |
|                                                                              |
| Purpose:                                                                     |
| Display how this program must be called                                      |
|                                                                              |
| Input:                                                                       |
| None                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| none                                                                         |
\******************************************************************************/
void DisplayHowto()
{
#ifdef _DEBUG
   const char* DebugOption="[-d] ";
#else
   const char* DebugOption="";
#endif

   printf("Usage:\n");
   printf("%s {-scan} [-v] %s\n",ExecutableName,DebugOption);
   if (drive.CanDump())
   {
      printf("%s {-dump}",ExecutableName);
      if (drive.NeedDriveTypeDump())
         printf(" {-t Drivetype}");
      printf(" [-v] %s[-s] {filename} {device}\n",DebugOption);
   }

   if (drive.CanFlash())
   {
      printf("%s {-flash}",ExecutableName);
      if (drive.NeedDriveTypeFlash())
         printf(" {-t Drivetype}");
      printf(" [-v] %s[-s] {filename} {device}\n",DebugOption);
   }

   printf("%s {-identify} [-s] {device} {Filename}\n",ExecutableName);
   printf("%s {-querybt} {device}\n",ExecutableName);
   printf("%s {-setbt} {Mediatype} {Booktype} {Temp|Perm} {device}\n",ExecutableName);
   printf("%s {-setbtrw} {Booktype} {Device}\n",ExecutableName);
   printf("%s {-clearbt} {device}\n",ExecutableName);
   printf("%s {-queryrpc} {device}\n",ExecutableName);
   printf("%s {-resetrpc} {device} {Vendor|Global}\n",ExecutableName);
  printf("%s {-version}\n\n",ExecutableName);

   printf("-scan     - Scan for supported devices\n");
   if (drive.CanDump())
      printf("-dump     - Dump firmware and write to file\n");
   if (drive.CanFlash())
      printf("-flash    - Flash firmware and write to file\n");
   printf("-identify - Identify unknown drives\n");
   printf("-querybt  - Query booktype information\n");
   printf("-setbt    - Change booktype settings\n");
   printf("-setbtrw  - Change booktype on current DVD+RW disc\n");
   printf("-clearbt  - Clear booktype settings and counter\n");
   printf("-queryrpc - Query region status\n");
   printf("-resetrpc - Reset region counter on RPC2 drives\n");
   printf("-version  - Display version information\n\n");

   if (drive.NeedDriveTypeFlash() || drive.NeedDriveTypeDump())
      printf("-t {x} - Supply drive type\n");
   printf(
      "-v     - Verbose mode\n"
#ifdef _DEBUG
      "-d     - Debug mode. Dump complete memory / don't really flash\n"
#endif
      "-s     - Skip disclaimer\n\n"
      );
}


/******************************************************************************\
| Function: CheckDisclaimer                                                    |
|                                                                              |
| Purpose:                                                                     |
| Print disclaimer and ask for acception                                       |
|                                                                              |
| Input:                                                                       |
| None                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Disclaimer accepted                                                 |
| false -> Disclaimer refused                                                  |
\******************************************************************************/
bool CheckDisclaimer()
{
   char c;
   const char *manufacturer=drive.GetManufacturer();

   printf(
   "                       DISCLAIMER\n\n"
   "THIS PROGRAM IS PROVIDED \"AS IS\" WITHOUT WARRANTY OF ANY KIND,\n"
   "EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,\n"
   "THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A\n"
   "PARTICULAR PURPOSE.\n\n"
   "THE ENTIRE RISK AS TO THE ABILITY OF THIS PROGRAM TO FLASH\n"
   "%s OR COMPATIBLE DVDR DRIVES IS WITH YOU. SHOULD THE\n"
   "PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY\n"
   "SERVICING, REPAIR OR CORRECTION.\n\n"
   "THIS PROGRAM IS NOT ENDORSED BY %s OR ANY\n"
   "COMPANY RESELLING %s EQUIPMENT AS THEIR OWN BRAND\n\n"
   "IF YOU UNDERSTAND THE RISKS ASSOCIATED WITH THIS PROGRAM AND\n"
   "DISCHARGE BOTH THE AUTHOR AND %s FROM ANY\n"
   "DAMAGE OCCURING AS THE RESULT OF ITS USE, PLEASE INDICATE SO\n"
   "BY ANSWERING THE FOLLOWING QUESTION:\n\n"
   "Do you understand and agree to the statement above (y/n)? ",
   manufacturer,manufacturer,manufacturer,manufacturer);

   c = (char) getcharflush();

   printf("\n");

   if ((c!='y') && (c!='Y'))
      return false;

   return true;
}



/******************************************************************************\
| Function: MessageCallback                                                    |
|                                                                              |
| Purpose:                                                                     |
| Proceed messages from flashing and dumping routine                           |
|                                                                              |
| Input:                                                                       |
| MessageType (warning, fatal, error, info, ...)                               |
| format string (printf)                                                       |
| {optional arguments}                                                         |
|                                                                              |
| Returnvalues:                                                                |
| >=0 -> number of printed characters.                                         |
| <0  -> Error occured                                                         |
\******************************************************************************/
int MessageCallback(MessageType level,const char* format,...)
{
   int rc=0;
   va_list arg_ptr;

   va_start(arg_ptr,format);

#ifdef __SPECIAL__
   if (verbosity<verbose)
      rc=vprintf(format,arg_ptr);
#endif

   if (((level<=verbosity) && (level!=numsteps)) || ((level==step) && (verbosity>=verbose)))
      rc=vprintf(format,arg_ptr);
   else if (level==yesno)
   {
      vprintf(format,arg_ptr);
      putchar(' '); // looks nicer this way
      char c = (char) getcharflush();
      printf("\n");

      if ((c!='y') && (c!='Y'))
         rc=0;
      else
         rc=1;
   }
   else if (level==okcancel)
   {
      vprintf(format,arg_ptr);
      printf("\nEnter=OK, Escape=Cancel ");
      fflush(stdin);
      char c = (char) getch();
      fflush(stdin);
      printf("\n");

      if ((c!='\n') && (c!='\r'))
         rc=0;
      else
         rc=1;
   }

   va_end(arg_ptr);

   return rc;
}


/******************************************************************************\
| Function: ScanCallback                                                       |
|                                                                              |
| Purpose:                                                                     |
| Proceed drive when scanning for devices                                      |
|                                                                              |
| Input:                                                                       |
| DriveInformation                                                             |
|                                                                              |
| Returnvalues:                                                                |
| None                                                                         |
\******************************************************************************/
void ScanCallback(const char* device, const Inquiry* Info)
{
   printf("\n");
   printf("Device   : %s\n",device+((device[5]==':')?4:0));
   printf("Vendor   : %s\n",Info->vendor);
   printf("Model    : %s\n",Info->identification);
   printf("Firmware : %s\n",Info->revision);
#ifdef _DEBUG
   if (debug)
      printf("Specific : %s\n",Info->specific);
#endif
}


/******************************************************************************\
| Function: Error                                                              |
|                                                                              |
| Purpose:                                                                     |
| Send error text to stderr                                                    |
|                                                                              |
| Input:                                                                       |
| Error number                                                                 |
|                                                                              |
| Returnvalues:                                                                |
| Result from fprintf                                                          |
\******************************************************************************/
int Error(ErrorCode errnum)
{
   const char* errtext;

   switch (errnum)
   {
   case E_ILLEGAL_CALL:
      errtext="Illegal call parameter";break;
   case E_CANCELLED:
      errtext="Action canceled";break;
   case E_ACCESSFIRMFILE:
      errtext="Error opening firmware file";break;
   case E_NOMEM:
      errtext="Could not allocate enough memory";break;
   case E_SCSINEW:
      errtext="Error creating SCSI handle";break;
   case E_SCSIOPEN:
      errtext="Error opening SCSI device";break;
   case E_NODRIVETYPE:
      errtext="Drivetype could not be determined";break;
   case E_UNSUPPORTEDDRIVE:
      errtext="Drive is not supported";break;
   case E_DUMPNOSUPPORT:
      errtext="Dumping not supported with this flasher";break;
   case E_DUMPERROR:
      errtext="Error while dumping firmware";break;
   case E_FLASHNOSUPPORT:
      errtext="Flashing not supported with this flasher";break;
   case E_FLASHERROR:
      errtext="Error while flashing firmware";break;
   case E_FIRMNOMATCH:
      errtext="Firmware is for a different drive";break;
   case E_NOFIRMWARE:
      errtext="Supplied file does not contain a valid firmware";break;
   case E_BOOKNOTSUPPORTED:
      errtext="This booktype operation is not supported with your firmware";break;
   case E_BOOKTYPEMEDIA:
      errtext="Operation not supported on current media";break;
   case E_BOOKINVALIDNAME:
      errtext="Invalid name for mediatype or booktype";break;
   case E_IDENTIFYBOOTKEY:
      errtext="Could not find key";break;
   case E_IDENTIFYMEM:
      errtext="Could not read drive memory";break;
   case E_RPC1:
      errtext="Cannot reset RPC1 drives";break;
   case E_RPCNOVENDOR:
      errtext="No vendor resets left";break;
   case E_RPCNOTNEEDED:
      errtext="No reset required";break;
   default:
      return fprintf(stderr,"Received unknown error code %d",(int)errnum);
   }

   return fprintf(stderr,"%s\n",errtext);
}


/******************************************************************************\
| Function: main                                                               |
|                                                                              |
| Purpose:                                                                     |
| Check commandline and initiate all tasks                                     |
|                                                                              |
| Input:                                                                       |
| argc -> Number of arguments                                                  |
| argv -> Argument list                                                        |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
int main(int argc, char* argv[])
{
// To decide which task to start
   int DoDump=0;
   int DoFlash=0;
   int DoScan=0;
   int DoIdentify=0;
   int DoVersion=0;
   int DoQueryBT=0;
   int DoSetBT=0;
   int DoSetBTRW=0;
   int DoClearBT=0;
   int DoQueryRPC=0;
   int DoResetRPC=0;

// Returncode
   ErrorCode rc=E_OK;

// Other options
   bool skipdisclaimer=false;
   bool force=false;
   int drivetype=-1;

// Find name of current executable (Unix and DOS/Windows)
   ExecutableName=strrchr(argv[0],'/');
   if (!ExecutableName)
      ExecutableName=strrchr(argv[0],'\\');

   if (!ExecutableName)
      ExecutableName=argv[0];
   else
      ExecutableName++;

   WelcomeScreen();

// Check commandline arguments
   int cmdidx=1;

   while (cmdidx<argc && argv[cmdidx][0]=='-')
   {
      if (!strcmp(&argv[cmdidx][1],"dump") && drive.CanDump())
         DoDump=1;
      else if (!strcmp(&argv[cmdidx][1],"flash") && drive.CanFlash())
         DoFlash=1;
      else if (!strcmp(&argv[cmdidx][1],"scan"))
         DoScan=1;
      else if (!strcmp(&argv[cmdidx][1],"identify"))
         DoIdentify=1;
      else if (!strcmp(&argv[cmdidx][1],"querybt"))
         DoQueryBT=1;
      else if (!strcmp(&argv[cmdidx][1],"setbt"))
         DoSetBT=1;
      else if (!strcmp(&argv[cmdidx][1],"setbtrw"))
         DoSetBTRW=1;
      else if (!strcmp(&argv[cmdidx][1],"clearbt"))
         DoClearBT=1;
      else if (!strcmp(&argv[cmdidx][1],"queryrpc"))
         DoQueryRPC=1;
      else if (!strcmp(&argv[cmdidx][1],"resetrpc"))
         DoResetRPC=1;
      else if (!strcmp(&argv[cmdidx][1],"version"))
         DoVersion=1;
#ifdef _DEBUG
      else if (!strcmp(&argv[cmdidx][1],"d"))
      {
         debug=true;
         drive.SetDebug(true);
      }
#endif
      else if (!strcmp(&argv[cmdidx][1],"s"))
         skipdisclaimer=true;
      else if (!strcmp(&argv[cmdidx][1],"f"))
         force=true;
      else if (!strcmp(&argv[cmdidx][1],"v"))
         verbosity++;
      else if (!strcmp(&argv[cmdidx][1],"t"))
      {
         if (drivetype==-1)
            drivetype=atoi(argv[++cmdidx]);
         else
         {
            printf("Using Option -t more than once\n");
            return 1;
         }
      }
      else
      {
         printf("Unknown option: %s\n",argv[cmdidx]);
         return 1;
      }

      cmdidx++;
   }

// Check for valid commandline
   if (
      ((DoDump+DoFlash+DoScan+DoVersion+DoIdentify+DoQueryBT+DoSetBT+DoSetBTRW+DoClearBT+DoQueryRPC+DoResetRPC)!=1)	// Only one of these Options allowed
    ||	(DoDump && !drive.CanDump())			// -dump although drive cannot dump
   ||	(DoFlash && !drive.CanFlash())			// -flash although drive cannot flash
   ||	((DoSetBT || DoSetBTRW || DoClearBT) && !drive.CanBitset()) // booktype changes although drive can't
   ||	(DoScan && (argc-cmdidx))				// -scan including further parameters
   ||	((DoQueryBT || DoClearBT || DoQueryRPC) && (argc-cmdidx)!=1) // -querybt, -clearbt and -queryrpc need one parameter
   ||	((DoDump || DoFlash || DoSetBTRW || DoIdentify || DoResetRPC) && (argc-cmdidx)!=2)		// wrong number of parameters for flash, dump, identify, setbtrw and resetrpc
   ||	(DoSetBT && (argc-cmdidx)!=4)									// -setbt needs three parameters
   ||	((drivetype!=-1) && DoDump && (!drive.NeedDriveTypeDump()))		// drivetype not supplied although needed
   ||	((drivetype!=-1) && DoFlash && (!drive.NeedDriveTypeFlash()))	// drivetype not supplied although needed
   ||	((drivetype==-1) && DoDump && (drive.NeedDriveTypeDump()))		// drivetype supplied although not needed
   ||	((drivetype==-1) && DoFlash && (drive.NeedDriveTypeFlash()))	// drivetype supplied although not needed
   )
   {
      DisplayHowto();
      return 1;
   }

   if (!DoScan && !DoVersion && !DoQueryBT && !DoQueryRPC && !skipdisclaimer && !CheckDisclaimer())
      return 2;

   if (DoVersion)
      DisplayVersionInfo();
   else if (DoScan)
      rc=drive.ScanForDevices(true,ScanCallback);
   else if (DoFlash)
      rc=drive.FlashFirmware(argv[cmdidx],argv[cmdidx+1]);
   else if (DoDump)
      rc=drive.DumpFirmware(argv[cmdidx],argv[cmdidx+1]);
   else if (DoIdentify)
      rc=drive.IdentifyNewDrive(argv[cmdidx],argv[cmdidx+1]);
   else if (DoQueryBT)
      rc=drive.QueryBooktype(argv[cmdidx]);
   else if (DoSetBT)
      rc=drive.SetBooktype(argv[cmdidx+3],argv[cmdidx],argv[cmdidx+1],argv[cmdidx+2]);
   else if (DoSetBTRW)
      rc=drive.SetBooktypeRW(argv[cmdidx+1],argv[cmdidx]);
   else if (DoClearBT)
      rc=drive.ClearBTSettings(argv[cmdidx]);
   else if (DoQueryRPC)
      rc=drive.GetRegion(argv[cmdidx]);
   else if (DoResetRPC)
      rc=drive.ResetRegion(argv[cmdidx],argv[cmdidx+1],force);
   else
      printf("You should not get to this line\n");

   if (rc && (rc!=E_CANCELLED))
      Error(rc);

   return rc;
}
