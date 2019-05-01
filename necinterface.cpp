/******************************************************************************\
| Revision history:                                                            |
|                                                                              |
| 0.90 - 20040828 - First public beta version                                  |
| 0.91 - 20040829 - Renamed function                                           |
|                 - Additional checks before flashing                          |
|        20040830 - Extended error checking                                    |
| 0.92 - 20040830 - Match versions                                             |
| 0.93 - 20040910 - debug variable only in Debug build                         |
| 0.94 - 20040914 - Enable flashing from given buffer                          |
| 0.95 - 20040915 - Fix stepcount while flashing                               |
|                 - Fix broken ReadFirmware filesize check                     |
|                 - Passed wrong variable to GetDriveTypeFromBuffer when       |
|                   flashing                                                   |
|        20040916 - Add bitsetting functions                                   |
|        20041003 - Add flashing from NEC EXE-Flasher                          |
|        20041004 - Add new function QuerySingleBooktype                       |
| 1.00 - 20041008 - Add forgotten scsiClose functions                          |
|        20041013 - Change output for error on getting Ricoh settings          |
|        20041014 - Leave safemode bank 1 for 1x00 drives when flashing        |
|        20041014 - Add support for 6x00 drives                                |
| 1.01 - 20041016 - Fixed bug when reading 6x00 firmwares                      |
| 1.02 - 20041019 - Fixed severe bug when flashing 6x00 drives                 |
| 1.03 - 20041115 - Fixed output when flashing and dumping. (Cosmetic change)  |
|        20041203 - Added support for NEC-3450 drives                          |
| 1.10 - 20041225 - Added support for NEC-3520 drives                          |
| 1.11 - 20050121 - Added support for NEC-6450 drives                          |
|        20050129 - Fixed severe bug when flashing NEC-1100 and 5100 drives    |
| 1.12 - 20050310 - Fixed bitsetting on NEC-3520 drives                        |
| 1.14 - 20050327 - Added support for NEC-3530 drives                          |
|        20050327 - Support flashing NEC-5x00 and NEC-6x00 slave firmwares     |
| 1.15 - 20050331 - Fixed flashing 2* and 3* firmwares with integrated Binflash|
| 1.16 - 20050412 - Added support for NEC-3540 drives                          |
| 1.17 - 20050524 - Added support for NEC-6650 drives                          |
| 1.18 - 20050629 - Try to disable safemode in case of problems during flash   |
|        20050719 - Recognzie "K100" as NEC1x00 drive                          |
|        20050802 - Try to prevent mixing up master and slave firmwares on     |
|                   notebook drives                                            |
| 1.19 - 20050910 - Recognzie "K111" as NEC1x00 drive                          |
| 1.20 - 20051003 - Add support for ND4550 drives.                             |
|        20051003 - Changed warning text for notebook drive master/slave issue |
|        20051003 - Change drivetype from int to enum                          |
|        20051004 - Modularize flashing routines                               |
| 1.21 - 20051016 - Add support for ND3550 drives.                             |
| 1.21a- 20051020 - Don't allow mixing up master and slave on notebook drives  |
| 1.22 - 20051029 - Fix bug in recognition of ND3550 drives                    |
| 1.23 - 20051209 - Add support for ND7550 drives                              |
| 1.24 - 20060105 - Add support for ND3551 drives                              |
| 1.25 - 20060210 - Check for crossflashed 2M drives                           |
| 1.26 - 20060218 - Add support for ND4550, ND4551 and ND7551 drives           |
|        20060312 - Change notebook drive master and slave detection           |
| 1.27 - 20060326 - Improve notebook drive master and slave detection again    |
| 1.29 - 20060525 - Add support for x56x and 357x drives                       |
| 1.30 - 20060531 - Output Dump ID in talky mode                               |
|        20060608 - Check laptop drives without firmware for correct bootcode  |
|        20060625 - Corrected ID for 3650/3651 drives                          |
| 1.31 - 20060628 - Do not leave safe mode for 2M drives without Dump ID       |
|        20060628 - Code cleanup with GetFlashSizeFromDriveType                |
|        20060629 - Code cleanup in GetDriveTypeFromBuffer                     |
|        20060629 - Code cleanup in ReadFirmware                               |
|        20060720 - Fix flashing of slimline drives without valid firmware     |
|        20060722 - Make sure that there's no disc inside the drive            |
| 1.32 - 20060927 - Support dumping and flashing Optiarc half height drives    |
| 1.34 - 20061006 - Correct data for 5540 (master) firmwares                   |
|        20060626 - Added routine to identify unknown drives                   |
| 1.35 - 20061126 - Correct data for 7543 (master) firmwares                   |
| 1.36 - 20061214 - Add support for 7170S (SATA) drives                        |
| 1.39 - 20070404 - Start adding support for HR-1100A HD-DVD drives            |
|        20070409 - Add support for 7630A (Slot-In) drives                     |
| 1.41 - 20070816 - Check bootcode version if provided by firmware file        |
| 1.42 - 20071016 - Add support for AD-5630A/AD-5633A drives                   |
| 1.43 - 20071214 - Add support for AD-5200A, 5203A, 5200S, 5203S, 7200A,      |
|                   7203A, 7200S and 7203S drives                              |
| 1.44 - 20080212 - Add support for AD-5201A, 5201S, 7201A, 7201A, 7590A and   |
|                   7593A drives                                               |
| 1.45 - 20080222 - Run identification routine even if every command fails     |
|                 - Add support for further drives                             |
| 1.46 - 20081215 - Add support for flashing files containing multiple         |
|                   firmwares                                                  |
|                 - Display version before flashing (command line)             |
| 1.46a- 20081215 - Bugfix for flashing files that don't provide bootcode ver. |
| 1.46b- 20090216 - Change order of dumping and retrieving bootcode on 1M fw   |
| 1.47p- 20090401 - Start adding support for AD-7240S                          |
| 1.48 - 20090907 - Add support for AD-5670S and AD-5960S                      |
| 1.49 - 20091214 - Add support for AD-7700S series and LiteOn 7240S OEMs      |
| 1.49b- 20091218 - More precise recognition of LiteOn OEM bootcode version    |
| 1.50 - 20100119 - Add support for AD-7230S and AD-7260S series               |
|                 - Rewrite of NECDRIVE::QueryBooktype (mostly rearrange)      |
| 1.51 - 20100823 - Extend drive detection - check for certain letters         |
| 1.52 - 20100908 - Improve handling of BluRay drives                          |
|                 - Add support for AD-7700H                                   |
| 1.53 - 20101215 - Add HP 724xS OEM drives boot ID                            |
| 1.54 - 20110123 - Add support for AD-x591x drives                            |
| 1.55 - 20110220 - Always retrieve bootcode version after dumping - fix for   |
|                   some older NEC drives                                      |
| 1.56 - 20110318 - Add region code reset                                      |
|                 - Add support for AD-5970H series                            |
| 1.57 - 20110615 - Reorganize region code reset and add optional messages     |
|        20110810 - Add support for AD-7280S series                            |
| 1.58 - 20111121 - Add AD-x69xH drives                                        |
|                 - New forum link (myce instead of cdfreaks)                  |
|        20111123 - Fix AD-7280S flashing support (Flash2MOpti4)               |
| 1.59 - 20111206 - Add AD-x71xH drives                                        |
| 1.60 - 20111222 - Add G305 drivetype                                         |
| 1.61 - 20120110 - Add x25x, x27x, x29x, x80x drives                          |
| 1.62 - 20120204 - Add 774x drives                                            |
| 1.63 - 20130103 - Make Opti4 flash routine compatible with official flasher  |
| 1.64 - 20130519 - Add 7717 and x79x drives                                   |
\******************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "plscsi/plscsi.h"

#include "types.h"
#include "drive.h"
#include "necinterface.h"
#include "necinternal.h"


#define DUMP_BLOCK_SIZE 0x1000
#define FULLDUMP 0

static const char* NECVersion="1.64";

static bool sm0,sm1;

/******************************************************************************\
| constructor: NECDRIVE::NECDRIVE                                              |
|                                                                              |
| Purpose:                                                                     |
| Initialize class                                                             |
|                                                                              |
| Input:                                                                       |
| Function for callback. Defaulted to NULL                                     |
|                                                                              |
| Returnvalues:                                                                |
| None                                                                         |
\******************************************************************************/
NECDRIVE::NECDRIVE(CallbackPrototype cbroutine)
{
   SetCallback(cbroutine);
}


/******************************************************************************\
| method: NECDRIVE::~NECDRIVE                                                  |
|                                                                              |
| Purpose:                                                                     |
| Destructor. Free memory                                                      |
|                                                                              |
| Input:                                                                       |
| None                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| None                                                                         |
\******************************************************************************/
NECDRIVE::~NECDRIVE()
{
}


/******************************************************************************\
| method: NECDRIVE::GetInterfaceVersion                                        |
|                                                                              |
| Purpose:                                                                     |
| Supply version of this interface                                             |
|                                                                              |
| Input:                                                                       |
| None                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| Interface version                                                            |
\******************************************************************************/
const char* NECDRIVE::GetInterfaceVersion() const
{
   return NECVersion;
}


/******************************************************************************\
| method: NECDRIVE::GetInternalVersion                                         |
|                                                                              |
| Purpose:                                                                     |
| Supply version of internal drive access                                      |
|                                                                              |
| Input:                                                                       |
| None                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| Internal version                                                             |
\******************************************************************************/
const char* NECDRIVE::GetInternalVersion() const
{
   return ::GetInternalVersion();
}


/******************************************************************************\
| method: NECDRIVE::GetManufacturer                                            |
|                                                                              |
| Purpose:                                                                     |
| Supply supported manufacturer for this interface                             |
|                                                                              |
| Input:                                                                       |
| None                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| Manufacturer Name                                                            |
\******************************************************************************/
const char* NECDRIVE::GetManufacturer() const
{
   return "NEC";
}


/******************************************************************************\
| method: NECDRIVE::CanDump                                                    |
|                                                                              |
| Purpose:                                                                     |
| Supply whether firmware can be dumped                                        |
|                                                                              |
| Input:                                                                       |
| None                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Firmware can be dumped                                              |
| false -> Firmware cannot be dumped                                           |
\******************************************************************************/
const bool NECDRIVE::CanDump() const
{
   return true;
}


/******************************************************************************\
| method: NECDRIVE::CanFlash                                                   |
|                                                                              |
| Purpose:                                                                     |
| Supply whether firmware can be flashed                                       |
|                                                                              |
| Input:                                                                       |
| None                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Firmware can be flashed                                             |
| false -> Firmware cannot be flashed                                          |
\******************************************************************************/
const bool NECDRIVE::CanFlash() const
{
   return true;
}


/******************************************************************************\
| method: NECDRIVE::CanBitset                                                  |
|                                                                              |
| Purpose:                                                                     |
| Supply whether the drive supports bitsetting                                 |
|                                                                              |
| Input:                                                                       |
| None                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Booktype can be changed                                             |
| false -> Booktype cannot be changed                                          |
\******************************************************************************/
const bool NECDRIVE::CanBitset() const
{
   return true;
}


/******************************************************************************\
| method: NECDRIVE::NeedDriveTypeDump                                          |
|                                                                              |
| Purpose:                                                                     |
| Supply whether drivetype (-t) is needed for dumping                          |
|                                                                              |
| Input:                                                                       |
| None                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Drive type is needed for dumping                                    |
| false -> Drive type is not needed for dumping                                |
\******************************************************************************/
const bool NECDRIVE::NeedDriveTypeDump() const
{
   return false;
}


