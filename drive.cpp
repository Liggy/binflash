/******************************************************************************\
| Revision history:                                                            |
|                                                                              |
| 0.91 - 20040830 - First version including this file                          |
| 0.92 - 20040830 - Match versions                                             |
| 0.93 - 20040910 - Change callback in DeviceScan - debug variable only in     |
|                   Debug build                                                |
|      - 20040910 - Change handling of devicename for Mac                      |
| 0.95 - 20040916 - Add bitsetting functions (no 0.94 for syncing)             |
|      - 20040925 - Remove mode sense DVD check in device scan (safe mode)     |
| 1.00 - 20041013 - No change. Release version 1.0                             |
| 1.01 - 20041101 - Check for '.' in firmware version during device scan       |
| 1.03 - 20041206 - Remove '.' check again due to firmwares without dot        |
| 1.11 - 20041228 - Add check for devicetype==0x05 (CD-ROM) in DriveEnum       |
| 1.13 - 20050312 - Changed scsiSay checks to <0                               |
| 1.20 - 20050925 - Add support for mediatype DVD+RW-DL                        |
| 1.21 - 20051009 - Rename GetDriveInfo to GetInquiry / DriveInfo to Inquiry   |
| 1.22 - 20060722 - Add IsUnitReady                                            |
| 1.34 - 20061026 - Fix IsUnitReady (change = to ==)                           |
| 1.52 - 20100909 - Allow optional SCSI trace                                  |
| 1.56 - 20110318 - Add region code display                                    |
\******************************************************************************/

#include <stdarg.h>
#include <string.h>
#include "plscsi/plscsi.h"

#include "types.h"
#include "drive.h"


// Null device
#if (_WIN32 || __MSDOS__)
#define NULL_FD fopen("NUL", "w")
#else
#define NULL_FD fopen("/dev/null", "w")
#endif

// Drive interface version
static const char* DriveVersion="1.52";

// Dummy callback function that does nothing
static int dummy(MessageType,const char*,...);

#if 0
#if (_WIN32 || __MSDOS__)
#define trace "c:\\binflash.trc"
#else
#define trace "/binflash.trc"
#endif
#else
#define trace NULL
#endif

/******************************************************************************\
| Function: dummy                                                              |
|                                                                              |
| Purpose:                                                                     |
| Callback function if none supplied                                           |
|                                                                              |
| Input:                                                                       |
| MessageType (warning, fatal, error, info, ...)                               |
| format string (printf)                                                       |
| {optional arguments}                                                         |
|                                                                              |
| Returnvalues:                                                                |
| 0 -> This function will allways succeed and return nothing                   |
\******************************************************************************/
static int dummy(MessageType, const char *format,...)
{
// Do nothing
   va_list arg_ptr;

   va_start(arg_ptr,format);
   va_end(arg_ptr);

   return 0;
}


/******************************************************************************\
| Constructor: DRIVE::DRIVE                                                    |
|                                                                              |
| Purpose:                                                                     |
| Initialize class                                                             |
|                                                                              |
| Input:                                                                       |
| None                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| None                                                                         |
\******************************************************************************/
DRIVE::DRIVE()
{
   callback=dummy;
#ifdef _DEBUG
   debug=false;
#endif
}


/******************************************************************************\
| Destructor: DRIVE::~DRIVE                                                    |
|                                                                              |
| Purpose:                                                                     |
| Destructor. Free memory, close devices                                       |
|                                                                              |
| Input:                                                                       |
| None                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| None                                                                         |
\******************************************************************************/
DRIVE::~DRIVE()
{
}


/******************************************************************************\
| Method: DRIVE::GetDriveVersion                                               |
|                                                                              |
| Purpose:                                                                     |
| Retrieve version of drive interface                                          |
|                                                                              |
| Input:                                                                       |
| none                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| Pointer to version of drive interface                                        |
\******************************************************************************/
const char* DRIVE::GetDriveVersion() const
{
   return DriveVersion;
}


