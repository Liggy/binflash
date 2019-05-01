/******************************************************************************\
| Revision history:                                                            |
|                                                                              |
| 0.90 - 20040828 - First public beta version                                  |
| 0.91 - 20040829 - Code cleanup                                               |
| 0.92 - 20040830 - Match versions                                             |
| 0.95 - 20040916 - Add bitsetting functions (no 0.93 and 0.94 for syncing)    |
| 1.00 - 20041013 - No change. Release version 1.0                             |
| 1.01 - 20041030 - Fix CDB-Size for Get Configuration when bitsetting +RWs    |
| 1.02 - 20041119 - Add SetSafeMode with string instead of int drivetype       |
| 1.10 - 20041225 - Added support for NEC ND3520                               |
| 1.13 - 20050312 - Changed scsiSay checks to <0                               |
|                 - Fixed buffer size for getting booktype command version NEC |
| 1.14 - 20050327 - Added support for NEC ND3530                               |
| 1.16 - 20050412 - Added support for NEC ND3540                               |
| 1.17 - 20050524 - Added support for NEC ND6650                               |
| 1.20 - 20051003 - Added support for NEC ND3550/ND4550                        |
|        20051003 - Prepare bitsetting of DVD+RW9                              |
|        20051004 - Add ReadBoot Function                                      |
| 1.21 - 20051009 - Move GetDriveType from interface to internal               |
|        20051016 - Add support for ND3550 drives                              |
| 1.23 - 20051209 - Add support for ND7550 drives                              |
| 1.24 - 20060105 - Add support for ND3551 drives                              |
| 1.26 - 20060218 - Add support for ND7551 drives                              |
|        20060227 - Add support for ND6750, ND4570 and ND4571 drives           |
|        20060312 - Change notebook drive master and slave detection           |
| 1.27 - 20060326 - Improve notebook drive master and slave detection again    |
| 1.28 - 20060422 - Fixed master/slave recognition on 6650 drives              |
| 1.29 - 20060525 - Add support for NDx56x and ND357x drives                   |
| 1.30 - 20060531 - Added dump ID for 3570 to (commented) GetIDFromDriveType   |
|        20060625 - Corrected ID for 3650/3651 drives                          |
| 1.31 - 20060628 - Added GetFlashSizeFromDriveType                            |
|        20060703 - Removed unnecessary buffer from FlashBoot                  |
| 1.32 - 20060828 - Add support for Optiarc x17x drives                        |
|        20060915 - Added support for D355 burners. (special 3550 version)     |
| 1.33 - 20061004 - Fix dumping x57x firmwares                                 |
| 1.34 - 20061006 - Correct data for 5540 (master) firmwares                   |
| 1.35 - 20061126 - Correct data for 7543 (master) firmwares                   |
|        20061130 - Add function SpecialChecks for uncommon drives/firmwares   |
|        20061203 - Correct data for 7540 (master) firmwares                   |
| 1.36 - 20061214 - Add support for 7170S (SATA) drives                        |
| 1.37 - 20061231 - Hopefully fix wrong drive ID message when flashing         |
|        20061231 - Add support for "strange" 3650/4650 firmwares              |
| 1.38 - 20070310 - Add support for 7173S (SATA) drives                        |
| 1.39 - 20070404 - Add support for HR-1100A HD-DVD drives                     |
| 1.40 - 20070813 - Add support for 5170S (SATA) drives                        |
| 1.42 - 20071016 - Add support for AD-5630A/AD-5633A drives                   |
| 1.43 - 20071207 - Add support for AD-5200A, 5203A, 5200S, 5203S, 7200A,      |
|                   7203A, 7200S and 7203S drives                              |
| 1.44 - 20080212 - Add support for AD-5201A, 5201S, 7201A, 7201A, 7590A and   |
|                   7593A drives                                               |
| 1.45 - 20080212 - Add support for AD-5170A duplicator drives                 |
|        20080609 - Add support for AD-x91xA drives                            |
| 1.46 - 20081218 - Add support for AD-x91xS and AD-x59xS drives               |
| 1.47p- 20090401 - Start adding support for AD-7240S                          |
| 1.47 - 20090425 - Finalize support for AD-7240S                              |
| 1.48 - 20090907 - Add support for AD-5670S and AD-5960S                      |
| 1.49 - 20091214 - Add support for AD-7700S series and LiteOn 7240S OEMs      |
| 1.49b- 20091218 - Fix LiteOn OEM support                                     |
| 1.50 - 20100119 - Add support for AD-7230S and AD-7260S series               |
|        20100119 - No change from DT_UNKNOWN to DT_INVALID in SpecialChecks   |
| 1.51 - 20100823 - Add support for AD-5680H drive                             |
| 1.52 - 20100907 - Add support for Blu-Ray drives                             |
|                 - Add support for AD-7700H                                   |
| 1.53 - 20101224 - Try adding support for 5701/3H and 7701/3H                 |
| 1.54 - 20110123 - Add support for AD-x591x drives                            |
| 1.56 - 20110318 - Add region code reset                                      |
|                 - Add support for AD-5970H series                            |
| 1.57 - 20110615 - Add debug option to global RPC region reset                |
|        20110723 - Add support for AD-x28xS series                            |
|        20110809 - Add support for BC-5640H drive. No dump or flash possible  |
| 1.58 - 20110830 - Add BD-5740H - No dump or flash possible                   |
|        20111121 - Add AD-x69xH drives                                        |
|        20111121 - Erase/FlashRegion functions added. Adjustments             |
| 1.59 - 20111206 - Add AD-x71xH drives                                        |
| 1.60 - 20111222 - Add G305 drivetype                                         |
| 1.61 - 20120110 - Add x25x, x27x, x29x, x80x drives                          |
| 1.62 - 20120204 - Add 774x drives                                            |
| 1.63 - 20130103 - Add SendChecksumOpti4/calcchecksum in WriteRAMOpti3        |
| 1.64 - 20130519 - Add 7717 and x79x drives                                   |
\******************************************************************************/

#include <string.h>
#include "plscsi/plscsi.h"

#include "types.h"
#include "necinternal.h"


// Command data block
static u8 cdb[CDB_SIZE];


static const char* InternalVersion="1.64";


/******************************************************************************\
| function: GetInternalVersion                                                 |
|                                                                              |
| Purpose:                                                                     |
| Supply version of internal access functions                                  |
|                                                                              |
| Input:                                                                       |
| None                                                                         |
|                                                                              |
| Returnvalues:                                                                |
| Interface version                                                            |
\******************************************************************************/
const char* GetInternalVersion()
{
   return InternalVersion;
}


/******************************************************************************\
| function: GetDriveID                                                         |
|                                                                              |
| Purpose:                                                                     |
| Retrieve ID for safe mode and firmware file identifier                       |
|                                                                              |
| Input:                                                                       |
| drive -> DriveType                                                           |
|                                                                              |
| Returnvalues:                                                                |
| Drive ID                                                                     |
\******************************************************************************/
const char* GetDriveID(DriveType drive)
{
   switch (drive)
   {
   case K1:
      return "K110";
   case L1:
      return "L110";
   case K2:
      return "K210";
   case L2:
      return "L210";
   case K350:
      return "K310";
   case K352:
      return "K352";
   case K353:
      return "K353";
   case K354:
      return "K354";
   case L251:
      return "L251";
   case K355:
      return "K355";
   case D355:
      return "D355";
   case K356:
      return "K356";
   case K450:
      return "K450";
   case K451:
      return "K451";
   case L252:
      return "L252";
   case L253:
      return "L253";
   case L350:
      return "L350";
   case L351:
      return "L351";
   case K365:
      return "K365";
   case K366:
      return "K366";
   case K370:
      return "K370";
   case K371:
      return "K371";
   case K460:
      return "K460";
   case K461:
      return "K461";
   case K470:
      return "K470";
   case K471:
      return "K471";
   case D570:
      return "D570";
   case D57D:
      return "D57D";
   case D573:
      return "D573";
   case D770:
      return "D770";
   case D773:
      return "D773";
   case L260:
      return "L260";
   case L261:
      return "L261";
   case L450:
      return "L450";
   case L451:
      return "L451";
   case Q175:
      return "Q175";
   case Q178:
      return "Q178";
   case G175:
      return "G175";
   case G178:
      return "G178";
   case Q630:
      return "Q630";
   case Q633:
      return "Q633";
   case G630:
      return "G630";
   case G633:
      return "G633";
   case Q200:
      return "Q200";
   case Q201:
      return "Q201";
   case Q203:
      return "Q203";
   case Q205:
      return "Q205";
   case Q206:
      return "Q206";
   case Q208:
      return "Q208";
   case G200:
      return "G200";
   case G201:
      return "G201";
   case G203:
      return "G203";
   case G205:
      return "G205";
   case G206:
      return "G206";
   case G208:
      return "G208";
   case Q230:
      return "Q230";
   case Q231:
      return "Q231";
   case Q233:
      return "Q233";
   case Q235:
      return "Q235";
   case Q236:
      return "Q236";
   case Q238:
      return "Q238";
   case G230:
      return "G230";
   case G231:
      return "G231";
   case G233:
      return "G233";
   case G235:
      return "G235";
   case G236:
      return "G236";
   case G238:
      return "G238";
   case Q240:
      return "Q240";
   case Q241:
      return "Q241";
   case Q243:
      return "Q243";
   case Q245:
      return "Q245";
   case Q246:
      return "Q246";
   case Q248:
      return "Q248";
   case G240:
      return "G240";
   case G241:
      return "G241";
   case G243:
      return "G243";
   case G245:
      return "G245";
   case G246:
      return "G246";
   case G248:
      return "G248";
   case G25A:
      return "G25A";
   case G25B:
      return "G25B";
   case G25D:
      return "G25D";
   case Q260:
      return "Q260";
   case Q261:
      return "Q261";
   case Q263:
      return "Q263";
   case Q265:
      return "Q265";
   case Q266:
      return "Q266";
   case Q268:
      return "Q268";
   case G260:
      return "G260";
   case G261:
      return "G261";
   case G263:
      return "G263";
   case G265:
      return "G265";
   case G266:
      return "G266";
   case G268:
      return "G268";
   case M110:
      return "M110";
   case Q590:
      return "Q590";
   case Q591:
      return "Q591";
   case Q593:
      return "Q593";
   case Q595:
      return "Q595";
   case Q596:
      return "Q596";
   case Q598:
      return "Q598";
   case G590:
      return "G590";
   case G591:
      return "G591";
   case G593:
      return "G593";
   case G595:
      return "G595";
   case G596:
      return "G596";
   case G598:
      return "G598";
   case Q670:
      return "Q670";
   case Q673:
      return "Q673";
   case Q675:
      return "Q675";
   case Q678:
      return "Q678";
   case G670:
      return "G670";
   case G673:
      return "G673";
   case G675:
      return "G675";
   case G678:
      return "G678";
   case Q910:
      return "Q910";
   case Q913:
      return "Q913";
   case Q915:
      return "Q915";
   case Q918:
      return "Q918";
   case G910:
      return "G910";
   case G913:
      return "G913";
   case G915:
      return "G915";
   case G918:
      return "G918";
   case Q960:
      return "Q960";
   case Q963:
      return "Q963";
   case Q965:
      return "Q965";
   case Q968:
      return "Q968";
   case G960:
      return "G960";
   case G963:
      return "G963";
   case G965:
      return "G965";
   case G968:
      return "G968";
   case Q700:
      return "Q700";
   case Q701:
      return "Q701";
   case Q703:
      return "Q703";
   case Q705:
      return "Q705";
   case Q706:
      return "Q706";
   case Q708:
      return "Q708";
   case Q70A:
      return "Q70A";
   case Q70B:
      return "Q70B";
   case Q70D:
      return "Q70D";
   case G700:
      return "G700";
   case G701:
      return "G701";
   case G703:
      return "G703";
   case G705:
      return "G705";
   case G706:
      return "G706";
   case G708:
      return "G708";
   case G70A:
      return "G70A";
   case G70B:
      return "G70B";
   case G70D:
      return "G70D";
   case Q68A:
      return "Q68A";
   case Q68B:
      return "Q68B";
   case Q68D:
      return "Q68D";
   case Q97A:
      return "Q97A";
   case Q97B:
      return "Q97B";
   case Q97D:
      return "Q97D";
   case G97A:
      return "G97A";
   case G97B:
      return "G97B";
   case G97D:
      return "G97D";
   case G27A:
      return "G27A";
   case G27B:
      return "G27B";
   case G27D:
      return "G27D";
   case Q280:
      return "Q280";
   case Q281:
      return "Q281";
   case Q283:
      return "Q283";
   case Q285:
      return "Q285";
   case Q286:
      return "Q286";
   case Q288:
      return "Q288";
   case G280:
      return "G280";
   case G281:
      return "G281";
   case G283:
      return "G283";
   case G285:
      return "G285";
   case G286:
      return "G286";
   case G288:
      return "G288";
   case G29A:
      return "G29A";
   case G29B:
      return "G29B";
   case G29D:
      return "G29D";
   case Q300:
      return "Q380";
   case Q301:
      return "Q301";
   case Q303:
      return "Q303";
   case Q305:
      return "Q305";
   case Q306:
      return "Q306";
   case Q308:
      return "Q308";
   case G300:
      return "G300";
   case G301:
      return "G301";
   case G303:
      return "G303";
   case G305:
      return "G305";
   case G306:
      return "G306";
   case G308:
      return "G308";
   case Q325:
      return "Q325";
   case Q326:
      return "Q326";
   case Q328:
      return "Q328";
   case G325:
      return "G325";
   case G326:
      return "G326";
   case G328:
      return "G328";
   case Q69A:
      return "Q69A";
   case Q69B:
      return "Q69B";
   case Q69D:
      return "Q69D";
   case G69A:
      return "G69A";
   case G69B:
      return "G69B";
   case G69D:
      return "G69D";
   case Q71A:
      return "Q71A";
   case Q71B:
      return "Q71B";
   case Q71D:
      return "Q71D";
   case G71A:
      return "G71A";
   case G71B:
      return "G71B";
   case G71D:
      return "G71D";
   case Q74A:
      return "Q74A";
   case Q74B:
      return "Q74B";
   case Q74D:
      return "Q74D";
   case G74A:
      return "G74A";
   case G74B:
      return "G74B";
   case G74D:
      return "G74D";
   case Q76A:
      return "Q76A";
   case Q76B:
      return "Q76B";
   case Q76D:
      return "Q76D";
   case G76A:
      return "G76A";
   case G76B:
      return "G76B";
   case G76D:
      return "G76D";
   case Q93A:
      return "Q93A";
   case Q93B:
      return "Q93B";
   case Q93D:
      return "Q93D";
   case G93A:
      return "G93A";
   case G93B:
      return "G93B";
   case G93D:
      return "G93D";
   case Q800:
      return "Q800";
   case Q801:
      return "Q801";
   case Q803:
      return "Q803";
   case Q805:
      return "Q805";
   case Q806:
      return "Q806";
   case Q808:
      return "Q808";
   case Q80A:
      return "Q80A";
   case Q80B:
      return "Q80B";
   case Q80D:
      return "Q80D";
   case G800:
      return "G800";
   case G801:
      return "G801";
   case G803:
      return "G803";
   case G805:
      return "G805";
   case G806:
      return "G806";
   case G808:
      return "G808";
   case G80A:
      return "G80A";
   case G80B:
      return "G80B";
   case G80D:
      return "G80D";
   case Q72A:
      return "Q72A";
   case Q72B:
      return "Q72B";
   case Q72D:
      return "Q72D";
   case G72A:
      return "G72A";
   case G72B:
      return "G72B";
   case G72D:
      return "G72D";
   case Q98A:
      return "Q98A";
   case Q98B:
      return "Q98B";
   case Q98D:
      return "Q98D";
   case G98A:
      return "G98A";
   case G98B:
      return "G98B";
   case G98D:
      return "G98D";

   case C500:
      return "C500";
   case C505:
      return "C505";
   case C600:
      return "C600";
  case C605:
      return "C605";
  case C64A:
      return "C64A";

  case B530:
      return "B530";
  case B535:
      return "B535";
  case B74A:
      return "B74A";


   case DT_INVALID: case DT_UNKNOWN:
      break; // Only avoid nasty compiler warnings
   }

// Default return empty string. Should prevent misflashes due to programming bugs
   return "\0\0\0\0";
}