/******************************************************************************\
| method: NECDRIVE::NeedDriveTypeFlash                                         |
|                                                                              |
| Purpose:                                                                     |
| Supply whether drivetype (-t) is needed for flashing                         |
|                                                                              |
| Input:                                                                       |
| None                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Drive type is needed for flashing                                   |
| false -> Drive type is not needed for flashing                               |
\******************************************************************************/
const bool NECDRIVE::NeedDriveTypeFlash() const
{
   return false;
}


/******************************************************************************\
| method: NECDRIVE::GetDriveType                                               |
|                                                                              |
| Purpose:                                                                     |
| Check the drivetype of the currently opened device                           |
|                                                                              |
| Input:                                                                       |
| none                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| -1 -> Drive is supported                                                     |
| >=0 -> Recognized drivetype                                                  |
\******************************************************************************/
DriveType NECDRIVE::IdentifyDrive(Scsi* scsi)
{
   return ::IdentifyDrive(scsi);
}


/******************************************************************************\
| method: NECDRIVE::GetDriveTypeFromBuffer                                     |
|                                                                              |
| Purpose:                                                                     |
| Retrieve Drivetype from given buffer                                         |
|                                                                              |
| Input:                                                                       |
| fwbuffer -> Buffer to firmware                                               |
| size - > Size of given buffer                                                |
| bootcode -> Expected bootcode version from firmware file. 0xffff->none given |
|                                                                              |
| Returnvalues:                                                                |
| drivetype for the given firmware                                             |
\******************************************************************************/
DriveType NECDRIVE::GetDriveTypeFromBuffer(u8* fwbuffer,size_t size,u16* bootcode)
{
   *bootcode=0xffff;

// Do not insert break after the *bootcode... line. The following command must
// always be executed too!
   switch(size)
   {
   case 634890:
      *bootcode=fwbuffer[0x9b008]<<8|fwbuffer[0x9b009];
   case 634888:
      return ::IdentifyDrive(0,fwbuffer+0x9b004);

   case 753670:
      *bootcode=fwbuffer[0xb8004]<<8|fwbuffer[0xb8005];
   case 753668:
      return ::IdentifyDrive(0,fwbuffer+0xb8000);

   case 1024006:
      *bootcode=fwbuffer[0xfa004]<<8|fwbuffer[0xfa005];
   case 1024004:
      return ::IdentifyDrive(0,fwbuffer+0xfa000);

   case 1966088:
      *bootcode=fwbuffer[0x1e0006]<<8|fwbuffer[0x1e0007];
   case 1966086:
      return ::IdentifyDrive(0,fwbuffer+0x1e0002);
   
   case 1835016:
      *bootcode=fwbuffer[0x1c0006]<<8|fwbuffer[0x1c0007];
   case 1835014:
      return ::IdentifyDrive(0,fwbuffer+0x1c0002);

   case 1769480:
      *bootcode=fwbuffer[0x1b0006]<<8|fwbuffer[0x1b0007];
   case 1769478:
      return ::IdentifyDrive(0,fwbuffer+0x1b0002);

   case 1901058:
      *bootcode=0xfffe; // Not relevant for these drives. Shall match value in RetrieveBootcodeVersion
      return ::IdentifyDrive(0,fwbuffer+0x42);

   default:
      *bootcode=0xffff; // Just to stop some compilers from complaining
   }

   return DT_INVALID;
}


/******************************************************************************\
| method: NECDRIVE::ReadFirmware                                               |
|                                                                              |
| Purpose:                                                                     |
| Scans firmware file for available firmwares                                  |
|                                                                              |
| Input:                                                                       |
| filename -> Filename for firmware                                            |
| FWSize   -> Size of provided firmware                                        |
| reqdt    -> requested drivetype                                              |
| reqbc    -> requested bootcode                                               |
|                                                                              |
| Returnvalues:                                                                |
| 0 -> Error reading firmware                                                  |
| !=0 -> Pointer to firmware                                                   |
\******************************************************************************/
u8* NECDRIVE::ReadFirmware(char* filename,u32* FWSize,DriveType reqdt,u16 reqbc)
{
   FILE* firmfile;
   bool found=false;
   DriveType founddt,bestdt=DT_INVALID;
   s32 filesize=0;
   u8 buffer[6];
   u16 bestbc=0;
   u32 offset=0,bestoffset=0,foundfwsize=0;
   u8* Firmware=0;

   firmfile=fopen(filename,"rb");

   if (!firmfile)
      return 0;

   fseek(firmfile,0,SEEK_END);
   filesize=(s32) ftell(firmfile);

   while (filesize>=6)
   {
      u16 foundbc=0xffff;
      fseek(firmfile,(long)filesize-6,SEEK_SET);
      fread(buffer,1,6,firmfile);

      founddt=::IdentifyDrive(0, buffer+2);
      if (founddt<0)
      {
         founddt=::IdentifyDrive(0, buffer);
         if (founddt>=0)
         {
            foundbc=(buffer[4]<<8)+buffer[5];
            offset=2;
         }
      }

      if (founddt<0) //Try to detect Blu-Ray drive firmwares. ID is stored at offset 0x42 from start (end-1900992)
      {
         if (filesize>=1901058)
         {
            fseek(firmfile,(long)filesize-1900992,SEEK_SET);
            fread(buffer,1,4,firmfile);
            founddt=::IdentifyDrive(0, buffer);
         }
      }

      if (founddt<0)
         break;

      switch (GetFlashSizeFromDriveType(founddt))
      {
      case FlashSize640K:
         filesize-=(634888+offset);
         foundfwsize=634888+offset;
         break;

      case FlashSize768K:
         filesize-=(753668+offset);
         foundfwsize=753668+offset;
         break;

      case FlashSize1M:
         filesize-=(1024004+offset);
         foundfwsize=1024004+offset;
         break;

      case FlashSize2M:
         filesize-=(1966086+offset);
         foundfwsize=1966086+offset;
         break;

      case FlashSize2MOpti: case FlashSize2MOpti2: case FlashSize2MHDDVD:
         filesize-=(1835014+offset);
         foundfwsize=1835014+offset;
         break;

      case FlashSize2MOpti3:case FlashSize2MOpti4:
         filesize-=(1769478+offset);
         foundfwsize=1769478+offset;
         break;

      case FlashSize2MOptiBD:
         filesize-=1901058;
         foundfwsize=1901058;
         break;

      default:
         filesize=-1;
         break;
      }

      if (
         (!found) // First run - accept everything
      || ((bestdt!=reqdt) && (founddt==reqdt)) // Previous one was for different drive, this one is what we requested
      || ((bestdt==reqdt) && (bestbc!=reqbc) && (founddt==reqdt) && ((foundbc==reqbc)||(foundbc==0xffff))) // Already had right drive but wrong bootcode. This one has the correct bootcode (or 0xffff bootcode)
         )
      {
         if (filesize>=0)
         {
            bestdt=founddt;
            bestbc=foundbc;
            bestoffset=filesize;
            *FWSize=foundfwsize;
            found=true;
         }
      }
   }

   if (found && (*FWSize<0x200020))
   {
      Firmware=new u8[*FWSize];
      if (Firmware)
      {
         fseek(firmfile,bestoffset,SEEK_SET);
         if (fread(Firmware,1,*FWSize,firmfile)!=*FWSize)
         {
            delete[] Firmware;
            Firmware=0;
            *FWSize=0;
         }
      }
   }

   fclose(firmfile);

   return Firmware;
}


/******************************************************************************\
| method: NECDRIVE::IdentifyFirmware                                           |
|                                                                              |
| Purpose:                                                                     |
| Get version and manufacturer information of the supplied firmware            |
|                                                                              |
| Input:                                                                       |
| firmware -> Pointer to firmware                                              |
| fwsize -> Size of firmware                                                   |
| driveinfo -> Pointer to structure for identification                         |
|                                                                              |
| Returnvalues:                                                                |
| true -> Firmware could be identified                                         |
| false -> Firmware could not be identified                                    |
\******************************************************************************/
bool NECDRIVE::IdentifyFirmware(u8* Firmware,size_t fwsize, Inquiry* driveinfo)
{
   const char* identify="\x07\x00\x03\x00\x78\x00\x78\x00\x78\x00";
   const int id_size=10;
   int id_offset=394;

   if (!driveinfo || !Firmware)
      return false;

   for (size_t i=0;i<(fwsize-(id_size+id_offset+sizeof(Inquiry)));i++)
   {
      if (!memcmp(Firmware+i,identify,id_size))
      {
         memset(driveinfo,0,sizeof(Inquiry));
         strncpy(driveinfo->vendor,(char*) (Firmware+i+id_offset),8);
         strncpy(driveinfo->identification,(char*) (Firmware+i+id_offset+8),16);
         strncpy(driveinfo->revision,(char*) (Firmware+i+id_offset+24),4);
         strncpy(driveinfo->specific,(char*) (Firmware+i+id_offset+28),20);

         return true;
      }
   }

   return false;
}


/******************************************************************************\
| method: NECDRIVE::FirmwareToFile                                             |
|                                                                              |
| Purpose:                                                                     |
| Read Firmware from drive and write to file                                   |
|                                                                              |
| Input:                                                                       |
| firmfile -> Target file handler                                              |
| start -> Start address of firmware                                           |
| end -> End address of firmware                                               |
| bank -> Bank used for Reading                                                |
|                                                                              |
| Returnvalues:                                                                |
| true -> Firmware successfully written                                        |
| false -> Error while writing firmware                                        |
\******************************************************************************/
bool NECDRIVE::FirmwareToFile(Scsi* scsi, FILE* firmfile, u32 start, u32 end,
                       bool bank, u16* chksum, u8 id)
{
   u8 buffer[DUMP_BLOCK_SIZE];
   u32 address;

   for (address=start;address<end;address+=DUMP_BLOCK_SIZE)
   {
      callback(step,"Reading FLASH at 0x%06lx in bank %d\n",address,bank?1:0);

//HACK: Don't use the else branch
#if 1
      if (!ReadRAM(scsi,buffer,address,DUMP_BLOCK_SIZE,bank,id))
         return false;
      if (fwrite(buffer,DUMP_BLOCK_SIZE,1,firmfile)!=1)
         return false;
#else
      memset(buffer,0x55,DUMP_BLOCK_SIZE);
      ReadRAM(scsi,buffer,address,DUMP_BLOCK_SIZE,bank,id);
      fwrite(buffer,DUMP_BLOCK_SIZE,1,firmfile);
#endif

      if (chksum)
      {
         for (int i=0;i<DUMP_BLOCK_SIZE;i+=2)
            *chksum+=(buffer[i]<<8)+buffer[i+1];
      }
   }

   return true;
}