/******************************************************************************\
| Method: DRIVE::GetMediatypeFromString                                        |
|                                                                              |
| Purpose:                                                                     |
| Convert string to mediatype                                                  |
|                                                                              |
| Input:                                                                       |
| MTString -> Mediatype as string                                              |
|                                                                              |
| Returnvalues:                                                                |
| Mediatype or MT_Error                                                        |
\******************************************************************************/
Mediatype DRIVE::GetMediatypeFromString(char* MTString)
{
   if (!stricmp(MTString,"DVD+R"))
      return MT_PlusR;
   else if (!stricmp(MTString,"DVD-ROM"))
      return MT_ROM;
   else if (!stricmp(MTString,"DVD-R"))
      return MT_MinusR;
   else if (!stricmp(MTString,"DVD+RW"))
      return MT_PlusRW;
   else if (!stricmp(MTString,"DVD-RW"))
      return MT_MinusRW;
   else if (!stricmp(MTString,"DVD+R9") || !stricmp(MTString,"DVD+R-DL"))
      return MT_PlusR9;
   else if (!stricmp(MTString,"DVD+RW9") || !stricmp(MTString,"DVD+RW-DL"))
      return MT_PlusRW9;
   else if (!stricmp(MTString,"CD-R"))
      return MT_CDR;
   else if (!stricmp(MTString,"CD-RW"))
      return MT_CDRW;

   return MT_error;

}


/******************************************************************************\
| Method: DRIVE::GetMediatypeAsString                                          |
|                                                                              |
| Purpose:                                                                     |
| Convert mediatype to string                                                  |
|                                                                              |
| Input:                                                                       |
| MT -> Mediatype                                                              |
|                                                                              |
| Returnvalues:                                                                |
| A string corresponding to the mediatype                                      |
\******************************************************************************/
const char* DRIVE::GetMediatypeAsString(Mediatype MT)
{
   switch (MT & 0xfff0)
   {
   case (MT_PlusR & 0xfff0):
      return "DVD+R";
   case (MT_MinusR & 0xfff0):
      return "DVD-R";
   case (MT_PlusRW & 0xfff0):
      return "DVD+RW";
   case (MT_MinusRW & 0xfff0):
      return "DVD-RW";
   case (MT_PlusR9 & 0xfff0):
      return "DVD+R-DL";
   case (MT_PlusRW9 & 0xfff0):
      return "DVD+RW-DL";
   case (MT_ROM & 0xfff0):
      return "DVD-ROM";
   case (MT_RAM & 0xfff0):
      return "DVD-RAM";
   }
   switch (MT)
   {
   case (MT_CDR):
      return "CD-R";
   case (MT_CDRW):
      return "CD-RW";
   case (MT_CDROM):
      return "CD-ROM";
   default:
      break;
   }

   return "unknown";
}


/******************************************************************************\
| Method: DRIVE::GetInquiry                                                    |
|                                                                              |
| Purpose:                                                                     |
| Retrieve information about the supplied device                               |
|                                                                              |
| Input:                                                                       |
| none                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| false -> Error occured while scanning                                        |
| true -> Everything was OK                                                    |
\******************************************************************************/
bool DRIVE::GetInquiry(Scsi* scsi,Inquiry* driveinfo)
{
   u8 buffer[0x80];

   memset(driveinfo,0,sizeof(Inquiry));
   memset(cdb,0,CDB_SIZE);

   cdb[0]=0x12;cdb[4]=0x60;

   if (scsiSay(scsi,(char*) cdb,6,(char*) buffer,0x60,X1_DATA_IN)<0)
      return false;

   memcpy(driveinfo->vendor,buffer+8,8);
   memcpy(driveinfo->identification,buffer+16,16);
   memcpy(driveinfo->revision,buffer+32,4);
   memcpy(driveinfo->specific,buffer+36,20);

   driveinfo->devicetype=buffer[0]&0x1f;

   return true;
}