/******************************************************************************\
| function: GetNECDriveType                                                    |
|                                                                              |
| Purpose:                                                                     |
| Check the drivetype of given device or string buffer                         |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler - may be set to 0 if buffer is given                    |
| buffer -> optional buffer when comparing with given string instead of drive  |
|                                                                              |
| Returnvalues:                                                                |
| -1 -> Drive is supported                                                     |
| >=0 -> Recognized drivetype                                                  |
\******************************************************************************/
DriveType IdentifyDrive(Scsi* scsi, u8* buffer)
{
   u8 check[0x20];
   bool ok=false;

   if (!buffer)
   {
      if (!ReadRAM(scsi,check,0x4500,0x20,0))
         memset(check,0,4);
      else
         ok=true;
   }
   else
      memcpy(check, buffer, 4);

   if (!memcmp(check,"K100",4) || !memcmp(check,"K111",4) || !memcmp(check,"K110",4) || !memcmp(check,"K130",4) || !memcmp(check,"A110",4))
      return K1;  // NEC 1x00A
   if (!memcmp(check,"L110",4) || !memcmp(check,"L130",4))
      return L1;  // NEC 5x00A
   if (!memcmp(check,"L11R",4) || !memcmp(check,"L13R",4))
      return L1;  // NEC 5x00A reverse ATA - only used in File
   if (!memcmp(check,"L210",4) || !memcmp(check,"L250",4) || !memcmp(check,"L245",4))
      return L2;  // NEC 6x00A
   if (!memcmp(check,"L21R",4) || !memcmp(check,"L25R",4))
      return L2;  // NEC 6x00A reverse ATA - only used in File


   if (!buffer)
   {
      if (!ReadRAM(scsi,check,0x4800,0x20,0))
         memset(check,0,4);
      else
         ok=true;
   }
   if (!memcmp(check,"K210",4) || !memcmp(check,"K250",4) || !memcmp(check,"K251",4))
      return K2;  // NEC 2x00A
   if (!memcmp(check,"K310",4) || !memcmp(check,"K350",4) || !memcmp(check,"K345",4))
      return K350;  // NEC 3x00A
   if (!memcmp(check,"K312",4) || !memcmp(check,"K352",4))
      return K352;  // NEC 3520A
   if (!memcmp(check,"K313",4) || !memcmp(check,"K353",4))
      return K353;  // NEC 3530A
   if (!memcmp(check,"K314",4) || !memcmp(check,"K354",4))
      return K354;  // NEC 3540A
   if (!memcmp(check,"L211",4) || !memcmp(check,"L251",4))
      return L251;  // NEC 6650A
   if (!memcmp(check,"LY1R",4) || !memcmp(check,"LY5R",4))
      return L251;  // NEC 6650A reverse ATA - only used in File
   if (!memcmp(check,"K315",4) || !memcmp(check,"K355",4))
      return K355;  // NEC 3550A
   if (!memcmp(check,"D315",4) || !memcmp(check,"D355",4))
      return D355;  // NEC 3550A - special versions
   if (!memcmp(check,"K316",4) || !memcmp(check,"K356",4))
      return K356;  // NEC 3551A
   if (!memcmp(check,"K450",4) || !memcmp(check,"K410",4))
      return K450;  // NEC 4550A
   if (!memcmp(check,"K451",4) || !memcmp(check,"K411",4))
      return K451;  // NEC 4551A
   if (!memcmp(check,"L212",4) || !memcmp(check,"L252",4))
      return L252;  // NEC 6750A
   if (!memcmp(check,"L213",4) || !memcmp(check,"L253",4))
      return L253;  // NEC 6751A
   if (!memcmp(check,"L350",4) || !memcmp(check,"L310",4))
      return L350;  // NEC 7550A
// 7551 seems to return L350. Correct drivetype determined in check for crossflash
   if (!memcmp(check,"L351",4) || !memcmp(check,"L311",4))
      return L351;  // NEC 7551A
   if (!memcmp(check,"K365",4) || !memcmp(check,"K325",4))
      return K365;  // NEC 3650A
   if (!memcmp(check,"K366",4) || !memcmp(check,"K326",4))
      return K366;  // NEC 3651A
   if (!memcmp(check,"K370",4) || !memcmp(check,"K330",4))
      return K370;  // NEC 3570A
   if (!memcmp(check,"K371",4) || !memcmp(check,"K331",4))
      return K371;  // NEC 3571A
   if (!memcmp(check,"K460",4) || !memcmp(check,"K420",4))
      return K460;  // NEC 4650A
   if (!memcmp(check,"K461",4) || !memcmp(check,"K421",4))
      return K461;  // NEC 4651A
   if (!memcmp(check,"K470",4) || !memcmp(check,"K430",4))
      return K470;  // NEC 4570A
   if (!memcmp(check,"K471",4) || !memcmp(check,"K431",4))
      return K471;  // NEC 4571A
   if (!memcmp(check,"D570",4) || !memcmp(check,"D530",4) || !memcmp(check,"K380",4) || !memcmp(check,"K340",4))
      return D570;  // Optiarc 5170A
   if (!memcmp(check,"D57D",4) || !memcmp(check,"D53D",4) || !memcmp(check,"K38D",4) || !memcmp(check,"K34D",4))
      return D57D;  // Optiarc 5170A for Duplicators
   if (!memcmp(check,"D573",4) || !memcmp(check,"D533",4) || !memcmp(check,"K381",4) || !memcmp(check,"K341",4))
      return D573;  // Optiarc 5173A
   if (!memcmp(check,"D770",4) || !memcmp(check,"D730",4) || !memcmp(check,"K480",4) || !memcmp(check,"K440",4))
      return D770;  // Optiarc 7170A
   if (!memcmp(check,"D773",4) || !memcmp(check,"D733",4) || !memcmp(check,"K481",4) || !memcmp(check,"K441",4))
      return D773;  // Optiarc 7173A
   if (!memcmp(check,"L260",4) || !memcmp(check,"L220",4))
      return L260;  // Optiarc 5540A
   if (!memcmp(check,"L261",4) || !memcmp(check,"L221",4))
      return L261;  // Optiarc 5543A
   if (!memcmp(check,"L450",4) || !memcmp(check,"L410",4))
      return L450;  // Optiarc 7540A
   if (!memcmp(check,"L451",4) || !memcmp(check,"L411",4))
      return L451;  // Optiarc 7543A
   if (!memcmp(check,"Q175",4) || !memcmp(check,"Q135",4))
      return Q175;  // Optiarc 5170S
   if (!memcmp(check,"Q178",4) || !memcmp(check,"Q138",4))
      return Q178;  // Optiarc 5173S
   if (!memcmp(check,"G175",4) || !memcmp(check,"G135",4))
      return G175;  // Optiarc 7170S
   if (!memcmp(check,"G178",4) || !memcmp(check,"G138",4))
      return G178;  // Optiarc 7173S
   if (!memcmp(check,"Q630",4))
      return Q630;  // Optiarc 5630A
   if (!memcmp(check,"Q633",4))
      return Q633;  // Optiarc 5633A
   if (!memcmp(check,"G630",4))
      return G630;  // Optiarc 7630A
   if (!memcmp(check,"G633",4))
      return G633;  // Optiarc 7633A
   if (!memcmp(check,"Q200",4))
      return Q200;  // Optiarc 5200A
   if (!memcmp(check,"Q201",4))
      return Q201;  // Optiarc 5201A
   if (!memcmp(check,"Q203",4))
      return Q203;  // Optiarc 5203A
   if (!memcmp(check,"Q205",4))
      return Q205;  // Optiarc 5200S
   if (!memcmp(check,"Q206",4))
      return Q206;  // Optiarc 5201S
   if (!memcmp(check,"Q208",4))
      return Q208;  // Optiarc 5203S
   if (!memcmp(check,"G200",4))
      return G200;  // Optiarc 7200A
   if (!memcmp(check,"G201",4))
      return G201;  // Optiarc 7201A
   if (!memcmp(check,"G203",4))
      return G203;  // Optiarc 7203A
   if (!memcmp(check,"G205",4))
      return G205;  // Optiarc 7200S
   if (!memcmp(check,"G206",4))
      return G206;  // Optiarc 7201S
   if (!memcmp(check,"G208",4))
      return G208;  // Optiarc 7203S
   if (!memcmp(check,"Q590",4))
      return Q590;  // Optiarc 5590A
   if (!memcmp(check,"Q591",4))
      return Q591;  // Optiarc 5591A
   if (!memcmp(check,"Q593",4))
      return Q593;  // Optiarc 5593A
   if (!memcmp(check,"Q595",4))
      return Q595;  // Optiarc 5590S
   if (!memcmp(check,"Q596",4))
      return Q596;  // Optiarc 5591S
   if (!memcmp(check,"Q598",4))
      return Q598;  // Optiarc 5593S
   if (!memcmp(check,"G590",4))
      return G590;  // Optiarc 7590A
   if (!memcmp(check,"G591",4))
      return G591;  // Optiarc 7591A
   if (!memcmp(check,"G593",4))
      return G593;  // Optiarc 7593A
   if (!memcmp(check,"G595",4))
      return G595;  // Optiarc 7590S
   if (!memcmp(check,"G596",4))
      return G596;  // Optiarc 7591S
   if (!memcmp(check,"G598",4))
      return G598;  // Optiarc 7593S
   if (!memcmp(check,"Q670",4))
      return Q670;  // Optiarc 5670A
   if (!memcmp(check,"Q673",4))
      return Q673;  // Optiarc 5673A
   if (!memcmp(check,"Q675",4))
      return Q675;  // Optiarc 5670S
   if (!memcmp(check,"Q678",4))
      return Q678;  // Optiarc 5673S
   if (!memcmp(check,"G670",4))
      return G670;  // Optiarc 7670A
   if (!memcmp(check,"G673",4))
      return G673;  // Optiarc 7673A
   if (!memcmp(check,"G675",4))
      return G675;  // Optiarc 7670S
   if (!memcmp(check,"G678",4))
      return G678;  // Optiarc 7673S
   if (!memcmp(check,"Q910",4))
      return Q910;  // Optiarc 5910A
   if (!memcmp(check,"Q913",4))
      return Q913;  // Optiarc 5913A
   if (!memcmp(check,"Q915",4))
      return Q915;  // Optiarc 5910S
   if (!memcmp(check,"Q918",4))
      return Q918;  // Optiarc 5913S
   if (!memcmp(check,"G910",4))
      return G910;  // Optiarc 7910A
   if (!memcmp(check,"G913",4))
      return G913;  // Optiarc 7913A
   if (!memcmp(check,"G915",4))
      return G915;  // Optiarc 7910S
   if (!memcmp(check,"G918",4))
      return G918;  // Optiarc 7913S
   if (!memcmp(check,"Q960",4))
      return Q960;  // Optiarc 5960A
   if (!memcmp(check,"Q963",4))
      return Q963;  // Optiarc 5963A
   if (!memcmp(check,"Q965",4))
      return Q965;  // Optiarc 5960S
   if (!memcmp(check,"Q968",4))
      return Q968;  // Optiarc 5963S
   if (!memcmp(check,"G960",4))
      return G960;  // Optiarc 7960A
   if (!memcmp(check,"G963",4))
      return G963;  // Optiarc 7963A
   if (!memcmp(check,"G965",4))
      return G965;  // Optiarc 7960S
   if (!memcmp(check,"G968",4))
      return G968;  // Optiarc 7963S


   if (!buffer)
   {
      if (!ReadRAM(scsi,check,0x10800,0x20,0))
         memset(check,0,4);
      else
         ok=true;
   }

// Could also move Q200, Q201, Q203, Q205, Q206, Q208, G200, G201, G203, G205, G206, G208
// Q590, Q593, Q595, Q598, G590, G593, G595, G598 and maybe even some more drives to this
// block but don't have to as they support both areas
   if (!memcmp(check,"Q230",4))
      return Q230;  // Optiarc 5230A
   if (!memcmp(check,"Q231",4))
      return Q231;  // Optiarc 5231A
   if (!memcmp(check,"Q233",4))
      return Q233;  // Optiarc 5233A
   if (!memcmp(check,"Q235",4))
      return Q235;  // Optiarc 5230S
   if (!memcmp(check,"Q236",4))
      return Q236;  // Optiarc 5231S
   if (!memcmp(check,"Q238",4))
      return Q238;  // Optiarc 5233S
   if (!memcmp(check,"G230",4))
      return G230;  // Optiarc 7230A
   if (!memcmp(check,"G231",4))
      return G231;  // Optiarc 7231A
   if (!memcmp(check,"G233",4))
      return G233;  // Optiarc 7233A
   if (!memcmp(check,"G235",4))
      return G235;  // Optiarc 7230S
   if (!memcmp(check,"G236",4))
      return G236;  // Optiarc 7231S
   if (!memcmp(check,"G238",4))
      return G238;  // Optiarc 7233S
   if (!memcmp(check,"Q240",4))
      return Q240;  // Optiarc 5240A
   if (!memcmp(check,"Q241",4))
      return Q241;  // Optiarc 5241A
   if (!memcmp(check,"Q243",4))
      return Q243;  // Optiarc 5243A
   if (!memcmp(check,"Q245",4))
      return Q245;  // Optiarc 5240S
   if (!memcmp(check,"Q246",4))
      return Q246;  // Optiarc 5241S
   if (!memcmp(check,"Q248",4))
      return Q248;  // Optiarc 5243S
   if (!memcmp(check,"G240",4))
      return G240;  // Optiarc 7240A
   if (!memcmp(check,"G241",4))
      return G241;  // Optiarc 7241A
   if (!memcmp(check,"G243",4))
      return G243;  // Optiarc 7243A
   if (!memcmp(check,"G245",4) || !memcmp(check,"P245",4))
      return G245;  // Optiarc 7240S - LiteOn iHAS324 Y
   if (!memcmp(check,"G246",4) || !memcmp(check,"P246",4))
      return G246;  // Optiarc 7241S - LiteOn iHAS424 Y
   if (!memcmp(check,"G248",4) || !memcmp(check,"P248",4))
      return G248;  // Optiarc 7243S - LiteOn iHAS624 Y ?
   if (!memcmp(check,"G25A",4))
      return G25A;  // Optiarc 7250H
   if (!memcmp(check,"G25B",4))
      return G25B;  // Optiarc 7251H
   if (!memcmp(check,"G25D",4))
      return G25D;  // Optiarc 7253H
   if (!memcmp(check,"Q260",4))
      return Q260;  // Optiarc 5260A
   if (!memcmp(check,"Q261",4))
      return Q261;  // Optiarc 5261A
   if (!memcmp(check,"Q263",4))
      return Q263;  // Optiarc 5263A
   if (!memcmp(check,"Q265",4))
      return Q265;  // Optiarc 5260S
   if (!memcmp(check,"Q266",4))
      return Q266;  // Optiarc 5261S
   if (!memcmp(check,"Q268",4))
      return Q268;  // Optiarc 5263S
   if (!memcmp(check,"G260",4))
      return G260;  // Optiarc 7260A
   if (!memcmp(check,"G261",4))
      return G261;  // Optiarc 7261A
   if (!memcmp(check,"G263",4))
      return G263;  // Optiarc 7263A
   if (!memcmp(check,"G265",4))
      return G265;  // Optiarc 7260S
   if (!memcmp(check,"G266",4))
      return G266;  // Optiarc 7261S
   if (!memcmp(check,"G268",4))
      return G268;  // Optiarc 7263S
   if (!memcmp(check,"G27A",4))
      return G27A;  // Optiarc 7270H
   if (!memcmp(check,"G27B",4))
      return G27B;  // Optiarc 7271H
   if (!memcmp(check,"G27D",4))
      return G27D;  // Optiarc 7273H
   if (!memcmp(check,"Q280",4))
      return Q280;  // Optiarc 5280A
   if (!memcmp(check,"Q281",4))
      return Q281;  // Optiarc 5281A
   if (!memcmp(check,"Q283",4))
      return Q283;  // Optiarc 5283A
   if (!memcmp(check,"Q285",4))
      return Q285;  // Optiarc 5280S
   if (!memcmp(check,"Q286",4))
      return Q286;  // Optiarc 5281S
   if (!memcmp(check,"Q288",4))
      return Q288;  // Optiarc 5283S
   if (!memcmp(check,"G280",4))
      return G280;  // Optiarc 7280A
   if (!memcmp(check,"G281",4))
      return G281;  // Optiarc 7281A
   if (!memcmp(check,"G283",4))
      return G283;  // Optiarc 7283A
   if (!memcmp(check,"G285",4))
      return G285;  // Optiarc 7280S
   if (!memcmp(check,"G286",4))
      return G286;  // Optiarc 7281S
   if (!memcmp(check,"G288",4))
      return G288;  // Optiarc 7283S
   if (!memcmp(check,"G29A",4))
      return G29A;  // Optiarc 7290H
   if (!memcmp(check,"G29B",4))
      return G29B;  // Optiarc 7291H
   if (!memcmp(check,"G29D",4))
      return G29D;  // Optiarc 7293H
   if (!memcmp(check,"Q300",4))
      return Q300;  // Optiarc 5300A
   if (!memcmp(check,"Q301",4))
      return Q301;  // Optiarc 5301A
   if (!memcmp(check,"Q303",4))
      return Q303;  // Optiarc 5303A
   if (!memcmp(check,"Q305",4))
      return Q305;  // Optiarc 5300S
   if (!memcmp(check,"Q306",4))
      return Q306;  // Optiarc 5301S
   if (!memcmp(check,"Q308",4))
      return Q308;  // Optiarc 5303S
   if (!memcmp(check,"G300",4))
      return G300;  // Optiarc 7300A
   if (!memcmp(check,"G301",4))
      return G301;  // Optiarc 7301A
   if (!memcmp(check,"G303",4))
      return G303;  // Optiarc 7303A
   if (!memcmp(check,"G305",4))
      return G305;  // Optiarc 7300S
   if (!memcmp(check,"G306",4))
      return G306;  // Optiarc 7301S
   if (!memcmp(check,"G308",4))
      return G308;  // Optiarc 7303S
   if (!memcmp(check,"Q325",4))
      return Q325;  // Optiarc 5320S
   if (!memcmp(check,"Q326",4))
      return Q326;  // Optiarc 5321S
   if (!memcmp(check,"Q328",4))
      return Q328;  // Optiarc 5323S
   if (!memcmp(check,"G325",4))
      return G325;  // Optiarc 7320S
   if (!memcmp(check,"G326",4))
      return G326;  // Optiarc 7321S
   if (!memcmp(check,"G328",4))
      return G328;  // Optiarc 7323S
   if (!memcmp(check,"Q700",4))
      return Q700;  // Optiarc 5700A
   if (!memcmp(check,"Q701",4))
      return Q701;  // Optiarc 5701A
   if (!memcmp(check,"Q703",4))
      return Q703;  // Optiarc 5703A
   if (!memcmp(check,"Q705",4))
      return Q705;  // Optiarc 5700S
   if (!memcmp(check,"Q706",4))
      return Q706;  // Optiarc 5701S
   if (!memcmp(check,"Q708",4))
      return Q708;  // Optiarc 5703S
   if (!memcmp(check,"Q70A",4))
      return Q70A;  // Optiarc 5700H
   if (!memcmp(check,"Q70B",4))
      return Q70B;  // Optiarc 5701H
   if (!memcmp(check,"Q70D",4))
      return Q70D;  // Optiarc 5703H
   if (!memcmp(check,"G700",4))
      return G700;  // Optiarc 7700A
   if (!memcmp(check,"G701",4))
      return G701;  // Optiarc 7701A
   if (!memcmp(check,"G703",4))
      return G703;  // Optiarc 7703A
   if (!memcmp(check,"G705",4))
      return G705;  // Optiarc 7700S
   if (!memcmp(check,"G706",4))
      return G706;  // Optiarc 7701S
   if (!memcmp(check,"G708",4))
      return G708;  // Optiarc 7703S
   if (!memcmp(check,"G70A",4))
      return G70A;  // Optiarc 7700H
   if (!memcmp(check,"G70B",4))
      return G70B;  // Optiarc 7701H
   if (!memcmp(check,"G70D",4))
      return G70D;  // Optiarc 7703H
   if (!memcmp(check,"Q800",4))
      return Q800;  // Optiarc 5800A
   if (!memcmp(check,"Q801",4))
      return Q801;  // Optiarc 5801A
   if (!memcmp(check,"Q803",4))
      return Q803;  // Optiarc 5803A
   if (!memcmp(check,"Q805",4))
      return Q805;  // Optiarc 5800S
   if (!memcmp(check,"Q806",4))
      return Q806;  // Optiarc 5801S
   if (!memcmp(check,"Q808",4))
      return Q808;  // Optiarc 5803S
   if (!memcmp(check,"Q80A",4))
      return Q80A;  // Optiarc 5800H
   if (!memcmp(check,"Q80B",4))
      return Q80B;  // Optiarc 5801H
   if (!memcmp(check,"Q80D",4))
      return Q80D;  // Optiarc 5803H
   if (!memcmp(check,"G800",4))
      return G800;  // Optiarc 7800A
   if (!memcmp(check,"G801",4))
      return G801;  // Optiarc 7801A
   if (!memcmp(check,"G803",4))
      return G803;  // Optiarc 7803A
   if (!memcmp(check,"G805",4))
      return G805;  // Optiarc 7800S
   if (!memcmp(check,"G806",4))
      return G806;  // Optiarc 7801S
   if (!memcmp(check,"G808",4))
      return G808;  // Optiarc 7803S
   if (!memcmp(check,"G80A",4))
      return G80A;  // Optiarc 7800H
   if (!memcmp(check,"G80B",4))
      return G80B;  // Optiarc 7801H
   if (!memcmp(check,"G80D",4))
      return G80D;  // Optiarc 7803H
   if (!memcmp(check,"Q68A",4))
      return Q68A;  // Optiarc 5680H
   if (!memcmp(check,"Q68B",4))
      return Q68B;  // Optiarc 5681H
   if (!memcmp(check,"Q68D",4))
      return Q68D;  // Optiarc 5683H
   if (!memcmp(check,"Q97A",4))
      return Q97A;  // Optiarc 5970H
   if (!memcmp(check,"Q97B",4))
      return Q97B;  // Optiarc 5971H
   if (!memcmp(check,"Q97D",4))
      return Q97D;  // Optiarc 5973H
   if (!memcmp(check,"G97A",4))
      return G97A;  // Optiarc 7970H
   if (!memcmp(check,"G97B",4))
      return G97B;  // Optiarc 7971H
   if (!memcmp(check,"G97D",4))
      return G97D;  // Optiarc 7973H
   if (!memcmp(check,"Q69A",4))
      return Q69A;  // Optiarc 5690H
   if (!memcmp(check,"Q69B",4))
      return Q69B;  // Optiarc 5691H
   if (!memcmp(check,"Q69D",4))
      return Q69D;  // Optiarc 5693H
   if (!memcmp(check,"G69A",4))
      return G69A;  // Optiarc 7690H
   if (!memcmp(check,"G69B",4))
      return G69B;  // Optiarc 7691H
   if (!memcmp(check,"G69D",4))
      return G69D;  // Optiarc 7693H
   if (!memcmp(check,"G71A",4))
      return G71A;  // Optiarc 5710H
   if (!memcmp(check,"G71B",4))
      return G71B;  // Optiarc 5711H
   if (!memcmp(check,"G71D",4))
      return G71D;  // Optiarc 5713H
   if (!memcmp(check,"G71A",4))
      return G71A;  // Optiarc 7710H
   if (!memcmp(check,"G71B",4))
      return G71B;  // Optiarc 7711H
   if (!memcmp(check,"G71D",4))
      return G71D;  // Optiarc 7713H
   if (!memcmp(check,"G72A",4))
      return G72A;  // Optiarc 5720H
   if (!memcmp(check,"G72B",4))
      return G72B;  // Optiarc 5721H
   if (!memcmp(check,"G72D",4))
      return G72D;  // Optiarc 5723H
   if (!memcmp(check,"G72A",4))
      return G72A;  // Optiarc 7720H
   if (!memcmp(check,"G72B",4))
      return G72B;  // Optiarc 7721H
   if (!memcmp(check,"G72D",4))
      return G72D;  // Optiarc 7723H
   if (!memcmp(check,"G74A",4))
      return G74A;  // Optiarc 5740H
   if (!memcmp(check,"G74B",4))
      return G74B;  // Optiarc 5741H
   if (!memcmp(check,"G74D",4))
      return G74D;  // Optiarc 5743H
   if (!memcmp(check,"G74A",4))
      return G74A;  // Optiarc 7740H
   if (!memcmp(check,"G74B",4))
      return G74B;  // Optiarc 7741H
   if (!memcmp(check,"G74D",4))
      return G74D;  // Optiarc 7743H
   if (!memcmp(check,"G76A",4))
      return G76A;  // Optiarc 5760H
   if (!memcmp(check,"G76B",4))
      return G76B;  // Optiarc 5761H
   if (!memcmp(check,"G76D",4))
      return G76D;  // Optiarc 5763H
   if (!memcmp(check,"G76A",4))
      return G76A;  // Optiarc 7760H
   if (!memcmp(check,"G76B",4))
      return G76B;  // Optiarc 7761H
   if (!memcmp(check,"G76D",4))
      return G76D;  // Optiarc 7763H
   if (!memcmp(check,"G98A",4))
      return G98A;  // Optiarc 5790H
   if (!memcmp(check,"G98B",4))
      return G98B;  // Optiarc 5791H
   if (!memcmp(check,"G98D",4))
      return G98D;  // Optiarc 5793H
   if (!memcmp(check,"G98A",4))
      return G98A;  // Optiarc 7790H
   if (!memcmp(check,"G98B",4))
      return G98B;  // Optiarc 7791H
   if (!memcmp(check,"G98D",4))
      return G98D;  // Optiarc 7793H
   if (!memcmp(check,"G93A",4))
      return G93A;  // Optiarc 5930H
   if (!memcmp(check,"G93B",4))
      return G93B;  // Optiarc 5931H
   if (!memcmp(check,"G93D",4))
      return G93D;  // Optiarc 5933H
   if (!memcmp(check,"G93A",4))
      return G93A;  // Optiarc 7930H
   if (!memcmp(check,"G93B",4))
      return G93B;  // Optiarc 7931H
   if (!memcmp(check,"G93D",4))
      return G93D;  // Optiarc 7933H

   if (!buffer)
   {
      if (!ReadRAM(scsi,check,0x7000,0x20,0))
         memset(check,0,4);
      else
         ok=true;
   }
   if (!memcmp(check,"C500",4))
      return C500;  // Optiarc BC-5500A
   if (!memcmp(check,"C505",4))
      return C505;  // Optiarc BC-5500S
   if (!memcmp(check,"C600",4))
      return C600;  // Optiarc BC-5600A
   if (!memcmp(check,"C605",4))
      return C605;  // Optiarc BC-5600S
   if (!memcmp(check,"C64A",4))
      return C64A;  // Optiarc BC-5640H
   if (!memcmp(check,"B530",4))
      return B530;  // Optiarc BD-5730A
   if (!memcmp(check,"B535",4))
      return B535;  // Optiarc BD-5730S
   if (!memcmp(check,"B74A",4))
      return B74A;  // Optiarc BD-5740H

   if (!buffer)
   {
      if (!ReadRAM(scsi,check,0x5000,0x20,0))
         memset(check,0,4);
      else
         ok=true;
   }
   if (!memcmp(check,"M110",4))
      return M110;  // NEC HR-1100A

   if (ok)
      return DT_UNKNOWN;
   else
      return DT_INVALID;
}