/******************************************************************************\
| method: NECDRIVE::GetStepCount                                               |
|                                                                              |
| Purpose:                                                                     |
| Retrieve number of steps needed for flashing/dumping                         |
|                                                                              |
| Input:                                                                       |
| drivetype -> drivetype used for flashing/dumping                             |
| flash -> True when flashing, false when dumping                              |
|                                                                              |
| Returnvalues:                                                                |
| Number of steps needed while flashing                                        |
\******************************************************************************/
int NECDRIVE::GetStepCount(DriveType drivetype,bool flash)
{
   int index;
   int steps[]={
// Flash, Dump normal, Dump debug
// Adjusted to DUMP_BLOCK_SIZE 0x1000
       59, 155, 160, // Offset  0 - K1, L1
       70, 250, 256, // Offset  3 - K2
       44, 184, 192, // Offset  6 - L2
       69, 250, 256, // Offset  9 - K350
       69, 250, 252, // Offset 12 - K352, K353, K354, L251
      124, 480, 492, // Offset 15 - D355, K355, K356, K365, K366, K450, K451, K460, K461, L252, L350, L351
      124, 482, 494, // Offset 18 - K370, K371, K470, K471 - require safemode for dumping
      115, 450, 514, // Offset 21 - D57*, D77*, L260, L261, L450, L451, G175, Q63*, G63*, Q91*, G91*, Q96*, G96*
      116, 450, 514, // Offset 24 - Q20*, G20*, Q59*, G59*, Q67*, G67*
      112, 434, 514, // Offset 27 - Q24*, G24*, Q26*, G26*, Q97*, G97*
      110, 434, 514, // Offset 30 - Q28*, G28*, Q30*, G30*, Q74*, G74*
      115, 111, 111, // Offset 33 - M110
      111,   0,   0, // Offset 36 - C500, C505, C600, C605, B530, B535
      -1
   };

   index=0; // Flash
   if (!flash)
   {
      index=1; // Dump normal
#ifdef _DEBUG
      if (debug)
         index=2; // Dump debug
#endif
   }

   switch (drivetype)
   {
   case K1: case L1:
      return steps[index];
   case K2:
      return steps[index+3];
   case L2:
      return steps[index+6];
   case K350:
      return steps[index+9];
   case K352:case K353:case K354:case L251:
      return steps[index+12];
   case D355:case K355:case K356:case K365:case K366:case K450:case K451:case K460:case K461: 
   case L252:case L253:case L350:case L351:
      return steps[index+15];
   case K370:case K371:case K470:case K471:
      return steps[index+18];
   case D570:case D57D:case D573:case D770:case D773:case L260:case L261:case L450:case L451:case G175:
   case Q630:case Q633:case G630:case G633:
   case Q910:case Q913:case Q915:case Q918:case G910:case G913:case G915:case G918:
   case Q960:case Q963:case Q965:case Q968:case G960:case G963:case G965:case G968:
     return steps[index+21];
   case Q200:case Q201:case Q203:case Q205:case Q206:case Q208:
   case G200:case G201:case G203:case G205:case G206:case G208:
   case Q590:case Q591:case Q593:case Q595:case Q596:case Q598:
   case G590:case G591:case G593:case G595:case G596:case G598:
   case Q670:case Q673:case Q675:case Q678:case G670:case G673:case G675:case G678:
      return steps[index+24];
   case Q230:case Q231:case Q233:case Q235:case Q236:case Q238:
   case G230:case G231:case G233:case G235:case G236:case G238:
   case Q240:case Q241:case Q243:case Q245:case Q246:case Q248:
   case G240:case G241:case G243:case G245:case G246:case G248:
   case G25A:case G25B:case G25D:
   case Q260:case Q261:case Q263:case Q265:case Q266:case Q268:
   case G260:case G261:case G263:case G265:case G266:case G268:
   case Q700:case Q701:case Q703:case Q705:case Q706:case Q708:case Q70A:case Q70B:case Q70D:
   case G700:case G701:case G703:case G705:case G706:case G708:case G70A:case G70B:case G70D:
   case Q68A:case Q68B:case Q68D:
   case Q97A:case Q97B:case Q97D:case G97A:case G97B:case G97D:
   case Q71A:case Q71B:case Q71D:case G71A:case G71B:case G71D:
   case Q72A:case Q72B:case Q72D:case G72A:case G72B:case G72D:
     return steps[index+27];
   case G27A:case G27B:case G27D:
   case Q280:case Q281:case Q283:case Q285:case Q286:case Q288:
   case G280:case G281:case G283:case G285:case G286:case G288:
   case G29A:case G29B:case G29D:
   case Q300:case Q301:case Q303:case Q305:case Q306:case Q308:
   case G300:case G301:case G303:case G305:case G306:case G308:
   case Q325:case Q326:case Q328:case G325:case G326:case G328:
   case Q800:case Q801:case Q803:case Q805:case Q806:case Q808:case Q80A:case Q80B:case Q80D:
   case G800:case G801:case G803:case G805:case G806:case G808:case G80A:case G80B:case G80D:
   case Q74A:case Q74B:case Q74D:case G74A:case G74B:case G74D:
   case Q76A:case Q76B:case Q76D:case G76A:case G76B:case G76D:
   case Q98A:case Q98B:case Q98D:case G98A:case G98B:case G98D:
   case Q93A:case Q93B:case Q93D:case G93A:case G93B:case G93D:
    return steps[index+30];
   case M110:
      return steps[index+33];
   case C500:case C505:case C600:case C605:case B530:case B535:case B74A:
      return steps[index+36];
   case DT_INVALID:
      return 0;
   default: // Should not happen
      break;
   }

   return 0;
}


/******************************************************************************\
| method: NECDRIVE::RetrieveBootcodeVersion                                    |
|                                                                              |
| Purpose:                                                                     |
| Retrieve version of currently installed bootcode                             |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handle                                                          |
| drivetype -> Exact drivetype to flash                                        |
|                                                                              |
| Returnvalues:                                                                |
| Bootcode as 16 bit value. Same as GetLaptopBootcode()                        |
\******************************************************************************/
u16 NECDRIVE::RetrieveBootcodeVersion(Scsi* scsi, DriveType drivetype)
{
   bool WasSafeMode;
   u16 BootcodeVersion=0xffff;
   Inquiry driveinfo;
   FlashType ft;

   ft=GetFlashSizeFromDriveType(drivetype);
   if ((ft==FlashSize2MOptiBD) || (ft==FlashSize2MOptiBD2)) //No bootcode from Opti-BD drives
      return 0xfffe;

   if (!GetInquiry(scsi,&driveinfo))
      return 0xffff;

   if (driveinfo.revision[0]=='0')
      WasSafeMode=true;
   else
   {
      WasSafeMode=false;
      if (!SetSafeMode(scsi,drivetype,0,1))
         return 0xffff;
   }

   if (!GetInquiry(scsi,&driveinfo))
      BootcodeVersion=0xffff;
   else if (driveinfo.revision[0]!='0')
      BootcodeVersion=0xffff;
   else if (driveinfo.revision[1]=='.')
      BootcodeVersion=(driveinfo.revision[2]<<8) + (driveinfo.revision[3]);
   else if (!memcmp(driveinfo.vendor,"ATAPI   ",8)) // Stupid LiteOn iHAS drives need special treatment
   {
      if (!memcmp(driveinfo.identification,"iHAS124   Y     ",16) && !memcmp(driveinfo.revision,"0L0Z",4))
         BootcodeVersion=0x3038;
      else if (!memcmp(driveinfo.identification,"iHAS224   Y     ",16) && !memcmp(driveinfo.revision,"0L0Z",4))
         BootcodeVersion=0x3038;
      else if (!memcmp(driveinfo.identification,"iHAS324   Y     ",16) && !memcmp(driveinfo.revision,"0L1Z",4))
         BootcodeVersion=0x3038;
      else if (!memcmp(driveinfo.identification,"iHAS424   Y     ",16) && !memcmp(driveinfo.revision,"0L1Z",4))
         BootcodeVersion=0x3038;
      else if (!memcmp(driveinfo.identification,"DVD A  DH24AYL  ",16) && !memcmp(driveinfo.revision,"0P5Z",4))
         BootcodeVersion=0x3038;
      else if (!memcmp(driveinfo.identification,"DVD A  DH24AYS  ",16) && !memcmp(driveinfo.revision,"0P5Z",4))
         BootcodeVersion=0x3038;
   }
   else if (!memcmp(driveinfo.vendor,"PLDS    ",8)) // Another OEM vendor ID for LiteOn drives
   {
      if (!memcmp(driveinfo.identification,"DVD A  DH24AYS  ",16) && !memcmp(driveinfo.revision,"0100",4))
         BootcodeVersion=0x3038;
   }
   else if (!memcmp(driveinfo.vendor,"HP      ",8)) // Some HP OEM drives use this one
   {
      if (!memcmp(driveinfo.identification,"DVD Writer 1270r",16) && !memcmp(driveinfo.revision,"0L21",4))
         BootcodeVersion=0x3038;
   }
   else if (!memcmp(driveinfo.vendor,"IMATION ",8)) // Imation LiteOn Optiarc OEM. What a piece of crap
   {
      if (!memcmp(driveinfo.identification,"DVD+-RAM ELITE  ",16) && !memcmp(driveinfo.revision,"0M4Z",4))
         BootcodeVersion=0x3038;
   }
   else
      BootcodeVersion=0xffff;

   if (!WasSafeMode)
      SetSafeMode(scsi,drivetype,0,0);

   return BootcodeVersion;
}