/******************************************************************************\
| Method: DRIVE::GetRegion                                                     |
|                                                                              |
| Purpose:                                                                     |
| Retrieve region code information from drive                                  |
|                                                                              |
| Input:                                                                       |
| device -> Name of device                                                     |
| info   -> Pointer to RegionInfo element. Will be filled with result          |
                                                                               |
| Returnvalues:                                                                |
| None - Failed command indicates RPC1                                         |
\******************************************************************************/
ErrorCode DRIVE::GetRegion(char* device,RegionInfo* rpc)
{
   Scsi* scsi;

   u8 buffer[8];

   scsi=OpenDevice(device);
   if (!scsi)
      return E_SCSIOPEN;

//TODO: Should query for CSS capabilities first
   if (rpc)
      memset(rpc,0,sizeof(RegionInfo)); // Assume RPC1 - will be filled later

   memset(cdb,0,CDB_SIZE);
   cdb[0]=0xa4;  // Report key
   cdb[9]=0x08;  // Allocation length
   cdb[10]=0x08; // Report RPC State

   if (scsiSay(scsi,(char*) cdb,12,(char*) buffer,8,X1_DATA_IN)>=0)
   {
      u8 region=1;

      while (buffer[5])
      {
         if (!(buffer[5]&1))
            break;
         region++;
         buffer[5]>>=1;
      }
      if (region==9)
         region=0;

      if (rpc)
      {
         rpc->RPC2=(buffer[6])?true:false;
         rpc->UserChanges=buffer[4]&0x07;
         rpc->VendorChanges=(buffer[4]>>3)&0x07;
         rpc->Region=region;
      }
      else
      {
         callback(info,"Region control is RPC%d\n",buffer[6]?2:1);
         callback(info,"%d user changes left\n",buffer[4]&0x07);
         callback(info,"%d vendor changes left\n",(buffer[4]>>3)&0x07);
         if (region)
            callback(info,"Drive is set to region %d\n",region);
         else
            callback(info,"Drive region has not been set\n");
      }
   }
   else if (!rpc)
      callback(info,"Region control is RPC1\n");

   scsiClose(scsi);

   return E_OK;
}


/******************************************************************************\
| Method: DRIVE::IsUnitReady                                                   |
|                                                                              |
| Purpose:                                                                     |
| Check whether there's a disc in the drive                                    |
|                                                                              |
| Input:                                                                       |
| none                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| false -> No disc in drive / Unit not ready                                   |
| true -> Disc in drive / Unit ready                                           |
\******************************************************************************/
bool DRIVE::IsUnitReady(Scsi* scsi)
{
   u8 sense[0x10];
   memset(cdb,0,CDB_SIZE);

   if (scsiSay(scsi,(char*) cdb,6,0,0,X0_DATA_NOT)<0)
   {
      if (scsiGetSense(scsi,(char*) sense,0x10,0)>=14)
      {
         if ((sense[12]!=0x3a) && (sense[12]==0x3e)) // Drive busy
            return true;
      }
      return false;
   }

   return true;
}


/******************************************************************************\
| Method: DRIVE::GetDiscBooktype                                               |
|                                                                              |
| Purpose:                                                                     |
| Get booktype of the currently inserted disc                                  |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handle                                                          |
|                                                                              |
| Returnvalues:                                                                |
| booktype                                                                     |
\******************************************************************************/
int DRIVE::GetDiscBooktype(Scsi* scsi)
{
   u8 buffer[8];
   int rc;

   memset(cdb,0,CDB_SIZE);

   cdb[0]=0xad;cdb[9]=0x08;

   if (scsiSay(scsi,(char*) cdb,12,(char*) buffer,8,X1_DATA_IN)>=0)
      rc=buffer[4];
   else
   {
      cdb[7]=0x11;
      if (scsiSay(scsi,(char*) cdb,12,(char*) buffer,8,X1_DATA_IN)>=0)
         rc=buffer[4];
      else
         rc=-1;
   }

   return rc;
}


/******************************************************************************\
| Method: DRIVE::GetDisctype                                                   |
|                                                                              |
| Purpose:                                                                     |
| Get disctype of the currently inserted disc                                  |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handle                                                          |
|                                                                              |
| Returnvalues:                                                                |
| disctype                                                                     |
\******************************************************************************/
int DRIVE::GetDisctype(Scsi* scsi)
{
   u8 buffer[8];
   int rc;

   memset(cdb,0,CDB_SIZE);
   cdb[0]=0x46;cdb[1]=0x02;cdb[8]=0x08;

   if (scsiSay(scsi,(char*) cdb,10,(char*) buffer,8,X1_DATA_IN)<0)
      rc=-1;
   else
   {
      switch (buffer[7])
      {
      case 0x10:
         rc=MT_ROM;break;
      case 0x11:
         rc=MT_MinusR;break;
      case 0x12:
         rc=MT_RAM;break;
      case 0x13:case 0x14:
         rc=MT_MinusRW;break;
      case 0x1a:
         rc=MT_PlusRW;break;
      case 0x1b:
         rc=MT_PlusR;break;
      case 0x2b:
         rc=MT_PlusR9;break;
//TODO: Try to recognize DVD+RW9
      case 0xa:
         rc=MT_CDRW;break;
      case 0x9:
         rc=MT_CDR;break;
      case 0x8:
         rc=MT_CDROM;break;
      default:
         rc=MT_error;
      }
   }

   return rc;
}