/******************************************************************************\
| function: GetIDFromDriveType                                                 |
|                                                                              |
| Purpose:                                                                     |
| Get the unique ID for a specific drive type                                  |
|                                                                              |
| Input:                                                                       |
| drivetype -> Current drive                                                   |
|                                                                              |
| Returnvalues:                                                                |
| 0 -> Not needed or not yet supported                                         |
| >=0 -> drive ID                                                              |
| 0xff -> Dumping firmwares not (yet) supported                                |
\******************************************************************************/
u8 GetIDFromDriveType(Scsi* scsi, DriveType drivetype)
{
   u8 retval=0;
   u8 buffer[4];

   switch (drivetype)
   {
   case K370:case K371:case K470:case K471:
      if (ReadRAM(scsi, buffer, 0x10000, 1, 0, 0))
         retval=buffer[0];
      else
         retval=0xff;
      break;

   case D570:case D57D:case D573:case D770:case D773:
   case L260:case L261:case L450:case L451:
   case Q175:case Q178:case G175:case G178:
   case Q630:case Q633:case G630:case G633:
   case Q200:case Q201:case Q203:case Q205:case Q206:case Q208:
   case G200:case G201:case G203:case G205:case G206:case G208:
   case Q230:case Q231:case Q233:case Q235:case Q236:case Q238:
   case G230:case G231:case G233:case G235:case G236:case G238:
   case Q240:case Q241:case Q243:case Q245:case Q246:case Q248:
   case G240:case G241:case G243:case G245:case G246:case G248:
   case G25A:case G25B:case G25D:
   case Q260:case Q261:case Q263:case Q265:case Q266:case Q268:
   case G260:case G261:case G263:case G265:case G266:case G268:
   case G27A:case G27B:case G27D:
   case Q280:case Q281:case Q283:case Q285:case Q286:case Q288:
   case G280:case G281:case G283:case G285:case G286:case G288:
   case G29A:case G29B:case G29D:
   case Q300:case Q301:case Q303:case Q305:case Q306:case Q308:
   case G300:case G301:case G303:case G305:case G306:case G308:
   case Q325:case Q326:case Q328:case G325:case G326:case G328:
   case Q590:case Q591:case Q593:case Q595:case Q596:case Q598:
   case G590:case G591:case G593:case G595:case G596:case G598:
   case Q670:case Q673:case Q675:case Q678:case G670:case G673:case G675:case G678:
   case Q910:case Q913:case Q915:case Q918:case G910:case G913:case G915:case G918:
   case Q960:case Q963:case Q965:case Q968:case G960:case G963:case G965:case G968:
   case Q700:case Q701:case Q703:case Q705:case Q706:case Q708:case Q70A:case Q70B:case Q70D:
   case G700:case G701:case G703:case G705:case G706:case G708:case G70A:case G70B:case G70D:
   case Q800:case Q801:case Q803:case Q805:case Q806:case Q808:case Q80A:case Q80B:case Q80D:
   case G800:case G801:case G803:case G805:case G806:case G808:case G80A:case G80B:case G80D:
   case Q68A:case Q68B:case Q68D:
   case Q69A:case Q69B:case Q69D:case G69A:case G69B:case G69D:
   case Q97A:case Q97B:case Q97D:case G97A:case G97B:case G97D:
   case Q71A:case Q71B:case Q71D:case G71A:case G71B:case G71D:
   case Q72A:case Q72B:case Q72D:case G72A:case G72B:case G72D:
   case Q74A:case Q74B:case Q74D:case G74A:case G74B:case G74D:
   case Q76A:case Q76B:case Q76D:case G76A:case G76B:case G76D:
   case Q98A:case Q98B:case Q98D:case G98A:case G98B:case G98D:
   case Q93A:case Q93B:case Q93D:case G93A:case G93B:case G93D:
      if (ReadRAM(scsi, buffer, 0x30000, 1, 0, 0))
         retval=buffer[0];
      else
         retval=0xff;
      break;

   case C500:case C505:case C600:case C605:case C64A:case B530:case B535:case B74A: // No BD-Dump support yet
      retval=0xff;
      break;

   default:
      break;
   }

   return retval;
}