/******************************************************************************\
| method: NECDRIVE::Flash640k                                                  |
|                                                                              |
| Purpose:                                                                     |
| Flash firmware on drives with 640k Flash (1x00, 5x00)                        |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handle                                                          |
| buffer -> Pointer to firmware                                                |
| drivetype -> Exact drivetype to flash                                        |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
FlashingError NECDRIVE::Flash640K(Scsi* scsi, u8* buffer, DriveType drivetype)
{
   u32 i;
   u16 checksum;
   bool ok=true;

   callback(step,"Entering safe mode in bank 0\n");
   if (!SetSafeMode(scsi,drivetype,0,1))
      return FE_ENTER_SAFEMODE;
   sm0=true;

   for (i=0;(i<0x03000) && ok;i+=0x1000)
   {
      callback(step,"Sending firmware to drive at 0x%06lx in bank 0\n",i+0x5000);
      ok=WriteRAM(scsi,buffer+i,0x5000+i,1,0,0);
   }

   for (i=0x03000;(i<0x7b000) && ok;i+=0x8000)
   {
      callback(step,"Sending firmware to drive at 0x%06lx in bank 0\n",i+0x5000);
      ok=WriteRAM(scsi,buffer+i,0x5000+i,8,0,0);
   }

   if (!ok)
      return FE_SEND_FLASH;

   checksum=(((u16) buffer[0x7affe]) << 8)+buffer[0x7afff];
   callback(step,"Sending checksum for bank 0 to drive\n");

   ok=SendChecksum(scsi,0,0,checksum,0);
   if (!ok)
      return FE_SEND_CHECKSUM;

   for (i=13;i<16 && ok;i++)
   {
      callback(step,"Erasing flash block %d in bank 0\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=EraseSingleBlock(scsi,0,i);
   }

   if (!ok)
      return FE_ERASE_BLOCK;

   for (i=0;i<8 && ok;i++)
   {
      callback(step,"Erasing flash block %d in bank 0\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=EraseSingleBlock(scsi,0,i);
   }
   if (!ok)
      return FE_ERASE_BLOCK;

   for (i=13;i<16 && ok;i++)
   {
      callback(step,"Writing flash block %d in bank 0\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=FlashSingleBlock(scsi,0,i);
   }
   if (!ok)
      return FE_FLASH_BLOCK;

   for (i=0;i<8 && ok;i++)
   {
      callback(step,"Writing flash block %d in bank 0\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=FlashSingleBlock(scsi,0,i);
   }
   if (!ok)
      return FE_FLASH_BLOCK;

   callback(step,"Leaving safe mode in bank 0\n");
   sm0=false;
   if (!SetSafeMode(scsi,drivetype,0,0))
      return FE_LEAVE_SAFEMODE;


//Flash bank 1
   callback(step,"Entering safe mode in bank 1\n");
   if (!SetSafeMode(scsi,drivetype,1,1))
      return FE_ENTER_SAFEMODE;
   sm1=true;

   for (i=0;(i<7) && ok;i++)
   {
      u32 target,source;
      u8 length;
      switch (i)
      {
      case 0:
         target=0x240000;source=0x7b004;length=2;break;
      case 1:
         target=0x242000;source=0x7d004;length=1;break;
      case 2:
         target=0x243000;source=0x7e004;length=1;break;
      case 3:
         target=0x244000;source=0x7f004;length=4;break;
      case 4:
         target=0x248000;source=0x83004;length=8;break;
      case 5:
         target=0x250000;source=0x8b004;length=8;break;
      case 6:
         target=0x258000;source=0x93004;length=8;break;
      default: // should never be reached
         target=0;source=0;length=0;break;
      }
      callback(step,"Sending firmware to drive at 0x%06lx in bank 1\n",target);
        ok&=WriteRAM(scsi,buffer+source,target,length,1,0);
   }

   if (!ok)
      return FE_SEND_FLASH;

   checksum=(((u16) buffer[0x9b002]) << 8)+buffer[0x9b003];
   callback(step,"Sending checksum for bank 1 to drive\n");

   ok=SendChecksum(scsi,1,0,checksum,0);
   if (!ok)
      return FE_SEND_CHECKSUM;

   callback(step,"Erasing flash in bank 1\n");

#ifdef _DEBUG
   if (!debug)
#endif
      ok=EraseBank(scsi,1);

   if (!ok)
      return FE_ERASE_BANK;

   for (i=8;i<15 && ok;i++)
   {
      callback(step,"Writing flash block %d in bank 1\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=FlashSingleBlock(scsi,1,i);
   }
   if (!ok)
        return FE_FLASH_BLOCK;

   callback(step,"Leaving safe mode in bank 1\n");
   sm1=false;
   if (!SetSafeMode(scsi,drivetype,1,0))
      return FE_LEAVE_SAFEMODE;

   return FE_OK;
}


/******************************************************************************\
| method: NECDRIVE::Flash768k                                                  |
|                                                                              |
| Purpose:                                                                     |
| Flash firmware on drives with 768k Flash (6x00, 6450)                        |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handle                                                          |
| buffer -> Pointer to firmware                                                |
| drivetype -> Exact drivetype to flash                                        |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
FlashingError NECDRIVE::Flash768K(Scsi* scsi, u8* buffer, DriveType drivetype)
{
   u32 i;
   u16 checksum;
   bool ok=true;

   callback(step,"Entering safe mode\n");
   if (!SetSafeMode(scsi,drivetype,0,1))
      return FE_ENTER_SAFEMODE;
   sm0=true;

   for (i=0;(i<0xb8000) && ok;i+=0x8000)
   {
      callback(step,"Sending firmware to drive at 0x%06lx\n",i+0x8000);
      ok=WriteRAM(scsi,buffer+i,0x8000+i,8,0,0);
   }

   if (!ok)
      return FE_SEND_FLASH;

   checksum=(((u16) buffer[0xb7ffe]) << 8)+buffer[0xb7fff];
   callback(step,"Sending checksum to drive\n");
   ok=SendChecksum(scsi,0,8,checksum,0);
   if (!ok)
      return FE_SEND_CHECKSUM;

   for (i=8;i<20 && ok;i++)
   {
      callback(step,"Erasing flash block %d\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=EraseSingleBlock(scsi,0,i);
   }
   if (!ok)
      return FE_ERASE_BLOCK;

   for (i=8;i<20 && ok;i++)
   {
      callback(step,"Writing flash block %d\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=FlashSingleBlock(scsi,0,i);
   }
   if (!ok)
      return FE_FLASH_BLOCK;

   callback(step,"Leaving safe mode\n");
   sm0=false;
   if (!SetSafeMode(scsi,drivetype,0,0))
      return FE_LEAVE_SAFEMODE;

   return FE_OK;
}


/******************************************************************************\
| method: NECDRIVE::Flash1M                                                    |
|                                                                              |
| Purpose:                                                                     |
| Flash firmware on drives with 1M Flash (2x00, 3500, 3520, 3530, 3540, 6650)  |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handle                                                          |
| buffer -> Pointer to firmware                                                |
| drivetype -> Exact drivetype to flash                                        |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
FlashingError NECDRIVE::Flash1M(Scsi* scsi, u8* buffer, DriveType drivetype)
{
   u32 i;
   u16 checksum;
   bool ok=true;

   callback(step,"Entering safe mode\n");
   if (!SetSafeMode(scsi,drivetype,0,1))
      return FE_ENTER_SAFEMODE;
   sm0=true;

   for (i=0;(i<0xf8000) && ok;i+=0x8000)
   {
      callback(step,"Sending firmware to drive at 0x%06lx\n",i+0x6000);
      ok=WriteRAM(scsi,buffer+i,0x6000+i,8,0,0);
   }

   for (i=0xf8000;(i<0xfa000) && ok;i+=0x2000)
   {
      callback(step,"Sending firmware to drive at 0x%06lx\n",i+0x6000);
      ok=WriteRAM(scsi,buffer+i,0x6000+i,2,0,0);
   }

   if (!ok)
      return FE_SEND_FLASH;

   if (drivetype==K2)
   {
      checksum=(((u16) buffer[0x29ffe]) << 8)+buffer[0x29fff];
      callback(step,"Sending checksum 1 to drive\n");
      ok=SendChecksum(scsi,0,1,checksum,0);
      if (!ok)
         return FE_SEND_CHECKSUM;

      checksum=(((u16) buffer[0xf9ffe]) << 8)+buffer[0xf9fff];
      callback(step,"Sending checksum 2 to drive\n");
      ok=SendChecksum(scsi,0,2,checksum,0);
      if (!ok)
         return FE_SEND_CHECKSUM;
   }
   else
   {
      checksum=(((u16) buffer[0xf9ffe]) << 8)+buffer[0xf9fff];
      callback(step,"Sending checksum to drive\n");
      ok=SendChecksum(scsi,0,8,checksum,0);
      if (!ok)
         return FE_SEND_CHECKSUM;
   }

   for (i=2;i<19 && ok;i++)
   {
      callback(step,"Erasing flash block %d\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=EraseSingleBlock(scsi,0,i);
   }
   if (!ok)
         return FE_ERASE_BLOCK;

   for (i=2;i<19 && ok;i++)
   {
      callback(step,"Writing flash block %d\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=FlashSingleBlock(scsi,0,i);
   }
   if (!ok)
         return FE_FLASH_BLOCK;

   callback(step,"Leaving safe mode\n");
   sm0=false;
   if (!SetSafeMode(scsi,drivetype,0,0))
      return FE_LEAVE_SAFEMODE;

   return FE_OK;
}

/******************************************************************************\
| method: NECDRIVE::Flash2M                                                    |
|                                                                              |
| Purpose:                                                                     |
| Flash firmware on drives with 2M Flash (3x50, 4x50, 7x50)                    |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handle                                                          |
| buffer -> Pointer to firmware                                                |
| drivetype -> Exact drivetype to flash                                        |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
FlashingError NECDRIVE::Flash2M(Scsi* scsi, u8* buffer, DriveType drivetype)
{
   u32 i;
   u16 checksum;
   bool ok=true;

   callback(step,"Entering safe mode\n");
   if (!SetSafeMode(scsi,drivetype,0,1))
      return FE_ENTER_SAFEMODE;
   sm0=true;

   for (i=0;(i<0x1e0000) && ok;i+=0x8000)
   {
      callback(step,"Sending firmware to drive at 0x%06lx\n",i+0x10000);
      ok=WriteRAM(scsi,buffer+i,0x10000+i,8,0,0);
   }

   if (!ok)
      return FE_SEND_FLASH;

   checksum=((buffer[0x1e0000] << 8)+(buffer[0x1e0001]));
   callback(step,"Sending checksum to drive\n");
   ok=SendChecksum(scsi,0,0,checksum,0);
   if (!ok)
      return FE_SEND_CHECKSUM;

   for (i=4;i<35 && ok;i++)
   {
      callback(step,"Erasing flash block %d\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=EraseSingleBlock(scsi,0,i);
   }
   if (!ok)
      return FE_ERASE_BLOCK;

   for (i=4;i<34 && ok;i++)
   {
      callback(step,"Writing flash block %d\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=FlashSingleBlock(scsi,0,i);
   }
   if (!ok)
      return FE_FLASH_BLOCK;

   callback(step,"Leaving safe mode\n");
   sm0=false;
   if (!SetSafeMode(scsi,drivetype,0,0))
      return FE_LEAVE_SAFEMODE;

   return FE_OK;
}


/******************************************************************************\
| method: NECDRIVE::Flash2MOpti                                                |
|                                                                              |
| Purpose:                                                                     |
| Flash firmware on Optiarc drives with 2M Flash (x17x)                        |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handle                                                          |
| buffer -> Pointer to firmware                                                |
| drivetype -> Exact drivetype to flash                                        |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
FlashingError NECDRIVE::Flash2MOpti(Scsi* scsi, u8* buffer, DriveType drivetype)
{
   u32 i;
   u16 checksum;
   bool ok=true;

   callback(step,"Entering safe mode\n");
   if (!SetSafeMode(scsi,drivetype,0,1))
      return FE_ENTER_SAFEMODE;
   sm0=true;

   for (i=0;(i<0x1c0000) && ok;i+=0x8000)
   {
      callback(step,"Sending firmware to drive at 0x%06lx\n",i+0x30000);
      ok=WriteRAMOpti(scsi,buffer+i,0x30000+i,8,0,0);
   }

   if (!ok)
      return FE_SEND_FLASH;

   checksum=((buffer[0x1c0000] << 8)+(buffer[0x1c0001]));
   callback(step,"Sending checksum %04x to drive\n", (int) checksum);
   ok=SendChecksum(scsi,0,0,checksum,0);
   if (!ok)
      return FE_SEND_CHECKSUM;

   for (i=3;i<31 && ok;i++)
   {
      callback(step,"Erasing flash block %d\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=EraseSingleBlock(scsi,0,i);
   }
   if (!ok)
      return FE_ERASE_BLOCK;

   for (i=3;i<31 && ok;i++)
   {
      callback(step,"Writing flash block %d\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=FlashSingleBlock(scsi,0,i);
   }
   if (!ok)
      return FE_FLASH_BLOCK;

   callback(step,"Leaving safe mode\n");
   sm0=false;
   if (!SetSafeMode(scsi,drivetype,0,0))
      return FE_LEAVE_SAFEMODE;

   return FE_OK;
}


/******************************************************************************\
| method: NECDRIVE::Flash2MOpti2                                               |
|                                                                              |
| Purpose:                                                                     |
| Flash firmware on Optiarc drives with 2M Flash new version (x20x drives)     |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handle                                                          |
| buffer -> Pointer to firmware                                                |
| drivetype -> Exact drivetype to flash                                        |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
FlashingError NECDRIVE::Flash2MOpti2(Scsi* scsi, u8* buffer, DriveType drivetype)
{
   u32 i;
   u16 checksum;
   bool ok=true;

   callback(step,"Entering safe mode\n");
   if (!SetSafeMode(scsi,drivetype,0,1))
      return FE_ENTER_SAFEMODE;
   sm0=true;

   for (i=0;(i<0x1c0000) && ok;i+=0x8000)
   {
      callback(step,"Sending firmware to drive at 0x%06lx\n",i+0x30000);
      ok=WriteRAM(scsi,buffer+i,0x30000+i,8,0,0);
   }

   if (!ok)
      return FE_SEND_FLASH;

   checksum=((buffer[0x1c0000] << 8)+(buffer[0x1c0001]));
   callback(step,"Sending checksum %04x to drive\n", (int) checksum);
   ok=SendChecksum(scsi,0,0,checksum,0);
   if (!ok)
      return FE_SEND_CHECKSUM;

   for (i=3;i<32 && ok;i++)
   {
      callback(step,"Erasing flash block %d\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=EraseBlock(scsi,false,(u8) i);
   }
   if (!ok)
      return FE_ERASE_BLOCK;

   for (i=3;i<31 && ok;i++)
   {
      callback(step,"Writing flash block %d\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=FlashBlock(scsi,false,(u8) i);
   }
   if (!ok)
      return FE_FLASH_BLOCK;

   callback(step,"Leaving safe mode\n");
   sm0=false;
   if (!SetSafeMode(scsi,drivetype,0,0))
      return FE_LEAVE_SAFEMODE;

   return FE_OK;
}


/******************************************************************************\
| method: NECDRIVE::Flash2MOpti3                                               |
|                                                                              |
| Purpose:                                                                     |
| Flash firmware on Optiarc drives with 2M Flash version 3 (x24x drives)       |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handle                                                          |
| buffer -> Pointer to firmware                                                |
| drivetype -> Exact drivetype to flash                                        |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
FlashingError NECDRIVE::Flash2MOpti3(Scsi* scsi, u8* buffer, DriveType drivetype)
{
   u32 i;
   u16 checksum;
   bool ok=true;

   callback(step,"Entering safe mode\n");
   if (!SetSafeMode(scsi,drivetype,0,1))
      return FE_ENTER_SAFEMODE;
   sm0=true;

   for (i=0;(i<0x1b0000) && ok;i+=0x8000)
   {
      callback(step,"Sending firmware to drive at 0x%06lx\n",i+0x30000);
      ok=WriteRAMOpti3(scsi,buffer+i,0x30000+i,0x8000,0,0);
   }

   if (!ok)
      return FE_SEND_FLASH;

   checksum=((buffer[0x1b0000] << 8)+(buffer[0x1b0001]));
   callback(step,"Sending checksum %04x to drive\n", (int) checksum);
   ok=SendChecksum(scsi,0,0,checksum,0);
   if (!ok)
      return FE_SEND_CHECKSUM;

   for (i=0x3;i<31 && ok;i++)
   {
      callback(step,"Erasing flash block %d\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=EraseRegion(scsi,false,i*0x10000,0x10000);
   }
   if (!ok)
      return FE_ERASE_BLOCK;

   for (i=3;i<30 && ok;i++)
   {
      callback(step,"Writing flash block %d\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=FlashRegion(scsi,false,i*0x10000,0x10000);
   }
   if (!ok)
      return FE_FLASH_BLOCK;

   callback(step,"Leaving safe mode\n");
   sm0=false;
   if (!SetSafeMode(scsi,drivetype,0,0))
      return FE_LEAVE_SAFEMODE;

   return FE_OK;
}


/******************************************************************************\
| method: NECDRIVE::Flash2MOpti4                                               |
|                                                                              |
| Purpose:                                                                     |
| Flash firmware on Optiarc drives with 2M Flash version 4 (x28x drives)       |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handle                                                          |
| buffer -> Pointer to firmware                                                |
| drivetype -> Exact drivetype to flash                                        |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
FlashingError NECDRIVE::Flash2MOpti4(Scsi* scsi, u8* buffer, DriveType drivetype)
{
   u32 i;
   u16 checksum;
   bool ok=true;

   callback(step,"Entering safe mode\n");
   if (!SetSafeMode(scsi,drivetype,0,1))
      return FE_ENTER_SAFEMODE;
   sm0=true;

   checksum=((buffer[0xe] << 8)+(buffer[0xf]));
   callback(step,"Sending checksum1 %04x to drive\n", (int) checksum);
   ok=SendChecksumOpti4(scsi,false,checksum,0x1b0000,5);
   if (!ok)
      return FE_SEND_CHECKSUM;

   for (i=0x30000;(i<0x1e0000) && ok;i+=0x10000)
   {
      callback(step,"Erasing flash at 0x%06lx\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=EraseRegion(scsi,true,i,0x10000);
   }
   if (!ok)
      return FE_ERASE_BLOCK;


   for (i=0;(i<0x1b0000) && ok;i+=0x10000)
   {
      callback(step,"Sending firmware to drive at 0x%06lx\n",i+0x30000);
      ok=WriteRAMOpti3(scsi,buffer+i,0x30000+i,0x8000,true,0,true);
      if (ok)
      {
         callback(step,"Sending firmware to drive at 0x%06lx\n",i+0x38000);
         ok=WriteRAMOpti3(scsi,buffer+i+0x8000,0x38000+i,0x8000,true,0,true);
      }
      if (ok)
      {
         callback(step,"Writing flash at 0x%06lx\n",0x30000+i);
#ifdef _DEBUG
         if (!debug)
#endif
            ok&=FlashRegion(scsi,true,0x30000+i,0x10000);
      }
   }

   if (!ok)
      return FE_SEND_FLASH;

   checksum=((buffer[0x1b0000] << 8)+(buffer[0x1b0001]));
   callback(step,"Sending checksum2 %04x to drive\n", (int) checksum);
   ok=SendChecksumOpti4(scsi,false,checksum,0x1b0000,4);
   if (!ok)
      return FE_SEND_CHECKSUM;

   callback(step,"Leaving safe mode\n");
   sm0=false;
   if (!SetSafeMode(scsi,drivetype,0,0))
      return FE_LEAVE_SAFEMODE;

   return FE_OK;
}


/******************************************************************************\
| method: NECDRIVE::Flash2MOptiBD                                              |
|                                                                              |
| Purpose:                                                                     |
| Flash firmware on Optiarc BluRay drives with 2M Flash                        |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handle                                                          |
| buffer -> Pointer to firmware                                                |
| drivetype -> Exact drivetype to flash                                        |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
FlashingError NECDRIVE::Flash2MOptiBD(Scsi* scsi, u8* buffer, DriveType drivetype)
{
   u32 i;
   u16 checksum;
   bool ok=true;

#if 1
   for (i=0,checksum=0;i<0x1d0200;i+=2)
      checksum+=(((u16) buffer[i])<<8)+buffer[i+1];

   if (checksum!=(((u16) buffer[0x1d0200])<<8)+buffer[0x1d0201])
      return FE_SEND_CHECKSUM;
#endif

   callback(step,"Entering safe mode\n");
   if (!SetSafeMode(scsi,drivetype,0,1))
      return FE_ENTER_SAFEMODE;
   sm0=true;

   for (i=0;(i<0x1d0200) && ok;i+=0x8000)
   {
      callback(step,"Sending firmware to drive at 0x%06lx\n",i);
      if (i!=0x1d0000)
         ok=WriteRAMOpti3(scsi,buffer+i,i,0x8000,0,0);
      else
         ok=WriteRAMOpti3(scsi,buffer+i,i,0x200,0,0);
   }

   if (!ok)
      return FE_SEND_FLASH;

   checksum=0;
   callback(step,"Sending checksum %04x to drive\n", (int) checksum);
   ok=SendChecksum(scsi,0,0,checksum,0,0);
   if (!ok)
      return FE_SEND_CHECKSUM;

   for (i=3;i<=31 && ok;i++)
   {
      callback(step,"Erasing flash block %d\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=EraseSingleBlock(scsi,0,i);
   }
   if (!ok)
      return FE_ERASE_BLOCK;

   for (i=3;i<=31 && ok;i++)
   {
      callback(step,"Writing flash block %d\n",i);
#ifdef _DEBUG
      if (!debug)
#endif
         ok&=FlashSingleBlock(scsi,0,i);
   }
   if (!ok)
      return FE_FLASH_BLOCK;

   callback(step,"Sending checksum %04x to drive\n", (int) checksum);
   ok=SendChecksum(scsi,0,0,checksum,0,1);

   callback(step,"Leaving safe mode\n");
   sm0=false;
   if (!SetSafeMode(scsi,drivetype,0,0))
      return FE_LEAVE_SAFEMODE;

   return FE_OK;
}


/******************************************************************************\
| method: NECDRIVE::FlashFirmware                                              |
|                                                                              |
| Purpose:                                                                     |
| Flash given firmware                                                         |
|                                                                              |
| Input:                                                                       |
| filename -> Name of firmware file                                            |
| device -> Name of the device to use                                          |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
ErrorCode NECDRIVE::FlashFirmware(char* filename,char* device)
{
   Scsi* scsi;
   DriveType FirmwareDrivetype,drivetype;
   Inquiry driveinfo;
   FlashingError rc;
   u16 bootcode,expectedbootcodefromfw;
   u8* buffer;
   u32 size;

   scsi=OpenDevice(device);
   if (!scsi)
      return E_SCSIOPEN;

   drivetype=SpecialChecks(scsi, IdentifyDrive(scsi));
   bootcode=RetrieveBootcodeVersion(scsi,drivetype);

   if (drivetype==DT_UNKNOWN)
      return E_UNSUPPORTEDDRIVE;

   if (drivetype==DT_INVALID)
      return E_NODRIVETYPE;

   buffer=ReadFirmware(filename,&size,drivetype,bootcode);
   if (!buffer)
      return E_ACCESSFIRMFILE;

   FirmwareDrivetype=GetDriveTypeFromBuffer(buffer,size,&expectedbootcodefromfw);

   callback(verbose,"Identified drive: %d - %04X\n",(int) drivetype,(int) bootcode);
   callback(verbose,"Detected drive from Firmware: %d - %04X\n\n",(int) FirmwareDrivetype,expectedbootcodefromfw);

   if (FirmwareDrivetype<0)
   {
      delete[] buffer;
      return E_NOFIRMWARE;
   }

   if (FirmwareDrivetype!=drivetype)
   {
      delete[] buffer;
      return E_FIRMNOMATCH;
   }

// Check for compatible notebook drive firmware. Not all files provide the bootcode version
   if (IsLaptopDrive(drivetype))
   {
      u8 driveID, fileID;
      u16 expectedbootcode;
      CompatibleLaptop LaptopDrive, LaptopFile;

      driveID=GetIDFromDrive(scsi, drivetype);
      fileID=GetIDFromBuffer(buffer, drivetype);
      LaptopDrive=GetCompatibleLaptop(driveID);
      LaptopFile=GetCompatibleLaptop(fileID);
      expectedbootcode=GetLaptopBootcode(LaptopFile);
      if (LaptopFile==L_INVALID)
      {
         delete[] buffer;
         return E_UNSUPPORTEDDRIVE;
      }

      if (LaptopDrive!=LaptopFile)
      {
         if (driveID!=0xff)
         {
            callback(error,"Probably mixing up master and slave firmware.\nThis is very likely to DAMAGE your drive or make it INACCESSIBLE!\nIt is not allowed to continue here.\n");
            callback(error,"ID from drive: %02x, ID from firmware: %02x\n",driveID,fileID);
            delete[] buffer;
            return E_FIRMNOMATCH;
         }
         if (bootcode!=expectedbootcode)
         {
            callback(error,"Your drive is in safe mode and the provided firmware does not match current IDE configuration. Either use a different firmware or change the bootcode\nFound 0x%04X - expected 0x%04X\n",(u32) bootcode, (u32) expectedbootcode);
            delete[] buffer;
            return E_FIRMNOMATCH;
         }

         if (!callback(yesno,"Your drive currently does not have a valid firmware.\nAlthough it should be safe to flash the provided firmware, there is a risk of breaking your drive.\nAre you really sure that you want to continue? If unsure say NO! (y/n)"))
         {
            delete[] buffer;
            return E_FIRMNOMATCH;
         }
      }
   }
   else if (expectedbootcodefromfw==0xffff) // Laptop check is safe enough and many laptop firmwares don't provide the bootcode
   {
      if (!callback(yesno,"The firmware file does not provide a bootcode version. While you are allowed to continue here, this firmware might not be intended for your drive.\nDo you really want to continue? If unsure say NO! (y/n)"))
      {
         delete[] buffer;
         return E_FIRMNOMATCH;
      }
   }
   else if (bootcode!=expectedbootcodefromfw)
   {
     callback(error,"The firmware file expects a different bootcode on your drive. You are not allowed to continue.\nDrive: 0x%04X - File: 0x%04X.",(u32) bootcode, (u32) expectedbootcodefromfw);
     delete[] buffer;
     return E_FIRMNOMATCH;
   }

   if (IdentifyFirmware(buffer,size,&driveinfo))
   {
      if (!callback(yesno,"You are about to flash your drive with the following firmware:\n\nVendor: %s\nIdentification: %s\nVersion: %s\n\nRemember no one can be held responsible for any kind of failure!\nAre you sure you want to proceed? (y/n)",
         driveinfo.vendor,driveinfo.identification,driveinfo.revision))
      {
         delete[] buffer;
         return E_CANCELLED;
      }
   }
   else
   {
      if (!callback(yesno,"You are about to flash your drive.\nRemember no one can be held responsible for any kind of failure!\nAre you sure you want to proceed? (y/n)"))
      {
         delete[] buffer;
         return E_CANCELLED;
      }
   }

   while (IsUnitReady(scsi))
   {
      if (!callback(okcancel,"There is a disc inside the drive or the drive is busy.\nPlease remove disc or wait until all activities have finished!"))
      {
         delete[] buffer;
         return E_OK;
      }
   }

   sm0=false; sm1=false;
   callback(numsteps,"%d",GetStepCount(drivetype,true));

   rc=FE_OK;
   switch (GetFlashSizeFromDriveType(drivetype))
   {
   case FlashSize640K: //NEC 1x00 and 5x00
      rc=Flash640K(scsi, buffer, drivetype);
      break;

   case FlashSize768K: // NEC 6x00
      rc=Flash768K(scsi, buffer, drivetype);
      break;

   case FlashSize1M: // NEC 2x00, 3x00, 3x20, 3x30, 3x40 and 6650
      rc=Flash1M(scsi, buffer, drivetype);
      break;

   case FlashSize2M: // NEC 3550, 3551, 4550, 4551, 7550, 7551, 3650, 3651, 3570, 3571, 4650, 4651, 4570, 4571
      rc=Flash2M(scsi, buffer, drivetype);
      break;

   case FlashSize2MOpti: // Optiarc 5170, 5173, 7170, 7173, 5140, 5143, 7140, 7143, 7630, 7633, 591*, 791*
      rc=Flash2MOpti(scsi, buffer, drivetype);
      break;

   case FlashSize2MOpti2: // Optiarc 520*, 720*, 559*, 759*
      rc=Flash2MOpti2(scsi, buffer, drivetype);
      break;

   case FlashSize2MOpti3: // Optiarc 524*, 724*
      rc=Flash2MOpti3(scsi, buffer, drivetype);
      break;

   case FlashSize2MOpti4: // Optiarc 528*, 728*
      rc=Flash2MOpti4(scsi, buffer, drivetype);
      break;

   case FlashSize2MOptiBD: // Optiarc 5500A/S, 5600A/S
      rc=Flash2MOptiBD(scsi, buffer, drivetype);
      break;

/*
   case FlashSize2MOptiBD2: // Optiarc 5730A/S
      rc=Flash2MOptiBD2(scsi, buffer, drivetype);
      break;
*/