/******************************************************************************\
| Method: DRIVE::ScanForDevices                                                |
|                                                                              |
| Purpose:                                                                     |
| Enumerate all supported devices                                              |
|                                                                              |
| Input:                                                                       |
| none                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
ErrorCode DRIVE::ScanForDevices(bool Unsupported,EnumPrototype InfoFunction)
{
   Inquiry driveinfo;
   DriveType dt;
   int devices=0;
   Scsi* scsi;
   char device[256];

   scsi=newScsi();
   if (!scsi)
      return E_SCSINEW;

   callback(info,"List of supported devices:\n");

   scsiSetErr(scsi, NULL_FD);
   while (scsiReadName(scsi, device, 255) >= 0)
   {
      callback(talky,"Trying to open device %s\n",device);
      if (!scsiOpen(scsi,device))
      {
         scsiLimitSense(scsi,0x0e);
         scsiLimitSeconds(scsi,20,0);
         if (GetInquiry(scsi,&driveinfo) && (driveinfo.devicetype==0x05))
         {
            callback(verbose,"Trying to query device %s\n",device);

            dt=IdentifyDrive(scsi);
            if (
                (Unsupported && (dt!=DT_INVALID))
             || (!Unsupported && (dt!=DT_INVALID) && (dt!=DT_UNKNOWN))
               )
            {
               devices++;

               if (InfoFunction)
                  InfoFunction(device,&driveinfo);
            }
            else
               callback(verbose,"device %s %s %s unsupported\n",
                  driveinfo.vendor,driveinfo.identification,driveinfo.revision);
         }
         scsiClose(scsi);
      }
   }
   scsiSetErr(scsi, stderr);

   if (!devices)
      callback(warning,"No supported devices found\n");

   return E_OK;
}


/******************************************************************************\
| Method: DRIVE::SetCallback                                                   |
|                                                                              |
| Purpose:                                                                     |
| If a callback routine is supplied use this for callback otherwise use dummy  |
|                                                                              |
| Input:                                                                       |
| Address of callback routine (defaults to NULL)                               |
|                                                                              |
| Returnvalues:                                                                |
| None                                                                         |
\******************************************************************************/
void DRIVE::SetCallback(CallbackPrototype cbroutine)
{
   if (cbroutine)
      callback=cbroutine;
   else
      callback=dummy;
}


/******************************************************************************\
| Method: DRIVE::SetDebug                                                      |
|                                                                              |
| Purpose:                                                                     |
| Enables debugging mode                                                       |
|                                                                              |
| Input:                                                                       |
| debugmode that will be used                                                  |
|                                                                              |
| Returnvalues:                                                                |
| None                                                                         |
\******************************************************************************/
#ifdef _DEBUG
void DRIVE::SetDebug(bool debugmode)
{
   debug=debugmode;
}
#endif


/******************************************************************************\
| Method: DRIVE::OpenDevice                                                    |
|                                                                              |
| Purpose:                                                                     |
| Create a SCSI handler for a given devicename                                 |
|                                                                              |
| Input:                                                                       |
| name of the device to use                                                    |
|                                                                              |
| Returnvalues:                                                                |
| SCSI handler (NULL in case of error)                                         |
\******************************************************************************/
Scsi* DRIVE::OpenDevice(char* devicename)
{
// Check for DOS-Name
   char ParsedDevice[256];
   Scsi* scsi;

   scsi=newScsi();
   if (!scsi)
      return NULL;

   if (devicename[1]==':' && !devicename[2])
   {
      *devicename&=0xdf;
#ifdef __APPLE__
      strncpy(ParsedDevice,devicename,255);
#else
      sprintf(ParsedDevice,"\\\\.\\%c:",*devicename);
#endif
   }
   else
      strncpy(ParsedDevice,devicename,255);

   callback(talky,"Trying to open device %s\n",ParsedDevice);
   if (scsiOpen(scsi,ParsedDevice,trace))
      return NULL;

   scsiLimitSense(scsi,0xe);
   scsiLimitSeconds(scsi,300,0);

   return scsi;
}