/******************************************************************************\
| function: GetIDFromDrive                                                     |
|                                                                              |
| Purpose:                                                                     |
| Retrieve unique Firmware ID from a drive                                     |
| CURRENTLY ONLY SUPPORTS NOTEBOOK DRIVES!                                     |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| drivetype -> Current drive                                                   |
|                                                                              |
| Returnvalues:                                                                |
| 0 -> Not needed or not yet supported                                         |
| >=0 -> drive ID                                                              |
\******************************************************************************/
u8 GetIDFromDrive(Scsi* scsi, DriveType drivetype)
{
   u32 location;
   u8 buffer[16];
   u16 readlength;

   switch (drivetype)
   {
   case L1:
      location=0x5000;readlength=4;break;
   case L2:
      location=0x8400;readlength=4;break;
   case L251:
      location=0x6400;readlength=4;break;
   case L252:case L253:case L350:case L351:
      location=0x10000;readlength=4;break;
   case L260:case L261:
      location=0x30000;readlength=1;break;
   default:
      location=0;readlength=0;break;
   }

   if (!location)
      return 0xff;

   if (!ReadRAM(scsi, buffer, location, readlength, false))
      return 0xff;
   else
      return buffer[0];
}


/******************************************************************************\
| function: GetIDFromBuffer                                                    |
|                                                                              |
| Purpose:                                                                     |
| Retrieve unique Firmware ID from a buffer                                    |
| CURRENTLY ONLY SUPPORTS NOTEBOOK DRIVES!                                     |
|                                                                              |
| Input:                                                                       |
| buffer -> Firmware buffer                                                    |
| drivetype -> Current drive                                                   |
|                                                                              |
| Returnvalues:                                                                |
| 0xff -> Not needed or not yet supported                                      |
| >=0 -> drive ID                                                              |
\******************************************************************************/
u8 GetIDFromBuffer(u8* buffer, DriveType drivetype)
{
   u8 retval;

   switch (drivetype)
   {
   case L1:case L252:case L253:case L350:case L351:
   case L260:case L261:
      retval=buffer[0]; break;
   case L2:case L251:
      retval=buffer[0x400]; break;
   default:
      retval=0xff; break;
   }

   return retval;
}


/******************************************************************************\
| function: GetCompatibleLaptop                                                |
|                                                                              |
| Purpose:                                                                     |
| Check for allowed laptop drive crossflashing                                 |
|                                                                              |
| Input:                                                                       |
| ID -> Firmware ID                                                            |
|                                                                              |
| Returnvalues:                                                                |
| Laptop ID                                                                    |
\******************************************************************************/
CompatibleLaptop GetCompatibleLaptop(u8 ID)
{
   CompatibleLaptop retval;

   switch (ID)
   {
   case 0x6f:case 0x7c: // 5100 - 5500
      retval=L1Master; break;
   case 0x7b:case 0x7f: // 5100 - 5500
      retval=L1Slave; break;
   case 0x78:case 0x86: case 0x84: // 6100 - 6450 - 6500
      retval=L2Master; break;
   case 0x85: // 6500
      retval=L2Slave; break;
   case 0x87: // 6650
      retval=L251Master; break;
   case 0x8c: // 6650
      retval=L251Slave; break;
   case 0x97: // 6750
      retval=L252Master; break;
   case 0x98: // 6750
      retval=L252Slave; break;
//   case 0x:
//      retval=L253Master; break;
//   case 0x:
//      retval=L253Slave; break;
   case 0x8f: // 7550
      retval=L350Master; break;
   case 0x90: // 7550
      retval=L350Slave; break;
   case 0x95: // 7551
      retval=L351Master; break;
   case 0x96: // 7551
      retval=L351Slave; break;
   case 0xAC: // 5540
      retval=L260Master; break;
   default:
      retval=L_INVALID; break;
   }

   return retval;
}


/******************************************************************************\
| function: GetLaptopBootcode                                                  |
|                                                                              |
| Purpose:                                                                     |
| Retrieve the expected bootcode version for a connection specific firmware    |
|                                                                              |
| Input:                                                                       |
| drivetype -> connection specific drive                                       |
|                                                                              |
| Returnvalues:                                                                |
| 2 Bytes as u16 - characters after '0.'                                       |
\******************************************************************************/
u16 GetLaptopBootcode(CompatibleLaptop drivetype)
{
   switch (drivetype)
   {
   case L1Master:
      return 0x3035; // 0.05
//TODO: This value still needs to be confirmed - but is very likely to be correct
   case L1Slave:
      return 0x3036; // 0.06
   case L2Master:
      return 0x3030; // 0.00
   case L2Slave:
      return 0x3031; // 0.01
   case L251Master:
      return 0x3035; // 0.05
   case L251Slave:
      return 0x3036; // 0.06
   case L252Master:
      return 0x3034; // 0.04
   case L252Slave:
      return 0x3035; // 0.05
   case L253Master:
      return 0x3034; // 0.04
   case L253Slave:
      return 0x3035; // 0.05
   case L350Master:
      return 0x3034; // 0.04
   case L350Slave:
      return 0x3035; // 0.05
   case L351Master:
      return 0x3034; // 0.04
   case L351Slave:
      return 0x3035; // 0.05
   case L260Master:
      return 0x3038; // 0.08
// case L260Slave:
//    return 0x3039; // 0.09
// case L261Master:
//    return 0x3038; // 0.08
// case L261Slave:
//    return 0x3039; // 0.09

   default:
      return 0xffff; // This should not happen
   }
}


/******************************************************************************\
| function: GetFlashSizeFromDriveType                                          |
|                                                                              |
| Purpose:                                                                     |
| Retrieve the size of a drive's flash memory                                  |
|                                                                              |
| Input:                                                                       |
| drivetype -> Current drive                                                   |
|                                                                              |
| Returnvalues:                                                                |
| Size of the drive's flash memory                                             |
\******************************************************************************/
FlashType GetFlashSizeFromDriveType(DriveType drivetype)
{
   switch (drivetype)
   {
      case K1:case L1:
         return FlashSize640K;

      case L2:
         return FlashSize768K;

      case K2:case K350:case K352:case K353:case K354:case L251:
         return FlashSize1M;

      case D355:
      case K355:case K356:case K450:case K451:case L252:case L253:case L350:case L351:
      case K365:case K366:case K370:case K371:case K460:case K461:case K470:case K471:
         return FlashSize2M;

      case D570:case D57D:case D573:case D770:case D773:
      case L260:case L261:case L450:case L451:
      case Q175:case Q178:case G175:case G178:
      case Q630:case Q633:case G630:case G633:
      case Q910:case Q913:case Q915:case Q918:case G910:case G913:case G915:case G918:
      case Q960:case Q963:case Q965:case Q968:case G960:case G963:case G965:case G968:
        return FlashSize2MOpti;

      case Q200:case Q201:case Q203:case Q205:case Q206:case Q208:
      case G200:case G201:case G203:case G205:case G206:case G208:
      case Q590:case Q591:case Q593:case Q595:case Q596:case Q598:
      case G590:case G591:case G593:case G595:case G596:case G598:
      case Q670:case Q673:case Q675:case Q678:case G670:case G673:case G675:case G678:
         return FlashSize2MOpti2;

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
      case Q69A:case Q69B:case Q69D:case G69A:case G69B:case G69D:
      case Q97A:case Q97B:case Q97D:case G97A:case G97B:case G97D:
      case Q71A:case Q71B:case Q71D:case G71A:case G71B:case G71D:
      case Q72A:case Q72B:case Q72D:case G72A:case G72B:case G72D:
        return FlashSize2MOpti3;

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
         return FlashSize2MOpti4;

      case C500:case C505:case C600:case C605:
         return FlashSize2MOptiBD;

      case C64A: // C64A is currently unknown. Leave here to disable flashing for now
      case B530:case B535:case B74A:
         return FlashSize2MOptiBD2;

      case M110:
         return FlashSize2MHDDVD;

      default:
         return FlashSizeInvalid;
   }
}