/*
   case FlashSize2MHDDVD:
      rc=Flash2MHDDVD(scsi, buffer, drivetype);
      break;
*/

   default:
      rc=FE_UNSUPPORTED; // Unsupported drive
      break;
   }

   if (sm0)
      SetSafeMode(scsi,drivetype,0,0);
   if (sm1)
      SetSafeMode(scsi,drivetype,1,0);

   scsiClose(scsi);
   delete[] buffer;

   switch(rc)
   {
   case FE_ENTER_SAFEMODE:
      callback(error,"Error entering safe mode\n");
      break;
   case FE_SEND_FLASH:
      callback(error,"Error sending firmware to drive\n");
      break;
   case FE_SEND_CHECKSUM:
      callback(error,"Error submitting checksum");
      break;
   case FE_ERASE_BLOCK:
      callback(fatal,"Error erasing flash block");
      break;
   case FE_FLASH_BLOCK:
      callback(fatal,"Error writing flash block");
      break;
   case FE_ERASE_BANK:
      callback(fatal,"Error erasing flash");
      break;
   case FE_LEAVE_SAFEMODE:
      callback(fatal,"Error leaving safe mode\n");
      break;
   default:
      break; // For FE_OK and FE_UNSUPPORTED
   }
   if (rc==FE_UNSUPPORTED)
      return E_UNSUPPORTEDDRIVE;

   if (rc!=FE_OK)
      return E_FLASHERROR;

   return E_OK;
}


/******************************************************************************\
| method: NECDRIVE::DumpFirmware                                               |
|                                                                              |
| Purpose:                                                                     |
| Dump the firmware to a file                                                  |
|                                                                              |
| Input:                                                                       |
| filename -> Name of the file used for dumping the firmware                   |
| device -> Name of the device to use                                          |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
ErrorCode NECDRIVE::DumpFirmware(char* filename,char* device)
{
   FILE *firmfile;
   DriveType drivetype;
   u32 start,end;
   bool OK=false;
   Scsi* scsi;
   u16 chksum=0;
   u16 bootcode=0xffff;

   scsi=OpenDevice(device);
   if (!scsi)
      return E_SCSIOPEN;

// Check for valid drivetype.
   drivetype=IdentifyDrive(scsi);
   callback(talky,"Drivetype from identify: %d\n",(int) drivetype);

   drivetype=SpecialChecks(scsi, drivetype);
   callback(talky,"Drivetype SC: %d\n",(int) drivetype);

   if (drivetype==DT_UNKNOWN)
      return E_UNSUPPORTEDDRIVE;

   if (drivetype==DT_INVALID)
      return E_NODRIVETYPE;

// Open firmware file for writing
   firmfile=fopen(filename,"wb");
   if (!firmfile)
   {
      callback(error,"Error opening firmwarefile");
      return E_ACCESSFIRMFILE;
   }
   u8 id=GetIDFromDriveType(scsi,drivetype);
   if (id==0xff)
   {
      callback(error,"Dumping firmwares from this drive is currently not supported");
      fclose(firmfile);
      return E_DUMPNOSUPPORT;
   }

   callback(talky,"Using dump ID %02X\n",(int) id);

   callback(numsteps,"%d",GetStepCount(drivetype,false));

   callback(verbose,"Dumping firmware\n");

// Start routine for used drivetype
   switch (GetFlashSizeFromDriveType(drivetype))
   {
// NEC 1x00 and 5x00
   case FlashSize640K:
#if FULLDUMP
      start=0x0;
#else
      start=0x5000;
   #ifdef _DEBUG
      if (debug)
         start=0x0;
   #endif
#endif

      OK=FirmwareToFile(scsi,firmfile,start,0x80000,0,0);
      if (!OK)
         break;

#if ! FULLDUMP
   #ifdef _DEBUG
      if (!debug)
   #endif
         fprintf(firmfile,"%s",GetDriveID(drivetype));
#endif

      OK=FirmwareToFile(scsi,firmfile,0x240000,0x260000,1,0);
      break;


// NEC 6x00
   case FlashSize768K:
#if FULLDUMP
      start=0x0;
#else
      start=0x8000;
   #ifdef _DEBUG
      if (debug)
         start=0x0;
   #endif
#endif

      OK=FirmwareToFile(scsi,firmfile,start,0xc0000,0,0);
      break;


// NEC 2x00, 3x00, 3x20, 3x30, 3x40 and 6650
   case FlashSize1M:
#if FULLDUMP
      start=0x0;
#else
      start=0x6000;
   #ifdef _DEBUG
      if (debug)
      {
         if ((drivetype==K2) || (drivetype==K350))
            start=0x0;
         else
            start=0x4000;
      }
   #endif
#endif
      OK=FirmwareToFile(scsi,firmfile,start,0x100000,0,0);
      bootcode=RetrieveBootcodeVersion(scsi,drivetype);
      break;


// NEC 355*, 365*. 357*, 455*, 465*, 457*, 675*, 755*
   case FlashSize2M:
#if FULLDUMP
      start=0x0;
#else
      start=0x10000;
   #ifdef _DEBUG
      if (debug)
         start=0x4000;
   #endif
#endif
      OK=true;
      if (id)
      {
         callback(step,"Entering safe mode\n");
         if (!SetSafeMode(scsi,drivetype,0,1))
            OK=false;
      }
      if (OK)
      {
         OK=FirmwareToFile(scsi,firmfile,start,0x1f0000,0,&chksum,id);
         bootcode=RetrieveBootcodeVersion(scsi,drivetype);
#if ! FULLDUMP
         fputc((chksum>>8), firmfile);
         fputc((chksum&0xff), firmfile);
#endif
         if (id)
         {
            callback(step,"Leaving safe mode\n");
            if (!SetSafeMode(scsi,drivetype,0,0))
               OK=false;
         }
      }
      break;

// Optiarc 517*, 520*, 717*, 514*, 714*, 763*, 720*, 591*, 791*, 559*, 759*
   case FlashSize2MOpti: case FlashSize2MOpti2:
#if FULLDUMP
     start=0x0;
     end=0x200000;
#else
      start=0x30000;
      end=0x1f0000;
   #ifdef _DEBUG
      if (debug)
      {
         start=0x0;
         end=0x200000;
      }
   #endif
#endif
      OK=true;
      if (id)
      {
         callback(step,"Entering safe mode\n");
         if (!SetSafeMode(scsi,drivetype,0,1))
            OK=false;
      }
      if (OK)
      {
         OK=FirmwareToFile(scsi,firmfile,start,end,0,&chksum,id);
         bootcode=RetrieveBootcodeVersion(scsi,drivetype);
#if ! FULLDUMP
         fputc((chksum>>8), firmfile);
         fputc((chksum&0xff), firmfile);
#endif
         if (id)
         {
            callback(step,"Leaving safe mode\n");
            if (!SetSafeMode(scsi,drivetype,0,0))
               OK=false;
         }
      }
      break;

// Optiarc 524*, 723*, 724*, 726*, 728*, 770*
   case FlashSize2MOpti3:case FlashSize2MOpti4:
#if FULLDUMP
      start=0x0;
      end=0x200000;
#else
      start=0x30000;
      end=0x1e0000;
   #ifdef _DEBUG
      if (debug)
      {
         start=0x0;
         end=0x200000;
      }
   #endif
#endif
      OK=true;
      if (id)
      {
         callback(step,"Entering safe mode\n");
         if (!SetSafeMode(scsi,drivetype,0,1))
            OK=false;
      }
      if (OK)
      {
         OK=FirmwareToFile(scsi,firmfile,start,end,0,&chksum,id);
         bootcode=RetrieveBootcodeVersion(scsi,drivetype);
#if ! FULLDUMP
         fputc((chksum>>8), firmfile);
         fputc((chksum&0xff), firmfile);
#endif
         if (id)
         {
            callback(step,"Leaving safe mode\n");
            if (!SetSafeMode(scsi,drivetype,0,0))
               OK=false;
         }
      }
      break;

// Optiarc BC-5500A/S, BC-5600A/S
   case FlashSize2MOptiBD:
      OK=FirmwareToFile(scsi,firmfile,0x6000,0xb000,0,0,id); // currently only dump calibration area
      bootcode=RetrieveBootcodeVersion(scsi,drivetype);
      break;
   
// Optiarc BD-5730A/S
   case FlashSize2MOptiBD2:
      OK=FirmwareToFile(scsi,firmfile,0x6000,0xb000,0,0,id); // currently only dump calibration area
      bootcode=RetrieveBootcodeVersion(scsi,drivetype);
      break;

   case FlashSize2MHDDVD:
#if FULLDUMP
      start=0x0;
#else
      start=0x40000;
   #ifdef _DEBUG
      if (debug)
         start=0x0;
   #endif
#endif
      OK=FirmwareToFile(scsi,firmfile,start,0x200000,0,0,id);
      bootcode=RetrieveBootcodeVersion(scsi,drivetype);
      break;

   case FlashSizeInvalid:
      break; // Only avoid nasty warnings
   }


#if ! FULLDUMP
   if (
#ifdef _DEBUG
      !debug &&
#endif
      OK)
   {
         fprintf(firmfile,"%s",GetDriveID(drivetype));
         if ((bootcode!=0xffff) && (bootcode!=0xfffe))
            fprintf(firmfile,"%c%c",(char)(bootcode>>8),(char)bootcode);
   }
#endif

   fclose(firmfile);
   scsiClose(scsi);

   if (!OK)
   {
      callback(error,"Error reading firmware from drive\n");
      return E_DUMPERROR;
   }

   return E_OK;
}


/******************************************************************************\
| method: NECDRIVE::SetBooktype                                                |
|                                                                              |
| Purpose:                                                                     |
| Change the booktype settings                                                 |
|                                                                              |
| Input:                                                                       |
| device -> Name of the device to use                                          |
| mt -> Mediatype to change                                                    |
| bt -> Booktype to use                                                        |
| temp -> Change temporary or permanent settings                               |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
ErrorCode NECDRIVE::SetBooktype(char* device,Mediatype mt,Mediatype bt,bool temp)
{
   int drivetype;
   Scsi* scsi;
   ErrorCode rc=E_OK;

   scsi=OpenDevice(device);
   if (!scsi)
      return E_SCSIOPEN;

   drivetype=IdentifyDrive(scsi);
   if (drivetype==DT_INVALID)
      rc=E_NODRIVETYPE;
   else if (!SetBooktypeNEC(scsi,mt,bt,temp))
   {
      if (temp)
         rc=E_BOOKNOTSUPPORTED;
      else if (!SetBooktypeRicoh(scsi,mt,bt))
         rc=E_BOOKNOTSUPPORTED;
   }
   else
      SelectTempBooktypeNEC(scsi,mt,temp);

   scsiClose(scsi);

   return rc;
}

ErrorCode NECDRIVE::SetBooktype(char* device,char* mt,char* bt,char* temp)
{
   Mediatype btconv;
   Mediatype mtconv;
   bool tempconv;

   mtconv=GetMediatypeFromString(mt);
   if ((mtconv!=MT_PlusR) && (mtconv!=MT_PlusRW) && (mtconv!=MT_PlusR9))
      return E_BOOKINVALIDNAME;

   btconv=GetMediatypeFromString(bt);
   if (((int)btconv<0) || ((int)btconv>0xff))
      return E_BOOKINVALIDNAME;

   if (!stricmp(temp,"TEMP") || !stricmp(temp,"Temporary"))
      tempconv=true;
   else if (!stricmp(temp,"PERM") || !stricmp(temp,"Permanent"))
      tempconv=false;
   else
      return E_ILLEGAL_CALL;

   return SetBooktype(device,mtconv,btconv,tempconv);
}


/******************************************************************************\
| method: NECDRIVE::SetBooktypeRW                                              |
|                                                                              |
| Purpose:                                                                     |
| Change the booktype of the currently inserted +RW disc                       |
|                                                                              |
| Input:                                                                       |
| device -> Name of the device to use                                          |
| bt -> Booktype to use                                                        |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
ErrorCode NECDRIVE::SetBooktypeRW(char* device, Mediatype newbt)
{
   int drivetype;
   Scsi* scsi;
   Mediatype discbt;
   ErrorCode rc=E_OK;

   scsi=OpenDevice(device);
   if (!scsi)
      return E_SCSIOPEN;

   drivetype=IdentifyDrive(scsi);
   if (drivetype==DT_INVALID)
      rc=E_NODRIVETYPE;
   else
   {
      discbt=(Mediatype)GetDiscBooktype(scsi);

      if (discbt!=MT_PlusRW)
         rc=E_BOOKTYPEMEDIA;
      else if (!SetBooktypeRWRicoh(scsi,newbt))
         rc=E_BOOKNOTSUPPORTED;
   }

   scsiClose(scsi);

   return rc;
}

ErrorCode NECDRIVE::SetBooktypeRW(char* device, char* bt)
{
   Mediatype btconv;

   btconv=GetMediatypeFromString(bt);
   if (((int)btconv<0) || ((int)btconv>0xff))
      return E_BOOKINVALIDNAME;

   return SetBooktypeRW(device,btconv);
}


/******************************************************************************\
| method: NECDRIVE::ClearBTSettings                                            |
|                                                                              |
| Purpose:                                                                     |
| Clear booktype settings                                                      |
|                                                                              |
| Input:                                                                       |
| device -> Name of the device to use                                          |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
ErrorCode NECDRIVE::ClearBTSettings(char* device)
{
   int drivetype;
   Scsi* scsi;
   ErrorCode rc=E_OK;

   scsi=OpenDevice(device);
   if (!scsi)
      return E_SCSIOPEN;

   drivetype=IdentifyDrive(scsi);
   if (drivetype==DT_INVALID)
      rc=E_NODRIVETYPE;
   else if (!ClearBooktypeSettingsNEC(scsi))
      rc=E_BOOKNOTSUPPORTED;

   scsiClose(scsi);

   return rc;
}


/******************************************************************************\
| method: NECDRIVE::QueryBooktype                                              |
|                                                                              |
| Purpose:                                                                     |
| Query current booktype settings of drive and disc                            |
|                                                                              |
| Input:                                                                       |
| device -> Name of the device to use                                          |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
ErrorCode NECDRIVE::QueryBooktype(char* device)
{
   Mediatype disctype;
   Mediatype booktype;
   BookNECResult necsetting;
   Scsi* scsi;
   int chgleft=-1;
   int btversion=-1;

   scsi=OpenDevice(device);
   if (!scsi)
      return E_SCSIOPEN;

   disctype=(Mediatype)GetDisctype(scsi);
   if (disctype>=0)
   {
      callback(info,"The current disctype is %s\n",GetMediatypeAsString(disctype));
      if (disctype<=0xff)
      {
         booktype=(Mediatype)GetDiscBooktype(scsi);
         callback(info,"The current booktype is %s\n",GetMediatypeAsString(booktype));
      }
   }
   else
      callback(info,"Currently no disc is inserted\n");

   if (GetBooktypeSettingNEC(scsi,MT_PlusR9,false,&necsetting)) // supported for all NEC command drives
   {
      callback(info,"Permanent booktype for DVD+R-DL is %s\n",GetMediatypeAsString((Mediatype)necsetting.booktype));
      chgleft=necsetting.changesleft;
      btversion=necsetting.version;

      if (btversion>=2) // Version 2 supports +R9 temp settings
      {
        if (GetBooktypeSettingNEC(scsi,MT_PlusR9,true,&necsetting))
        {
          callback(info,"Temporary booktype for DVD+R-DL is %s\n",GetMediatypeAsString((Mediatype)necsetting.booktype));
          if (!necsetting.usetemp)
            callback(info,"The permanent DVD+R-DL settings are currently active\n");
          else
            callback(info,"The temporary DVD+R-DL settings are currently active\n");
        }
      }

      if (btversion>=3) // Version 3 supports +R perm and temp settings
      {
        if (GetBooktypeSettingNEC(scsi,MT_PlusR,false,&necsetting))
        {
          callback(info,"Permanent booktype for DVD+R is %s\n",GetMediatypeAsString((Mediatype)necsetting.booktype));
        }
        if (GetBooktypeSettingNEC(scsi,MT_PlusR,true,&necsetting))
        {
          callback(info,"Temporary booktype for DVD+R is %s\n",GetMediatypeAsString((Mediatype)necsetting.booktype));
          if (!necsetting.usetemp)
            callback(info,"The permanent DVD+R settings are currently active\n");
          else
            callback(info,"The temporary DVD+R settings are currently active\n");
        }
      }

      if (btversion>=4) // Version 3 supports +R perm and temp settings
      {
        if (GetBooktypeSettingNEC(scsi,MT_PlusRW,false,&necsetting))
        {
          callback(info,"Permanent booktype for DVD+RW is %s\n",GetMediatypeAsString((Mediatype)necsetting.booktype));
        }
        if (GetBooktypeSettingNEC(scsi,MT_PlusRW,true,&necsetting))
        {
          callback(info,"Temporary booktype for DVD+RW is %s\n",GetMediatypeAsString((Mediatype)necsetting.booktype));
          if (!necsetting.usetemp)
            callback(info,"The permanent DVD+RW settings are currently active\n");
          else
            callback(info,"The temporary DVD+RW settings are currently active\n");
        }
      }

      callback(info,"You have %d changes left for the permanent booktype settings\n",chgleft);
      callback(verbose,"Your drive uses NEC bitsetting version %d\n",btversion);
   }
   else
   {
      booktype=(Mediatype)GetBooktypeSettingRicoh(scsi,MT_PlusR9);
      if (booktype>=0)
         callback(info,"Permanent booktype for DVD+R-DL is %s\n",GetMediatypeAsString((Mediatype)booktype));
      else
         callback(info,"The permanent booktype on DVD+R-DL cannot be changed\n");

      booktype=(Mediatype)GetBooktypeSettingRicoh(scsi,MT_PlusR);
      if (booktype>=0)
         callback(info,"Permanent booktype for DVD+R is %s\n",GetMediatypeAsString((Mediatype)booktype));
      else
         callback(info,"The permanent booktype on DVD+R either has not been set or cannot be changed\n");

      booktype=(Mediatype)GetBooktypeSettingRicoh(scsi,MT_PlusRW);
      if (booktype>=0)
         callback(info,"Permanent booktype for DVD+RW is %s\n",GetMediatypeAsString((Mediatype)booktype));
      else
         callback(info,"The permanent booktype on DVD+RW either has not been set or cannot be changed\n");

      callback(verbose,"The drive uses Ricoh commands to change the booktype\n");
   }

   scsiClose(scsi);

   return E_OK;
}

/******************************************************************************\
| method: NECDRIVE::QuerySingleBooktype                                        |
|                                                                              |
| Purpose:                                                                     |
| Query specific booktype setting of drive                                     |
|                                                                              |
| Input:                                                                       |
| device -> Name of the device to use                                          |
| type -> Select which mediatype to retrieve                                   |
| temp -> Determine whether to retrieve temporary or permanent settings        |
| result -> Result structure                                                   |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
ErrorCode NECDRIVE::QuerySingleBooktype(char* device,Mediatype type,bool temp,BooktypeInfo* result)
{
   Scsi* scsi;
   BookNECResult necsetting;
   Mediatype Booktype = MT_error;

   result->booktype = MT_error;
   result->mediatype = type;
   result->changesleft = 0xffff;
   result->usetemp = false;
   result->ricoh = false;
   result->version = 0;

   scsi=OpenDevice(device);
   if (!scsi)
      return E_SCSIOPEN;

   if (GetBooktypeSettingNEC(scsi,type,temp,&necsetting))
   {
      Booktype = (Mediatype) necsetting.booktype;
      result->mediatype = type;
      result->changesleft = necsetting.changesleft;
      result->usetemp = necsetting.usetemp;
      result->ricoh = false;
      result->version = necsetting.version;
   }
   else if (temp == false)
   {
      Booktype = (Mediatype) GetBooktypeSettingRicoh(scsi,type);
      result->ricoh = true;
   }

   if (Booktype != MT_error)
   {
      switch (Booktype & 0xfff0)
      {
      case (MT_PlusR & 0xfff0):
         result->booktype = MT_PlusR;
         break;
      case (MT_MinusR & 0xfff0):
         result->booktype = MT_MinusR;
         break;
      case (MT_PlusRW & 0xfff0):
         result->booktype = MT_PlusRW;
         break;
      case (MT_MinusRW & 0xfff0):
         result->booktype = MT_MinusRW;
         break;
      case (MT_PlusR9 & 0xfff0):
         result->booktype = MT_PlusR9;
         break;
      case (MT_PlusRW9 & 0xfff0):
         result->booktype = MT_PlusRW9;
         break;
      case (MT_ROM & 0xfff0):
         result->booktype = MT_ROM;
         break;
      case (MT_RAM & 0xfff0):
         result->booktype = MT_RAM;
         break;
      }
   }

   scsiClose(scsi);

   return E_OK;
}


/******************************************************************************\
| method: NECDRIVE::IdentifyNewDrive                                           |
|                                                                              |
| Purpose:                                                                     |
| Try to retrieve information about unsupported drives                         |
|                                                                              |
| Input:                                                                       |
| device -> Name of the device to use                                          |
| filename -> Name of a file which will retrieve the data                      |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
ErrorCode NECDRIVE::IdentifyNewDrive(char* device, char* filename)
{
   Scsi* scsi;
   FILE* outfile;
   Inquiry driveinfonorm,driveinfoboot;
   char bootkey[8];
   u16 method=0;
   u8 buffer[4];
   u8 readid[32];
   u8 xflashID[48];
   u32 last=0;
   u16 outbuffer[4108];
   bool success;

   scsi=OpenDevice(device);
   if (!scsi)
      return E_SCSIOPEN;

// Make sure it's a "CDROM" device
   if (!GetInquiry(scsi, &driveinfonorm))
      return E_NODRIVETYPE;
   if (driveinfonorm.devicetype!=0x05)
      return E_NODRIVETYPE;

// Initialize some data
   memset(readid,0,32);
   memset(xflashID,0,48);
   for (u32 i=0;i<(sizeof(outbuffer)/sizeof(*outbuffer));i++)
      outbuffer[i]=0x100;
   
// Read key at 0x4800
   success=ReadRAM(scsi,buffer,0x4800,4,false);
   if (!success)
   {
// Try 0x10800 instead
      success=ReadRAM(scsi,buffer,0x10800,4,false);
      if (success)
         method=1;
   }
   if (!success)
   {
// Try 0x7000 instead
      success=ReadRAM(scsi,buffer,0x7000,4,false);
      if (success)
         method=2;
   }
   for (int i=0;(i<4) && success;i++)
      outbuffer[i]=buffer[i];

   if (success)
   {
      if (method==2)
      {
         success=ReadRAM(scsi,xflashID,0x8020,48,false);
         if (!xflashID[0])
         {
            success=ReadRAM(scsi,xflashID,0x8b20,48,false);
            method=3;
         }
      }
      else if (method==1)
         success=ReadRAM(scsi,xflashID,0x20f20,48,false);
      else
         success=ReadRAM(scsi,xflashID,0x5f20,48,false);
   }

// Find bootkey
   const char alphabet[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   bool found=false;
   for (char letter='A';(letter<='Z') && !found;letter++)
   {
      for (int c1=0;(c1<10) && !found;c1++) // 0-9
      {
         for (int c2=0;(c2<10) && !found;c2++) // 0-9
         {
            for (int c3=0;(c3<18) && !found;c3++) // 0-9, A-H
            {
               sprintf(bootkey,"%c%c%c%c",letter,alphabet[c1],alphabet[c2],alphabet[c3]);
               if (SetSafeMode(scsi,bootkey,0,1))
               {
                  found=true;
               }
               if (found)
               {
                  GetInquiry(scsi, &driveinfoboot); // Retrieve bootcode version information
                  for (int i=0;i<256;i++)
                     if (ReadRAM(scsi,buffer,0x30000,4,false,(u8)i))
                        readid[i/8]|=(1<<(i&7));

                  SetSafeMode(scsi,bootkey,0,0);

                  for (int i=0;i<4;i++)
                     outbuffer[i+4]=bootkey[i];
                  for (int i=0;i<4;i++)
                     outbuffer[i+8]=driveinfoboot.revision[i];
               }
            }
         }
      }
   }

// Read 1 Byte at start of every page (0x000000 to 0xfff000)
   for (u32 i=0;i<0x1000;i++)
   {
      if (ReadRAM(scsi,buffer,(i*0x1000),1,false))
      {
         outbuffer[i+12]=buffer[0];
         last=i+1;
      }
   }
   scsiClose(scsi);

   outfile=fopen(filename,"w");
   if (!outfile)
      return E_ACCESSFIRMFILE;

   fprintf(outfile,"Please provide the following information at http://club.myce.com/showthread.php?t=105676\n");
   fprintf(outfile,"--- Start of identification ---\n");
   fprintf(outfile,"Version: %s\n",NECVersion);
   fprintf(outfile,"Normal : %s - %s - %s\n",driveinfonorm.vendor, driveinfonorm.identification,driveinfonorm.revision);
   if (found)
      fprintf(outfile,"Boot   : %s - %s - %04X\n",driveinfoboot.vendor, driveinfoboot.identification,(int) method);
   else
      fprintf(outfile,"Could not detect boot ID\n");

   u32 i;

   if (success)
   {
      fputc('|',outfile);
      for (i=0;i<48;i++)
      {
         if ((xflashID[i]>=0x20) && (xflashID[i]<0x7f))
            fprintf(outfile,"%c", (int) xflashID[i]);
         else
            fprintf(outfile,"*");

      }
      fputc('|',outfile);
      fputc('\n',outfile);
   }
   
   for (i=0;i<32;i++)
      fprintf(outfile,"%02X", (int) readid[i]);
   fputc('\n',outfile);
   for (i=0;i<12+last;i++)
   {
      if (outbuffer[i]<0x100)
         fprintf(outfile,"%02X", (int) outbuffer[i]);
      else
         fprintf(outfile,"XX");
      if ((i&0x1f) == 0x1f)
         fputc('\n',outfile);
   }
   if (i&0x1f)
      fputc('\n',outfile);
   fprintf(outfile,"--- End of identification ---\n");
   fclose(outfile);

   return E_OK;
}


/******************************************************************************\
| method: NECDRIVE::DoRegionReset                                              |
|                                                                              |
| Purpose:                                                                     |
| Perform the actual region code reset.                                        |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| global -> Also reset vendor counter if set, else only perform vendor reset   |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
ErrorCode NECDRIVE::DoRegionReset(Scsi* scsi,bool global)
{
   DriveType drivetype;
   u8 resetkey[5]; // 4 Bytes key + trailing \0 for callback
   bool dbg=false;

#ifdef _DEBUG
   if (debug)
      dbg=true; // RPC2 global reset debug
#endif


// Check for valid drivetype.
   drivetype=IdentifyDrive(scsi);
   drivetype=SpecialChecks(scsi, drivetype);

   if (drivetype==DT_INVALID)
      return E_NODRIVETYPE;
   
   if (!callback(yesno,
      "Resetting the region counter too often increases\n"
      "the risk of damaging your drive. It is recommended\n"
      "to use RPC1 or Auto-Reset firmwares instead.\n\n"
      "Do you still want to continue?"))
      return E_CANCELLED;
   
   if (!global)
   {
      callback(talky,"Trying to issue vendor reset\n");
      if (RPCVendorReset(scsi))
         return E_OK;
      else
         return E_UNSUPPORTEDDRIVE;
   }


   callback(talky,"First attempt: %d\n",(int) drivetype);
   if (RPCGlobalReset(scsi,drivetype,dbg)) // will fail on 3450 and 3500 drives
      return E_OK;

   if (drivetype==K350)
   {
      callback(talky,"Attempting K350\n");
      if (RPCGlobalReset(scsi,(u8*) "K350",dbg)) //alternative call for different 3450 and 3500 drives
         return E_OK;
   }


// Find resetkey (Brute Force)
   callback(talky,"Trying brute force\n");

   const char alphabet[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   for (char letter='A';letter<='Z';letter++)
   {
      for (int c1=0;c1<10;c1++) // 0-9
      {
         for (int c2=0;c2<10;c2++) // 0-9
         {
            for (int c3=0;c3<18;c3++) // 0-9, A-H
            {
               resetkey[0]=letter;
               resetkey[1]=alphabet[c1];
               resetkey[2]=alphabet[c2];
               resetkey[3]=alphabet[c3];
               if (RPCGlobalReset(scsi,resetkey,dbg))
               {
                  resetkey[4]=0;
                  callback(talky,"Succeeded with %s",resetkey);
                  return E_OK;
               }
            }
         }
      }
   }

   return E_UNSUPPORTEDDRIVE;
}


/******************************************************************************\
| method: NECDRIVE::ResetRegion                                                |
|                                                                              |
| Purpose:                                                                     |
| Reset region code counter. Mainly do checks, open SCSI and call main reset   |
|                                                                              |
| Input:                                                                       |
| device -> Name of the device to use                                          |
| global -> Also reset vendor counter if set, else only perform vendor reset   |
|                                                                              |
| Returnvalues:                                                                |
| ErrorCode                                                                    |
\******************************************************************************/
ErrorCode NECDRIVE::ResetRegion(char* device,char* mode,bool force)
{
   RegionInfo rpc;
   Scsi* scsi;
   ErrorCode rc;
   bool global;

   if (!stricmp(mode,"vendor"))
      global=false;
   else if (!stricmp(mode,"global"))
      global=true;
   else
      return E_ILLEGAL_CALL;

   rc=GetRegion(device,&rpc);
   if (rc!=E_OK)
      return rc;

   callback(verbose,"Current settings: RPC%d - V:%d - U:%d\n",rpc.RPC2?2:1,rpc.VendorChanges,rpc.UserChanges);
   if (!force) // Issue commands even if not required
   {
      if (!rpc.RPC2)
         return E_RPC1;

      if ((rpc.VendorChanges==4) && (rpc.UserChanges==5))
         return E_RPCNOTNEEDED;

      if ((rpc.UserChanges==5) && !global)
         return E_RPCNOTNEEDED;

      if (!rpc.VendorChanges && !global)
         return E_RPCNOVENDOR;
   }

   scsi=OpenDevice(device);
   if (!scsi)
      return E_SCSIOPEN;

   rc=DoRegionReset(scsi,global);

   scsiClose(scsi);

   return rc;
}