/******************************************************************************\
| function: GetBootcodeSizeFromDriveType                                       |
|                                                                              |
| Purpose:                                                                     |
| Retrieve the size of a drive's boot code                                     |
|                                                                              |
| Input:                                                                       |
| drivetype -> Current drive                                                   |
|                                                                              |
| Returnvalues:                                                                |
| Size of the drive's boot code in bytes                                       |
\******************************************************************************/
u32 GetBootcodeSizeFromDriveType(DriveType drivetype)
{
   switch (GetFlashSizeFromDriveType(drivetype))
   {
   case FlashSize640K: case FlashSize768K: case FlashSize1M: case FlashSize2M:
   case FlashSize2MHDDVD:
      return 0x4000;

   case FlashSize2MOpti:case FlashSize2MOpti2:case FlashSize2MOpti3:case FlashSize2MOpti4:
      return 0x8000;

   case FlashSize2MOptiBD:case FlashSize2MOptiBD2: // Dumping not yet supported anyway
      return 0x6000;

   default:
      return FlashSizeInvalid;
   }
}


/******************************************************************************\
| function: GetCalibArea                                                       |
|                                                                              |
| Purpose:                                                                     |
| Determine where to retrieve calibration/settings data                        |
|                                                                              |
| Input:                                                                       |
| drivetype -> Current drive                                                   |
| area      -> memory area                                                     |
|                                                                              |
| Returnvalues:                                                                |
| start of settings area - 0 means not available                               |
\******************************************************************************/
u32 GetSettingsArea(DriveType drivetype,ConfigType area)
{
   const u32 offsets[][5]=
   { // CT_DRIVEID, CT_RPC, CT_RID, CT_BOOKTYPE_NEC, CT_BOOKTYPE_RICOH
      { 0x4500,  0x4f80,  0x4f00,  0x0000,  0x0000},
      { 0x4800,  0x5f80,  0x5f00,  0x5080,  0x5000},
      {0x10800, 0x20f80, 0x20f00, 0x20000, 0x20080},
      { 0x7000,  0x8f80,  0x8000,  0x8f50,  0x0000}, // TODO: Booktype area is yet unknown
      { 0x7000,  0x0000,  0x8b00,  0x0000,  0x0000}, // TODO: Booktype and RPC area are yet unknown
   };

   switch(drivetype)
   {
      case K1:case L1:case L2:
         return offsets[0][(int) area];

      case K2:case K350:case K352:case K353:case K354:case L251:
      case D355:
      case K355:case K356:case K450:case K451:case L252:case L253:case L350:case L351:
      case K365:case K366:case K370:case K371:case K460:case K461:case K470:case K471:
      case D570:case D57D:case D573:case D770:case D773:
      case L260:case L261:case L450:case L451:
      case Q175:case Q178:case G175:case G178:
      case Q630:case Q633:case G630:case G633:
      case Q910:case Q913:case Q915:case Q918:case G910:case G913:case G915:case G918:
      case Q960:case Q963:case Q965:case Q968:case G960:case G963:case G965:case G968:
      case Q200:case Q201:case Q203:case Q205:case Q206:case Q208:
      case G200:case G201:case G203:case G205:case G206:case G208:
      case Q590:case Q591:case Q593:case Q595:case Q596:case Q598:
      case G590:case G591:case G593:case G595:case G596:case G598:
      case Q670:case Q673:case Q675:case Q678:case G670:case G673:case G675:case G678:
      case M110:
         return offsets[1][(int) area];

      case Q230:case Q231:case Q233:case Q235:case Q236:case Q238:
      case G230:case G231:case G233:case G235:case G236:case G238:
      case Q240:case Q241:case Q243:case Q245:case Q246:case Q248:
      case G240:case G241:case G243:case G245:case G246:case G248:
      case G25A:case G25B:case G25D:
      case Q260:case Q261:case Q263:case Q265:case Q266:case Q268:
      case G260:case G261:case G263:case G265:case G266:case G268:
      case G27A:case G27B:case G27D:
      case Q280:case Q281:case Q283:case Q285:case Q286:case Q288:
      case G280:case G281:case G283:case G285:case G286:case G288:
      case G29A:case G29B:case G29D:
      case Q300:case Q301:case Q303:case Q305:case Q306:case Q308:
      case G300:case G301:case G303:case G305:case G306:case G308:
      case Q325:case Q326:case Q328:case G325:case G326:case G328:
      case Q700:case Q701:case Q703:case Q705:case Q706:case Q708:case Q70A:case Q70B:case Q70D:
      case G700:case G701:case G703:case G705:case G706:case G708:case G70A:case G70B:case G70D:
      case Q68A:case Q68B:case Q68D:
      case Q69A:case Q69B:case Q69D:case G69A:case G69B:case G69D:
      case Q97A:case Q97B:case Q97D:case G97A:case G97B:case G97D:
      case Q71A:case Q71B:case Q71D:case G71A:case G71B:case G71D:
      case Q72A:case Q72B:case Q72D:case G72A:case G72B:case G72D:
      case Q74A:case Q74B:case Q74D:case G74A:case G74B:case G74D:
      case Q76A:case Q76B:case Q76D:case G76A:case G76B:case G76D:
      case Q98A:case Q98B:case Q98D:case G98A:case G98B:case G98D:
      case Q93A:case Q93B:case Q93D:case G93A:case G93B:case G93D:
      case Q800:case Q801:case Q803:case Q805:case Q806:case Q808:case Q80A:case Q80B:case Q80D:
      case G800:case G801:case G803:case G805:case G806:case G808:case G80A:case G80B:case G80D:
         return offsets[2][(int) area];

      case C500:case C505:case C600:case C605:case C64A:
         return offsets[3][(int) area];

      case B530:case B535:case B74A:
         return offsets[4][(int) area];

      default:
         return 0;
   }
}


/******************************************************************************\
| function: GetBaseHardware                                                    |
|                                                                              |
| Purpose:                                                                     |
| Retrieve the lowest drive model with same hardware version                   |
|                                                                              |
| Input:                                                                       |
| drivetype -> Original drivetype                                              |
| extend -> 0->Do not change, 1->standard comparison, 2->extended conversion   |
|                                                                              |
| Returnvalues:                                                                |
| Drivetype with the same hardware but fewest features                         |
\******************************************************************************/
DriveType GetBaseHardware(DriveType orig,u8 extend)
{
   DriveType basehw=orig; // Initialisation not needed but compiler complains

   if (!extend)
      basehw=orig;
   else switch (orig)
   {
   case K354:
      if (extend==2)
         basehw=K353; // could not make it work although hardware should be the same
      break;
   case D355:case K356:case K450:case K451:
      basehw=K355; break;
   case K366:case K460:case K461:
      basehw=K365; break;
   case K371:case K470:case K471:
      basehw=K370; break;
   case L253:case L350:case L351:
      basehw=L252; break;
   case D57D:case D573:case D770:case D773:
      basehw=D570; break;
   case L261:case L450:case L451:
      basehw=L260; break;
   case Q178:case G175:case G178:
      basehw=Q175; break;
   case Q633:case G630:case G633:
      basehw=Q630; break;
   case Q201:case Q203:case G200:case G201:case G203:
      basehw=Q200; break;
   case Q206:case Q208:case G205:case G206:case G208:
      basehw=Q205; break;
   case Q231:case Q233:case G230:case G231:case G233:
      basehw=Q230; break;
   case Q236:case Q238:case G235:case G236:case G238:
      basehw=Q235; break;
   case Q241:case Q243:case G240:case G241:case G243:
      basehw=Q240; break;
   case Q246:case Q248:case G245:case G246:case G248:
      basehw=Q245; break;
   case Q260:case Q261:case Q263:case G260:case G261:case G263:
      if (extend==2)
         basehw=Q240;
      else
         basehw=Q260;
      break;
   case G25A:case G25B:case G25D:case Q265:case Q266:case Q268:case G265:case G266:case G268:
      if (extend==2)
         basehw=Q245;
      else
         basehw=Q265;
      break;
   case Q281:case Q283:case G280:case G281:case G283:
      basehw=Q280; break;
   case G27A:case G27B:case G27D:case Q286:case Q288:case G285:case G286:case G288:
      basehw=Q285; break;
   case Q300:case Q301:case Q303:case G300:case G301:case G303:
      if (extend==2)
         basehw=Q280;
      else
         basehw=Q300;
      break;
   case Q326:case Q328:case G325:case G326:case G328:
      basehw=Q325;
      break;
   case G29A:case G29B:case G29D:case Q305:case Q306:case Q308:case G305:case G306:case G308:
      if (extend==2)
         basehw=Q285;
      else
         basehw=Q305;
      break;
   case Q591:case Q593:case G590:case G591:case G593:
      basehw=Q590; break;
   case Q596:case Q598:case G595:case G596:case G598:
      basehw=Q595; break;
   case Q673:case G670:case G673:
      basehw=Q670; break;
   case Q678:case G675:case G678:
      basehw=Q675; break;
   case Q913:case G910:case G913:
      basehw=Q910; break;
   case Q918:case G915:case G918:
      basehw=Q915; break;
   case Q963:case G960:case G963:
      basehw=Q960; break;
   case Q968:case G965:case G968:
      basehw=Q965; break;
   case Q701:case Q703:case G700:case G701:case G703:
      basehw=Q700; break;
   case Q706:case Q708:case Q70A:case Q70B:case Q70D:case G705:case G706:case G708:case G70A:case G70B:case G70D:
      basehw=Q705; break;
   case Q801:case Q803:case G800:case G801:case G803:
      basehw=Q800; break;
   case Q806:case Q808:case Q80A:case Q80B:case Q80D:case G805:case G806:case G808:case G80A:case G80B:case G80D:
      basehw=Q805; break;
   case Q68B:case Q68D:
      basehw=Q68A; break;
   case Q69B:case Q69D:case G69A:case G69B:case G69D:
      basehw=Q69A; break;
   case Q97B:case Q97D:case G97A:case G97B:case G97D:
      basehw=Q97A; break;
   case Q71B:case Q71D:case G71A:case G71B:case G71D:
      basehw=Q71A; break;
   case Q72B:case Q72D:case G72A:case G72B:case G72D:
      basehw=Q72A; break;
   case Q74B:case Q74D:case G74A:case G74B:case G74D:
      basehw=Q74A; break;
   case Q76B:case Q76D:case G76A:case G76B:case G76D:
      basehw=Q76A; break;
   case Q98B:case Q98D:case G98A:case G98B:case G98D:
      basehw=Q98A; break;
   case Q93B:case Q93D:case G93A:case G93B:case G93D:
      basehw=Q93A; break;
   default:
      basehw=orig; break;
   }

   return basehw;
}


/******************************************************************************\
| function: CheckXFlash                                                        |
|                                                                              |
| Purpose:                                                                     |
| Check for crossflashed drives                                                |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| orig -> Supposed drivetype                                                   |
|                                                                              |
| Returnvalues:                                                                |
| real drivetype to prevent crossflashing if applicable                        |
\******************************************************************************/
DriveType CheckXFlash(Scsi* scsi, DriveType orig)
{
   u8 buffer[128];
   u8 Check=0;

   if (!ReadRAM(scsi, buffer, GetSettingsArea(orig,CT_RID), 0x80, 0))
      return DT_INVALID;
   if ((buffer[0x20]=='N') && (buffer[0x21]=='E') && (buffer[0x22]=='C'))
      Check=1;
   else if ((buffer[0x20]=='S') && (buffer[0x21]=='o') && (buffer[0x22]=='n') && (buffer[0x23]=='y'))
      Check=1;
   else if ((buffer[0x20]=='S') && (buffer[0x21]=='O') && (buffer[0x22]=='N') && (buffer[0x23]=='Y'))
      Check=1;
   else if ((buffer[0x20]=='A') && (buffer[0x21]=='T') && (buffer[0x22]=='A') && (buffer[0x23]=='P') && (buffer[0x24]=='I'))
      Check=2;
   else if ((buffer[0x20]=='P') && (buffer[0x21]=='L') && (buffer[0x22]=='D') && (buffer[0x23]=='S'))
      Check=3;

   if (!Check)
      return orig;

   if (Check==1)
   {
      if ((orig==K355) || (orig==K356) || (orig==K450) || (orig==K451))
      {
         if ((buffer[0x43]=='3') && (buffer[0x46]=='0'))
            return K355;
         else if ((buffer[0x43]=='3') && (buffer[0x46]=='1'))
            return K356;
         else if ((buffer[0x43]=='4') && (buffer[0x46]=='0'))
            return K450;
         else if ((buffer[0x43]=='4') && (buffer[0x46]=='1'))
            return K451;
         else
            return orig;
      }
      else if ((orig==L252) /*|| (orig==L253)*/ || (orig==L350) || (orig==L351))
      {
         if ((buffer[0x43]=='6') && (buffer[0x46]=='0'))
            return L252;
         else if ((buffer[0x43]=='6') && (buffer[0x46]=='1'))
            return L253;
         else if ((buffer[0x43]=='7') && (buffer[0x46]=='0'))
            return L350;
         else if ((buffer[0x43]=='7') && (buffer[0x46]=='1'))
            return L351;
         else
            return orig;
      }
      else if ((orig==K365) || (orig==K366) || (orig==K460) || (orig==K461))
      {
         if ((buffer[0x43]=='3') && (buffer[0x46]=='0'))
            return K365;
         else if ((buffer[0x43]=='3') && (buffer[0x46]=='1'))
            return K366;
         else if ((buffer[0x43]=='4') && (buffer[0x46]=='0'))
            return K460;
         else if ((buffer[0x43]=='4') && (buffer[0x46]=='1'))
            return K461;
         else
            return orig;
      }
      else if ((orig==K370) || (orig==K371) || (orig==K470) || (orig==K471))
      {
         if ((buffer[0x43]=='3') && (buffer[0x46]=='0'))
            return K370;
         else if ((buffer[0x43]=='3') && (buffer[0x46]=='1'))
            return K371;
         else if ((buffer[0x43]=='4') && (buffer[0x46]=='0'))
            return K470;
         else if ((buffer[0x43]=='4') && (buffer[0x46]=='1'))
            return K471;
         else
            return orig;
      }
      else if ((orig==D570) || (orig==D57D) || (orig==D573) || (orig==D770) || (orig==D773))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
         {
            if (buffer[0x57]=='L') // Seems to be used for duplicator drives
               return D57D;
            else
               return D570;
         }
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return D573;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return D770;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return D773;
         else
            return orig;
      }
      else if ((orig==L260) || (orig==L261) || (orig==L450) || (orig==L451))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return L260;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return L261;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return L450;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return L451;
         else
            return orig;
      }
      else if ((orig==Q175) || (orig==Q178) || (orig==G175) || (orig==G178))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q175;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q178;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G175;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G178;
         else
            return orig;
      }
      else if ((orig==Q630) || (orig==Q633) || (orig==G630) || (orig==G633))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q630;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q633;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G630;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G633;
         else
            return orig;
      }
      else if ((orig==Q200) || (orig==Q201) || (orig==Q203) || (orig==G200) || (orig==G201) || (orig==G203))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q200;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1'))
            return Q201;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q203;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G200;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1'))
            return G201;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G203;
         else
            return orig;
      }
      else if ((orig==Q205) || (orig==Q206) || (orig==Q208) || (orig==G205) || (orig==G206) || (orig==G208))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q205;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1'))
            return Q206;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q208;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G205;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1'))
            return G206;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G208;
         else
            return orig;
      }
      else if ((orig==Q230) || (orig==Q231) || (orig==Q233) || (orig==G230) || (orig==G231) || (orig==G233))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q230;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1'))
            return Q231;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q233;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G230;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1'))
            return G231;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G233;
         else
            return orig;
      }
      else if ((orig==Q235) || (orig==Q236) || (orig==Q238) || (orig==G235) || (orig==G236) || (orig==G238))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q235;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1'))
            return Q236;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q238;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G235;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1'))
            return G236;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G238;
         else
            return orig;
      }
      else if ((orig==Q240) || (orig==Q241) || (orig==Q243) || (orig==G240) || (orig==G241) || (orig==G243))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q240;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1'))
            return Q241;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q243;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G240;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1'))
            return G241;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G243;
         else
            return orig;
      }
      else if ((orig==Q245) || (orig==Q246) || (orig==Q248) || (orig==G245) || (orig==G246) || (orig==G248))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q245;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1'))
            return Q246;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q248;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G245;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1'))
            return G246;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G248;
         else
            return orig;
      }
      else if ((orig==Q260) || (orig==Q261) || (orig==Q263) || (orig==G260) || (orig==G261) || (orig==G263))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q260;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1'))
            return Q261;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q263;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G260;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1'))
            return G261;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G263;
         else
            return orig;
      }
      else if ((orig==Q265) || (orig==Q266) || (orig==Q268) || (orig==G265) || (orig==G266) || (orig==G268))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q265;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1'))
            return Q266;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q268;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G265;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1'))
            return G266;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G268;
         else
            return orig;
      }
#if 0 // 5280S and 7283S drives seem to identify as 7280S here
      else if ((orig==Q280) || (orig==Q281) || (orig==Q283) || (orig==G280) || (orig==G281) || (orig==G283))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q280;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1'))
            return Q281;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q283;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G280;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1'))
            return G281;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G283;
         else
            return orig;
      }
      else if ((orig==Q285) || (orig==Q286) || (orig==Q288) || (orig==G285) || (orig==G286) || (orig==G288))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q285;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1'))
            return Q286;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q288;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G285;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1'))
            return G286;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G288;
         else
            return orig;
      }
      else if ((orig==Q300) || (orig==Q301) || (orig==Q303) || (orig==G300) || (orig==G301) || (orig==G303))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q300;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1'))
            return Q301;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q303;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G300;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1'))
            return G301;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G303;
         else
            return orig;
      }
      else if ((orig==Q305) || (orig==Q306) || (orig==Q308) || (orig==G305) || (orig==G306) || (orig==G308))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q305;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1'))
            return Q306;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q308;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G305;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1'))
            return G306;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G308;
         else
            return orig;
      }
#endif
      else if ((orig==Q590) || (orig==Q591) || (orig==Q593) || (orig==G590) || (orig==G591) || (orig==G593))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q590;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1'))
            return Q591;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q593;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G590;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1'))
            return G591;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G593;
         else
            return orig;
      }
      else if ((orig==Q595) || (orig==Q596) || (orig==Q598) || (orig==G595) || (orig==G596) || (orig==G598))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q595;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1'))
            return Q596;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q598;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G595;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1'))
            return G596;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G598;
         else
            return orig;
      }
      else if ((orig==Q670) || (orig==Q673) || (orig==G670) || (orig==G673))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q670;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q673;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G670;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G673;
         else
            return orig;
      }
      else if ((orig==Q675) || (orig==Q678) || (orig==G675) || (orig==G678))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q675;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q678;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G675;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G678;
         else
            return orig;
      }
      else if ((orig==Q910) || (orig==Q913) || (orig==G910) || (orig==G913))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q910;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q913;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G910;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G913;
         else
            return orig;
      }
      else if ((orig==Q915) || (orig==Q918) || (orig==G915) || (orig==G918))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q915;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q918;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G915;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G918;
         else
            return orig;
      }
      else if ((orig==Q960) || (orig==Q963) || (orig==G960) || (orig==G963))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q960;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q963;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G960;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G963;
         else
            return orig;
      }
      else if ((orig==Q965) || (orig==Q968) || (orig==G965) || (orig==G968))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q965;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q968;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G965;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G968;
         else
            return orig;
      }
      else if ((orig==Q700) || (orig==Q701) || (orig==Q703) || (orig==G700) || (orig==G701) || (orig==G703))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0'))
            return Q700;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1'))
            return Q701;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3'))
            return Q703;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0'))
            return G700;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1'))
            return G701;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3'))
            return G703;
         else
            return orig;
      }
      else if ((orig==Q705) || (orig==Q706) || (orig==Q708) || (orig==Q70A) || (orig==Q70B) || (orig==Q70D) || (orig==G705) || (orig==G706) || (orig==G708) || (orig==G70A) || (orig==G70B) || (orig==G70D))
      {
         buffer[0x43]&=0xf;
         if ((buffer[0x43]==5) && (buffer[0x46]=='0')  && (buffer[0x47]=='S'))
            return Q705;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1') && (buffer[0x47]=='S'))
            return Q706;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3') && (buffer[0x47]=='S'))
            return Q708;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='0') && (buffer[0x47]=='H'))
            return Q70A;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='1') && (buffer[0x47]=='H'))
            return Q70B;
         else if ((buffer[0x43]==5) && (buffer[0x46]=='3') && (buffer[0x47]=='H'))
            return Q70D;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0') && (buffer[0x47]=='S'))
            return G705;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1') && (buffer[0x47]=='S'))
            return G706;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3') && (buffer[0x47]=='S'))
            return G708;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='0') && (buffer[0x47]=='H'))
            return G70A;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='1') && (buffer[0x47]=='H'))
            return G70B;
         else if ((buffer[0x43]==7) && (buffer[0x46]=='3') && (buffer[0x47]=='H'))
            return G70D;
         else
            return orig;
      }
   }
   else if (Check==2) // LiteOn OEMs
   {
      if ((orig==G245) || (orig==G246))
      {
         if (buffer[0x47]=='L') // Lightscribe drives
            return G246;
         else if (buffer[0x47]=='S') // Non-Lightscribe drives
            return G245;
         else
            return orig;
      }
   }

   return orig;
}


/******************************************************************************\
| function: SpecialChecks                                                      |
|                                                                              |
| Purpose:                                                                     |
| Check for special cases. Includes XFlash check and D355 check                |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| orig -> Supposed drivetype                                                   |
|                                                                              |
| Returnvalues:                                                                |
| real drivetype                                                               |
\******************************************************************************/
DriveType SpecialChecks(Scsi* scsi, DriveType orig)
{
   DriveType detected;
   
   if ((orig==DT_INVALID) || (orig==DT_UNKNOWN))
      return orig;

   detected=CheckXFlash(scsi,orig);

// Try a somewhat safe detection of D355 drives
   if (detected==K355)
   {
      bool mode=IsSafeMode(scsi);
      if (SetSafeMode(scsi,D355,false,true))
      {
         detected=D355;
         if (!mode)
            SetSafeMode(scsi,D355,false,false);
      }
   }
// Some K365 and K460 drives seem to have a wrong bootcode
   else if (detected==K365)
   {
      bool mode=IsSafeMode(scsi);
      if (SetSafeMode(scsi,K460,false,true))
      {
         detected=K460;
         if (!mode)
            SetSafeMode(scsi,K460,false,false);
      }
   }
   else if (detected==K460)
   {
      bool mode=IsSafeMode(scsi);
      if (SetSafeMode(scsi,K365,false,true))
      {
         detected=K365;
         if (!mode)
            SetSafeMode(scsi,K365,false,false);
      }
   }
// Try a somewhat safe detection of D57D drives
   else if (detected==D570)
   {
      bool mode=IsSafeMode(scsi);
      if (SetSafeMode(scsi,D57D,false,true))
      {
         detected=D57D;
         if (!mode)
            SetSafeMode(scsi,D57D,false,false);
      }
   }
   else if (detected==D57D)
   {
      bool mode=IsSafeMode(scsi);
      if (SetSafeMode(scsi,D570,false,true))
      {
         detected=D570;
         if (!mode)
            SetSafeMode(scsi,D570,false,false);
      }
   }

   return detected;
}


/******************************************************************************\
| function: IsLaptopDrive                                                      |
|                                                                              |
| Purpose:                                                                     |
| Determine whether drive is half height or slimline drive.                    |
| Only detects drives that require connection specific bootcode                |
|                                                                              |
| Input:                                                                       |
| dt -> drive type                                                             |
|                                                                              |
| Returnvalues:                                                                |
| true -> Drive is slimline drive                                              |
| false -> Drive is half height drive                                          |
\******************************************************************************/
bool IsLaptopDrive(DriveType dt)
{
   bool IsLaptop;

   switch (dt)
   {
   case L1: case L2: case L251: case L252: case L253: case L350: case L351:
   case L260: case L261:
      IsLaptop=true;break;

   default:
      IsLaptop=false;break;
   }

   return IsLaptop;
}


/******************************************************************************\
| Function: ReadBoot                                                           |
|                                                                              |
| Purpose:                                                                     |
| Read data from the drive's bootcode                                          |
| Only supported on V850 drives when in safe mode                              |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| buffer -> Buffer to store data (must be at least 16384 Bytes)                |
| bank -> Select which bank to read (not needed for drivetype>=2)              |
| size -> Size of bootcode                                                     |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Bootcode was read successfully. Data in buffer                      |
| false -> Bootcode could not be read                                          |
\******************************************************************************/
bool ReadBoot(Scsi* scsi,u8* buffer,bool bank,u32 size)
{
   memset(buffer,0,size);
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcd;
   
   if (bank)
      cdb[1]|=0x10;

   cdb[2]=0x52;cdb[3]=0x44;cdb[4]=0x42;cdb[5]=0x4f;cdb[6]=0x4f;cdb[7]=0x54; // RDBOOT

   if (scsiSay(scsi,(char *) cdb,12,(char *) buffer,size,X1_DATA_IN)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: ReadRAM                                                            |
|                                                                              |
| Purpose:                                                                     |
| Read data from the drive's RAM                                               |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| buffer -> Buffer to store data (must be big enough and a multiple of 4)      |
| address -> Source address used for reading                                   |
| length -> Number of Bytes to read should be 2^x with 0<=x<=15                |
| bank -> Select which bank to read (not needed for drivetype>=2)              |
| id -> Internal Firmware ID. Needed for 4570 and newer drives                 |
|                                                                              |
| Returnvalues:                                                                |
| true  -> RAM was read successfully. Data in buffer                           |
| false -> RAM could not be read                                               |
\******************************************************************************/
bool ReadRAM(Scsi* scsi,u8* buffer,u32 address,u16 length,bool bank, u8 id)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcc;
   
   if (bank)
      cdb[1]|=0x10;

   cdb[2]=(u8) (address>>24);
   cdb[3]=(u8) (address>>16);
   cdb[4]=(u8) (address>>8);
   cdb[5]=(u8) (address);

   cdb[7]=(u8) (length>>8);
   cdb[8]=(u8) (length);

   if (id)
   {
      cdb[9]=0x52; // 'R'
      cdb[10]=0x44; // 'D'
      cdb[11]=id;
   }

   length=(length+3) & 0xfffc;
   if (scsiSay(scsi,(char *) cdb,12,(char *) buffer,length,X1_DATA_IN)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: WriteRAM                                                           |
|                                                                              |
| Purpose:                                                                     |
| Write data to the drive's RAM                                                |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| buffer -> Buffer to send to drive                                            |
| address -> Target address used for writing                                   |
| length -> Number of Bytes to write*0x1000 (should not exceed 8)              |
| bank -> Select which bank to write (not needed for drivetype>=2)             |
| area -> Select area for writing (bootcode, firmware...) (only on 2MB drives) |
|                                                                              |
| Returnvalues:                                                                |
| true  -> RAM was written successfully                                        |
| false -> RAM could not be written                                            |
\******************************************************************************/
bool WriteRAM(Scsi* scsi,u8* buffer,u32 address,u8 length,bool bank,u8 area)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcb;cdb[1]=0x01;
   
   if (bank)
      cdb[1]|=0x10;

   cdb[9]=area;

   cdb[2]=(u8) (address>>24);
   cdb[3]=(u8) (address>>16);
   cdb[4]=(u8) (address>>8);
   cdb[5]=(u8) (address);

   cdb[8]=length;

   if (scsiSay(scsi,(char *) cdb,12,(char *) buffer,length*0x1000,X2_DATA_OUT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: WriteRAMOpti                                                       |
|                                                                              |
| Purpose:                                                                     |
| Write data to the drive's RAM for Optiarc drives                             |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| buffer -> Buffer to send to drive                                            |
| address -> Target address used for writing                                   |
| length -> Number of Bytes to write*0x1000 (should not exceed 8)              |
| bank -> Select which bank to write (not needed for drivetype>=2)             |
| area -> Select area for writing (bootcode, firmware...) (only on 2MB drives) |
|                                                                              |
| Returnvalues:                                                                |
| true  -> RAM was written successfully                                        |
| false -> RAM could not be written                                            |
\******************************************************************************/
bool WriteRAMOpti(Scsi* scsi,u8* buffer,u32 address,u8 length,bool bank,u8 area)
{
   bool rc;
   u8* CopyBuffer;

// This is required for working around a bootcode bug in the AD-7170A and compatible drives
   CopyBuffer=new u8[length*0x1004];
   if (!CopyBuffer)
      return false;
   memcpy(CopyBuffer,buffer,length*0x1000);
   memset(CopyBuffer+length*0x1000,0,length*4);

   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcb;cdb[1]=0x01;
   
   if (bank)
      cdb[1]|=0x10;

   cdb[9]=area;

   cdb[2]=(u8) (address>>24);
   cdb[3]=(u8) (address>>16);
   cdb[4]=(u8) (address>>8);
   cdb[5]=(u8) (address);

   cdb[8]=length;

   if (scsiSay(scsi,(char *) cdb,12,(char *) CopyBuffer,length*0x1004,X2_DATA_OUT)<0)
      rc=false;
   else
      rc=true;

   delete[] CopyBuffer;

   return rc;
}


/******************************************************************************\
| Function: WriteRAMOpti3                                                      |
|                                                                              |
| Purpose:                                                                     |
| Write data to the drive's RAM (Optiarc AD-7240S series)                      |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| buffer -> Buffer to send to drive                                            |
| address -> Target address used for writing                                   |
| length -> Number of Bytes to write*0x1000 (should not exceed 8)              |
| bank -> Select which bank to write (not needed for drivetype>=2)             |
| area -> Select area for writing (bootcode, firmware...) (only on 2MB drives) |
| calcchecksum -> Calculate 16 bit checksum for buffer to be flashed           |
|                                                                              |
| Returnvalues:                                                                |
| true  -> RAM was written successfully                                        |
| false -> RAM could not be written                                            |
\******************************************************************************/
bool WriteRAMOpti3(Scsi* scsi,u8* buffer,u32 address,u32 length,bool bank,u8 area,bool calcchecksum)
{
   u16 checksum=0;
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcb;cdb[1]=0x01;
   
   if (bank)
      cdb[1]|=0x10;

   cdb[9]=area;

   cdb[2]=(u8) (address>>24);
   cdb[3]=(u8) (address>>16);
   cdb[4]=(u8) (address>>8);
   cdb[5]=(u8) (address);

   cdb[6]=(u8) (length>>16);
   cdb[7]=(u8) (length>>8);
   cdb[8]=(u8) (length);

   if (calcchecksum)
   {
      for (u32 i=0;i<length;i+=2)
         checksum+=(((u16) buffer[i])<<8)+buffer[i+1];

      cdb[10]=(u8) (checksum>>8);
      cdb[11]=(u8) (checksum);
   }

   if (scsiSay(scsi,(char *) cdb,12,(char *) buffer,length,X2_DATA_OUT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: WriteCalib                                                         |
|                                                                              |
| Purpose:                                                                     |
| Write data to the drive's calibration area                                   |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| buffer -> Buffer to send to drive                                            |
| address -> Target address used for writing                                   |
| length -> Number of Bytes to write                                           |
| bank -> Select which bank to write (not needed for drivetype>=2)             |
|                                                                              |
| Returnvalues:                                                                |
| true  -> data was written successfully                                       |
| false -> data could not be written                                           |
\******************************************************************************/
bool WriteCalib(Scsi* scsi,u8* buffer,u32 address,u16 length,bool bank)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcb;
   
   if (bank)
      cdb[1]|=0x10;

   cdb[3]=(u8) (address>>16);
   cdb[4]=(u8) (address>>8);
   cdb[5]=(u8) (address);

   cdb[7]=(u8) (length>>8);
   cdb[8]=(u8) (length);

   if (scsiSay(scsi,(char *) cdb,12,(char *) buffer,length,X2_DATA_OUT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: IsSafeMode                                                         |
|                                                                              |
| Purpose:                                                                     |
| Check if the drive is currently in safe mode                                 |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
|                                                                              |
| Returnvalues:                                                                |
| true  -> drive is in safe mode                                               |
| false -> drive is not in safe mode                                           |
\******************************************************************************/
bool IsSafeMode(Scsi* scsi)
{
   u8 buffer[0x60];

   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0x12;cdb[4]=0x60;

   if (scsiSay(scsi,(char *) cdb,6,(char*) buffer,0x60,X1_DATA_IN)<0)
      return false;

// Check for firmware version starting with 0
   if (buffer[32]=='0')
      return true;
   
   return false;
}


/******************************************************************************\
| Function: SetSafeMode                                                        |
|                                                                              |
| Purpose:                                                                     |
| Sets the drive to safe or normal mode                                        |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| drivetype -> Exact drivetype to flash                                        |
| bank -> Select which bank to read (not needed for drivetype>=2)              |
| enable -> Enable or disable safe mode                                        |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Safe mode was successfully changed                                  |
| false -> Safe mode could not be changed                                      |
\******************************************************************************/
bool SetSafeMode(Scsi* scsi,DriveType drivetype,bool bank,bool enable)
{
   char buffer[4];

   if (drivetype<0) // Invalid or unknown
      return false;

   memcpy(buffer,GetDriveID(drivetype),4);

   return SetSafeMode(scsi,buffer,bank,enable);
}


/******************************************************************************\
| Function: SetSafeMode                                                        |
|                                                                              |
| Purpose:                                                                     |
| Sets the drive to safe or normal mode                                        |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| drivetype -> Exact drivetype to flash                                        |
| bank -> Select which bank to read (not needed for drivetype>=2)              |
| enable -> Enable or disable safe mode                                        |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Safe mode was successfully changed                                  |
| false -> Safe mode could not be changed                                      |
\******************************************************************************/
bool SetSafeMode(Scsi* scsi,char* drivetype,bool bank,bool enable)
{
   bool rc;

   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcc;cdb[1]=0x01;
   
   if (bank)
      cdb[1]|=0x10;

   if (enable)
      cdb[2]=0x01;

   cdb[8]=1; // Only required since FlashSize2MOpti3 but also works on other drives

   memcpy(cdb+4,drivetype,4);

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      rc=false;
   else
      rc=true;

#if 0
// This area might be required. Some drives need a while to react properly
// after disabling safe mode. Workaround for now is to first ReadRAM and then
// switch to safe mode when dumping firmwares
   if (!enable)
   {
      u8 sense[0x10];
      memset(cdb,0,6);
      while (scsiSay(scsi,(char*) cdb,6,0,0,X0_DATA_NOT)<0)
      {
         if (scsiGetSense(scsi,(char*) sense,0x10,0)<14)
            break;
         if ((sense[12]!=0x04) && (sense[12]!=0x28)) // drive becoming ready / medium changed. Not a very precise check but should be sufficient
               return true;
      }
   }
#endif

   return rc;
}


/******************************************************************************\
| Function: SendChecksum                                                       |
|                                                                              |
| Purpose:                                                                     |
| Submit Checksum to drive                                                     |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| index -> Select index used to store checksum (will be multiplied with 4)     |
| bank -> Select which bank to use                                             |
| checksum -> Checksum to send to drive                                        |
| area -> Select area for writing (bootcode, firmware...) (only on 2MB drives) |
| bd -> Unknown, only found on BluRay drives. Set to 1 after flashing          |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Checksum was submitted successfully                                 |
| false -> Checksum could not be submitted                                     |
\******************************************************************************/
bool SendChecksum(Scsi* scsi,bool bank, u8 index, u16 checksum, u8 area, u8 bd)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcc;cdb[1]=0x04;

   if (bank)
      cdb[1]|=0x10;

   cdb[2]=(u8) (checksum>>8);
   cdb[3]=(u8) (checksum);

   cdb[7]=bd;
   cdb[8]=0x04*index;
   cdb[9]=area;

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: SendChecksumOpti4                                                  |
|                                                                              |
| Purpose:                                                                     |
| Submit Checksum to drive                                                     |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| bank -> Select which bank to use                                             |
| checksum -> Checksum to send to drive                                        |
| length -> Just a guess. Was set to 0x1b0000 so far. Not found to be used     |
| area -> Select area for writing (bootcode, firmware...)                      |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Checksum was submitted successfully                                 |
| false -> Checksum could not be submitted                                     |
\******************************************************************************/
bool SendChecksumOpti4(Scsi* scsi,bool bank, u16 checksum, u32 length, u8 area)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcc;cdb[1]=0x06;

   if (bank)
      cdb[1]|=0x10;

   cdb[2]=(u8) (checksum>>8);
   cdb[3]=(u8) (checksum);

   cdb[5]=(u8) (length>>24);
   cdb[6]=(u8) (length>>16);
   cdb[7]=(u8) (length>>8);
   cdb[8]=(u8) (length);

// 01 -> 0x000000-0x007ffd (ignores checksum, takes u16 value from 0x007ffe)
// 02 -> 0x010000-0x010fff
// 03 -> 0x020000-0x020fff
// 04 -> 0x030000-0x1dffff
// 05 -> 0x000000-0x007ffd
   cdb[9]=area;

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: EraseBank                                                          |
|                                                                              |
| Purpose:                                                                     |
| Erase a complete memory bank. Only supported on 1300/5500 series             |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| bank -> Select which bank to erase                                           |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Bank was erased successfully                                        |
| false -> Drive returned an error while erasing                               |
\******************************************************************************/
bool EraseBank(Scsi* scsi,bool bank)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcb;cdb[1]=0x08;
   if (bank)
      cdb[1]|=0x10;

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: EraseSingleBlock                                                   |
|                                                                              |
| Purpose:                                                                     |
| scsi -> SCSI handler                                                         |
| Erase a block in flash memory                                                |
|                                                                              |
| Input:                                                                       |
| bank -> Select which bank to erase                                           |
| blocknr -> Select which block to erase (0-55)                                |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Block was erased successfully                                       |
| false -> Drive returned an error while erasing                               |
\******************************************************************************/
bool EraseSingleBlock(Scsi* scsi,bool bank, int block)
{
   u8 pattern;
   u8 position;

   if ((block<0) || (block>55))
      return false;

   pattern=1<<((u8)(block&7));
   position=8-(u8)(block>>3);

   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcb;cdb[1]=0x04;
   if (bank)
      cdb[1]|=0x10;

   cdb[position]=pattern;

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: FlashSingleBlock                                                   |
|                                                                              |
| Purpose:                                                                     |
| Write a block to flash memory                                                |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| bank -> Select which bank to flash                                           |
| blocknr -> Select which block to flash (0-55)                                |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Block was flashed successfully                                      |
| false -> Drive returned an error while flashing                              |
\******************************************************************************/
bool FlashSingleBlock(Scsi* scsi,bool bank,int block)
{
   u8 pattern;
   u8 position;

   if ((block<0) || (block>55))
      return false;

   pattern=1<<((u8)(block&7));
   position=8-(u8)(block>>3);

   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcb;cdb[1]=0x02;
   if (bank)
      cdb[1]|=0x10;

   cdb[position]=pattern;

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: EraseBlock                                                         |
|                                                                              |
| Purpose:                                                                     |
| Erase given blocks / single Block on Optiarc x20x drives                     |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| bank -> Select which bank to erase                                           |
| block -> block to erase                                                      |
| area -> Area to flash. 0 Firmware, 1 Bootcode. Should be 0                   |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Blocks were erased successfully                                     |
| false -> An error occured while erasing the blocks                           |
\******************************************************************************/
bool EraseBlock(Scsi* scsi,bool bank,u8 block,u8 area)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcb;cdb[1]=0x04;
   if (bank)
      cdb[1]|=0x10;

   cdb[8]=block;
   cdb[9]=area;

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: FlashBlock                                                         |
|                                                                              |
| Purpose:                                                                     |
| Flash given blocks                                                           |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| bank -> Select which bank to flash                                           |
| patternX -> pattern for cdb[X]. Decides which areas need to be flashed       |
| area -> Area to flash. 0 Firmware, 1 Bootcode                                |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Blocks were flashed successfully                                    |
| false -> An error occured while flashing the blocks                          |
\******************************************************************************/
bool FlashBlock(Scsi* scsi,bool bank,u8 block,u8 area)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcb;cdb[1]=0x02;
   if (bank)
      cdb[1]|=0x10;

   cdb[8]=block;
   cdb[9]=area;

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: EraseBlockOptiarc                                                  |
|                                                                              |
| Purpose:                                                                     |
| Erase single Block on drives with FlashSize2MOpti2                           |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| bank -> Select which bank to flash                                           |
| block -> block to be erased. Does not need area or confirmed firmware        |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Blocks were erased successfully                                     |
| false -> An error occured while erasing the blocks                           |
\******************************************************************************/
bool EraseBlockOptiarc(Scsi* scsi,bool bank,u8 block)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcb;cdb[1]=0x05;
   if (bank)
      cdb[1]|=0x10;

   cdb[2]=0x45; // E
   cdb[3]=0x52; // R
   cdb[4]=0x41; // A
   cdb[5]=0x53; // S
   cdb[6]=0x45; // E
   cdb[8]=block;

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: EraseRegion                                                        |
|                                                                              |
| Purpose:                                                                     |
| Erase given region                                                           |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| bank -> Select which bank to erase                                           |
| address -> Start address                                                     |
| length -> Size - Should be 0x10000                                           |
| area -> Area to erase. 0 Firmware, 1 Bootcode. Should be 0                   |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Regions was erased successfully                                     |
| false -> An error occured while erasing the region                           |
\******************************************************************************/
bool EraseRegion(Scsi* scsi,bool bank,u32 address,u32 length,u8 area)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcb;cdb[1]=0x04;
   if (bank)
      cdb[1]|=0x10;

   cdb[2]=(u8) (address>>24);
   cdb[3]=(u8) (address>>16);
   cdb[4]=(u8) (address>>8);
   cdb[5]=(u8) (address);

   cdb[6]=(u8) (length>>16);
   cdb[7]=(u8) (length>>8);
   cdb[8]=(u8) (length);

   cdb[9]=area;

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: FlashRegion                                                        |
|                                                                              |
| Purpose:                                                                     |
| Flash given region                                                           |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| bank -> Select which bank to flash                                           |
| address -> Start address                                                     |
| length -> Size - Should be 0x10000                                           |
| area -> Area to erase. 0 Firmware, 1 Bootcode                                |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Region was flashed successfully                                     |
| false -> An error occured while flashing the region                          |
\******************************************************************************/
bool FlashRegion(Scsi* scsi,bool bank,u32 address,u32 length,u8 area)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcb;cdb[1]=0x02;
   if (bank)
      cdb[1]|=0x10;

   cdb[2]=(u8) (address>>24);
   cdb[3]=(u8) (address>>16);
   cdb[4]=(u8) (address>>8);
   cdb[5]=(u8) (address);

   cdb[6]=(u8) (length>>16);
   cdb[7]=(u8) (length>>8);
   cdb[8]=(u8) (length);

   cdb[9]=area;

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: EraseRegionOptiarc                                                 |
|                                                                              |
| Purpose:                                                                     |
| Erase region on drives with FlashSize2MOpti3 and 4                           |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| bank -> Select which bank to erase                                           |
| address -> Start address                                                     |
| length -> Size - Should be 0x10000                                           |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Region was erased successfully                                      |
| false -> An error occured while erasing the region                           |
\******************************************************************************/
bool EraseRegionOptiarc(Scsi* scsi,bool bank,u32 address,u32 length)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcb;cdb[1]=0x05;
   if (bank)
      cdb[1]|=0x10;

   cdb[2]=(u8) (address>>24);
   cdb[3]=(u8) (address>>16);
   cdb[4]=(u8) (address>>8);
   cdb[5]=(u8) (address);

   cdb[6]=(u8) (length>>16);
   cdb[7]=(u8) (length>>8);
   cdb[8]=(u8) (length);

   cdb[10]=0x45; // E
   cdb[11]=0x52; // R

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: UnlockBootflash                                                    |
|                                                                              |
| Purpose:                                                                     |
| Allow flashing the bootcode on new Optiarc drives (FlashSize2MOpti2 and 3)   |
| Optional 0x202c0 (calib: 0x52c0) can be set to 0x00 for permanent setting    |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Opened drive successfully for bootcode flashing                     |
| false -> Failed opening the drive for bootcode flashing                      |
\******************************************************************************/
bool UnlockBootflash(Scsi* scsi)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0x4a; // GET EVENT STATUS NOTIFICATION
   cdb[1]=0x01; // Polled data
   cdb[4]=0x20; // Notification Class

   if (scsiSay(scsi,(char *) cdb,10,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: GetStrategyVersions                                                |
|                                                                              |
| Purpose:                                                                     |
| Retrieve the strategy versions                                               |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| buffer -> Buffer for strategy data                                           |
| buffersize -> size of buffer                                                 |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Strategy versions retrieved successfully                            |
| false -> An error occured when retrieving the strategy versions              |
\******************************************************************************/
bool GetStrategyVersions(Scsi* scsi,u8* buffer, u32 buffersize)
{
   memset(buffer,0,buffersize);
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xcf;cdb[1]=0x03;

   if (scsiSay(scsi,(char *) cdb,12,(char *) buffer,buffersize,X1_DATA_IN)<0)
      return false;

   return true;
}



/******************************************************************************\
| Function: GetBooktypeSettingRicoh                                            |
|                                                                              |
| Purpose:                                                                     |
| Get the current value for the default booktype (Ricoh-style)                 |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| type -> Select which mediatype to retrieve                                   |
|                                                                              |
| Returnvalues:                                                                |
| Stored booktype setting                                                      |
| -1 -> Error retrieving setting                                               |
\******************************************************************************/
int GetBooktypeSettingRicoh(Scsi* scsi,Mediatype type)
{
   u8 buffer[16];

   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xfa;
   cdb[8]=0x10;

   if (type==MT_PlusR)
      cdb[1]=0x10;
   else if (type!=MT_PlusRW)
      return -1;

   if (scsiSay(scsi,(char *) cdb,12,(char*) buffer,16,X1_DATA_IN)<0)
      return -1;

   return (int) buffer[4];
}


/******************************************************************************\
| Function: SetBooktypeRicoh                                                   |
|                                                                              |
| Purpose:                                                                     |
| Change the stored default booktype value (Ricoh-style)                       |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| mediatype -> Select which mediatype to change                                |
| booktype -> Select which booktype to use                                     |
|                                                                              |
| Returnvalues:                                                                |
| true -> The setting was applied                                              |
| false -> The setting could not be applied                                    |
\******************************************************************************/
bool SetBooktypeRicoh(Scsi* scsi,Mediatype mediatype,Mediatype booktype)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xf9;cdb[2]=(u8)booktype;cdb[3]=0xff;

   if (mediatype==MT_PlusR)
      cdb[1]=0x14;
   else if (mediatype==MT_PlusRW)
      cdb[1]=0x0c;
   else
      return false;

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: SetBooktypeRWRicoh                                                 |
|                                                                              |
| Purpose:                                                                     |
| Change the booktype of a currently inserted DVD+RW (Ricoh-style)             |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| booktype -> Select which booktype to set                                     |
|                                                                              |
| Returnvalues:                                                                |
| true -> The setting was applied                                              |
| false -> The setting could not be applied                                    |
\******************************************************************************/
bool SetBooktypeRWRicoh(Scsi* scsi, Mediatype booktype)
{
   u8 buffer[8];

   memset(cdb,0,CDB_SIZE);
   cdb[0]=0x46;cdb[1]=0x02;cdb[8]=0x08;

   if (scsiSay(scsi,(char*) cdb,10,(char*) buffer,8,X1_DATA_IN)<0)
      return false;

// No DVD+RW inserted
   if (buffer[7]!=0x1a)
      return false;

   memset(cdb,0,CDB_SIZE);
   cdb[0]=0xf9;cdb[1]=0x02;cdb[2]=(u8)booktype;cdb[3]=0xff;

   if (scsiSay(scsi,(char*) cdb,12,(char*) buffer,8,X1_DATA_IN)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: GetBooktypeSettingNEC                                              |
|                                                                              |
| Purpose:                                                                     |
| Get the current value for the default booktype (NEC-style)                   |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| type -> Select which mediatype to retrieve                                   |
| temp -> Determine whether to retrieve temporary or permanent settings        |
| result -> Result structure                                                   |
|                                                                              |
| Returnvalues:                                                                |
| false -> Could not determine settings                                        |
| true -> Settings are stored in result                                        |
\******************************************************************************/
bool GetBooktypeSettingNEC(Scsi* scsi,Mediatype type,bool temp,BookNECResult* result)
{
   u8 buffer[6];

   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xc9;cdb[1]=0x02;cdb[3]=0x01;
   switch (type)
   {
   case MT_PlusR:
      break;
   case MT_PlusRW:
      cdb[2]=1;
      break;
   case MT_PlusR9:
      cdb[2]=2;
      break;
//   case MT_PlusRW9:
//      cdb[2]=3;
//      break;
   default:
      return false;
   }
   cdb[4]=0x42;cdb[5]=0x54;cdb[6]=0x59;cdb[7]=0x50;cdb[8]=0x45; // BTYPE
   
   if (temp)
      cdb[9]=0x01;

   if (scsiSay(scsi,(char *) cdb,12,(char*)buffer,6,X1_DATA_IN)<0)
      return false;

   result->mediatype=buffer[0];
   result->booktype=buffer[1];
   result->changesleft=((buffer[2] & 0x7f) << 8)+buffer[3];
   result->version=(buffer[4] << 8)+buffer[5];
   result->usetemp=(buffer[2] & 0x80)?true:false;

   return true;
}


/******************************************************************************\
| Function: ClearBooktypeSettingsNEC                                           |
|                                                                              |
| Purpose:                                                                     |
| Reset the booktype counter                                                   |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
|                                                                              |
| Returnvalues:                                                                |
| false -> Could not reset counter                                             |
| true -> Counter reset was successfully sent                                  |
\******************************************************************************/
bool ClearBooktypeSettingsNEC(Scsi* scsi)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xc9;cdb[1]=0x0c;
   cdb[4]=0x42;cdb[5]=0x54;cdb[6]=0x59;cdb[7]=0x50;cdb[8]=0x45; // BTYPE

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: SetBooktypeNEC                                                     |
|                                                                              |
| Purpose:                                                                     |
| Change the stored default booktype value (NEC-style)                         |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| mediatype -> Select which mediatype to change                                |
| booktype -> Select which booktype to use                                     |
| temp -> Select whether to change temporary or permanent setting              |
|                                                                              |
| Returnvalues:                                                                |
| true -> The setting was applied                                              |
| false -> The setting could not be applied                                    |
\******************************************************************************/
bool SetBooktypeNEC(Scsi* scsi,Mediatype mediatype,Mediatype booktype,bool temp)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xc9;cdb[3]=(u8)booktype;
   cdb[4]=0x42;cdb[5]=0x54;cdb[6]=0x59;cdb[7]=0x50;cdb[8]=0x45; // BTYPE

   if (temp)
      cdb[1]=0x03;
   else
      cdb[1]=0x01;

   switch (mediatype)
   {
   case MT_PlusR:
      break;
   case MT_PlusRW:
      cdb[2]=1;
      break;
   case MT_PlusR9:
      cdb[2]=2;
      break;
//   case MT_PlusRW9:
//      cdb[2]=3;
//      break;
   default:
      return false;
   }

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: SelectTempBooktypeNEC                                              |
|                                                                              |
| Purpose:                                                                     |
| Switch between permanent and temporary settings (NEC-style)                  |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| mediatype -> Select which mediatype to change                                |
| temp -> Select wheter to use permanent or temporary settings                 |
|                                                                              |
| Returnvalues:                                                                |
| true -> The setting was applied                                              |
| false -> The setting could not be applied                                    |
\******************************************************************************/
bool SelectTempBooktypeNEC(Scsi* scsi,Mediatype mediatype,bool temp)
{
   memset(cdb,0x00,CDB_SIZE);
   cdb[0]=0xc9;cdb[1]=0x04;
   cdb[4]=0x42;cdb[5]=0x54;cdb[6]=0x59;cdb[7]=0x50;cdb[8]=0x45; // BTYPE

   if (temp)
      cdb[9]=0x01;

   switch (mediatype)
   {
   case MT_PlusR:
      break;
   case MT_PlusRW:
      cdb[2]=1;
      break;
   case MT_PlusR9:
      cdb[2]=2;
      break;
//   case MT_PlusRW9:
//      cdb[2]=3;
//      break;
   default:
      return false;
   }

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: RPCVendorReset                                                     |
|                                                                              |
| Purpose:                                                                     |
| Performs a region code vendor reset                                          |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Vendor reset successful                                             |
| false -> Vendor reset failed                                                 |
\******************************************************************************/
bool RPCVendorReset(Scsi* scsi)
{
   memset(cdb,0x00,CDB_SIZE);

   cdb[0]=0xcb; // May be 0xcc as well
   cdb[6]=0x52;  // 'R'
   cdb[7]=0x45;  // 'E'
   cdb[8]=0x47;  // 'G'
   cdb[9]=0x49;  // 'I'
   cdb[10]=0x4f; // 'O'
   cdb[11]=0x4e; // 'N'

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   return true;
}


/******************************************************************************\
| Function: RPCGlobalReset                                                     |
|                                                                              |
| Purpose:                                                                     |
| Completely resets region code information                                    |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| drivetype -> Exact drivetype to flash                                        |
| debug -> Only execute first command. Do not actually reset                   |
|                                                                              |
| Returnvalues:                                                                |
| true  -> RPC Reset successful                                                |
| false -> RPC reset failed                                                    |
\******************************************************************************/
bool RPCGlobalReset(Scsi* scsi,DriveType drivetype,bool debug)
{
   u8 buffer[4];

   if (drivetype<0) // Invalid or unknown
      return false;

   memcpy(buffer,(u8*) GetDriveID(drivetype),4);

   return RPCGlobalReset(scsi,buffer,debug);
}


/******************************************************************************\
| Function: RPCGlobalReset                                                     |
|                                                                              |
| Purpose:                                                                     |
| Performs a region code vendor reset                                          |
|                                                                              |
| Input:                                                                       |
| scsi -> SCSI handler                                                         |
| drivetype -> Exact drivetype to flash                                        |
| debug -> Only execute first command. Do not actually reset                   |
|                                                                              |
| Returnvalues:                                                                |
| true  -> Vendor reset successful                                             |
| false -> Vendor reset failed                                                 |
\******************************************************************************/
bool RPCGlobalReset(Scsi* scsi,u8* drivetype,bool debug)
{
   memset(cdb,0x00,CDB_SIZE);

   cdb[0]=0xcb; // May be 0xcc as well
   cdb[6]=0x55+drivetype[0];
   cdb[7]=0x55+drivetype[1];
   cdb[8]=0x55+drivetype[2];
   cdb[9]=0x55+drivetype[3];
   cdb[10]=0x55;
   cdb[11]=0x55;

   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false;

   if (debug)
      return true;

   cdb[0]=0xcb; // May be 0xcc as well
   cdb[6]=0xaa-drivetype[0];
   cdb[7]=0xaa-drivetype[1];
   cdb[8]=0xaa-drivetype[2];
   cdb[9]=0xaa-drivetype[3];
   cdb[10]=0xaa;
   cdb[11]=0xaa;
   if (scsiSay(scsi,(char *) cdb,12,0,0,X0_DATA_NOT)<0)
      return false; // Should be rather unlikely

   return true;
}
