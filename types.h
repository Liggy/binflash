// Universal type definitions
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;

#define CDB_SIZE 16

//Enable for detailled output by default
//#define __SPECIAL__

#if (_WIN32 || __MSDOS__)
#define stricmp stricmp
#else
#define stricmp strcasecmp
#endif

enum ErrorCode{
   E_OK=0, E_ILLEGAL_CALL=1, E_REFUSED_DISCLAIMER=2, E_ACCESSFIRMFILE=3, E_NOMEM=4, E_CANCELLED=5,
   E_SCSINEW=10, E_SCSIOPEN=11, E_NODRIVETYPE=12, E_UNSUPPORTEDDRIVE=13,
   E_DUMPNOSUPPORT=21, E_DUMPERROR=22,
   E_FLASHNOSUPPORT=31, E_FLASHERROR=32, E_FIRMNOMATCH=33, E_NOFIRMWARE=34,
   E_BOOKNOTSUPPORTED=41, E_BOOKTYPEMEDIA=42, E_BOOKINVALIDNAME=43,
   E_IDENTIFYBOOTKEY=51, E_IDENTIFYMEM=52,
   E_RPC1=61, E_RPCNOVENDOR=62, E_RPCNOTNEEDED=63
};

// Different errors during flashing
enum FlashingError{
   FE_OK=0, FE_ENTER_SAFEMODE=1, FE_SEND_FLASH=2, FE_SEND_CHECKSUM=3,
   FE_ERASE_BLOCK=4, FE_FLASH_BLOCK=5, FE_ERASE_BANK=6, FE_LEAVE_SAFEMODE=7,
   FE_UNSUPPORTED=8, FE_OTHER=9,
};

// Different message types
enum MessageType {
   fatal=-1, error=-2, warning=-3,
   numsteps=-10, info=0, verbose=1, talky=2,
   step=1000, yesno=1001, okcancel=1002, fwinfo=1003
};

// Different booktypes
enum Mediatype {
   MT_error=-1, MT_ROM=0x01, MT_RAM=0x10, 
   MT_MinusR=0x20,	MT_MinusRW=0x32,
   MT_PlusRW=0x92, MT_PlusR=0xa1, MT_PlusRW9=0xd1, MT_PlusR9=0xe1,
   MT_CDROM=0x100, MT_CDR=0x101, MT_CDRW=0x102
};

// Different firmware compatible drives
enum DriveType {
   DT_UNKNOWN=-2,
   DT_INVALID=-1,
   K1=0,       // ND-1x00A
   L1=1,       // ND-5x00A
   L2=2,       // ND-6x00A/ND-6450A
   K2=3,       // ND-2xx0A
   K350=4,     // ND-3x00A/ND3450A
   K352=5,     // ND-3520A
   K353=6,     // ND-3530A
   K354=7,     // ND-3540A
   L251=8,     // ND-6650A
   D355=9,     // ND-3550A
   K355=10,    // ND-3550A
   K356=11,    // ND-3551A
   K450=12,    // ND-4550A
   K451=13,    // ND-4551A
   L252=14,    // ND-6750A
   L253=15,    // ND-6751A
//TODO: Find correct value for ND-6751A. Let's assume L253
   L350=16,    // ND-7550A
   L351=17,    // ND-7551A
   K365=18,    // ND-3650A
   K366=19,    // ND-3651A
   K370=20,    // ND-3570A
   K371=21,    // ND-3571A
   K460=22,    // ND-4650A
   K461=23,    // ND-4651A
   K470=24,    // ND-4570A
   K471=25,    // ND-4571A
   D570=26,    // AD-5170A
   D57D=27,    // AD-5170A for Duplicators
   D573=28,    // AD-5173A
   D770=29,    // AD-7170A
   D773=30,    // AD-7173A
   L260=31,    // AD-5540A
   L261=32,    // AD-5543A
   L450=33,    // AD-7540A
   L451=34,    // AD-7543A
   Q175=35,    // AD-5170S
   Q178=36,    // AD-5173S
   G175=37,    // AD-7170S
   G178=38,    // AD-7173S
   M110=39,    // HR-1100A
   Q630=40,    // AD-5630A
   Q633=41,    // AD-5633A
   G630=42,    // AD-7630A
   G633=43,    // AD-7633A
   Q200=44,    // AD-5200A
   Q201=45,    // AD-5201A
   Q203=46,    // AD-5203A
   Q205=47,    // AD-5200S
   Q206=48,    // AD-5201S
   Q208=49,    // AD-5203S
   G200=50,    // AD-7200A
   G201=51,    // AD-7201A
   G203=52,    // AD-7203A
   G205=53,    // AD-7200S
   G206=54,    // AD-7201S
   G208=55,    // AD-7203S
   Q230=56,    // AD-5230A
   Q231=57,    // AD-5231A
   Q233=58,    // AD-5233A
   Q235=59,    // AD-5230S
   Q236=60,    // AD-5231S
   Q238=61,    // AD-5233S
   G230=62,    // AD-7230A
   G231=63,    // AD-7231A
   G233=64,    // AD-7233A
   G235=65,    // AD-7230S
   G236=66,    // AD-7231S
   G238=67,    // AD-7233S
   Q240=68,    // AD-5240A
   Q241=69,    // AD-5241A
   Q243=70,    // AD-5243A
   Q245=71,    // AD-5240S
   Q246=72,    // AD-5241S
   Q248=73,    // AD-5243S
   G240=74,    // AD-7240A
   G241=75,    // AD-7241A
   G243=76,    // AD-7243A
   G245=77,    // AD-7240S
   G246=78,    // AD-7241S
   G248=79,    // AD-7243S
   Q260=80,    // AD-5260A
   Q261=81,    // AD-5261A
   Q263=82,    // AD-5263A
   Q265=83,    // AD-5260S
   Q266=84,    // AD-5261S
   Q268=85,    // AD-5263S
   G260=86,    // AD-7260A
   G261=87,    // AD-7261A
   G263=88,    // AD-7263A
   G265=89,    // AD-7260S
   G266=90,    // AD-7261S
   G268=91,    // AD-7263S
   Q590=92,    // AD-5590A
   Q591=93,    // AD-5591A
   Q593=94,    // AD-5593A
   Q595=95,    // AD-5590S
   Q596=96,    // AD-5591S
   Q598=97,    // AD-5593S
   G590=98,    // AD-7590A
   G591=99,    // AD-7591A
   G593=100,   // AD-7593A
   G595=101,   // AD-7590S
   G596=102,   // AD-7591S
   G598=103,   // AD-7593S
   Q670=104,   // AD-5670A
   Q673=105,   // AD-5673A
   Q675=106,   // AD-5670S
   Q678=107,   // AD-5673S
   G670=108,   // AD-7670A
   G673=109,   // AD-7673A
   G675=110,   // AD-7670S
   G678=111,   // AD-7673S
   Q910=112,   // AD-5910A
   Q913=113,   // AD-5913A
   Q915=114,   // AD-5910S
   Q918=115,   // AD-5913S
   G910=116,   // AD-7910A
   G913=117,   // AD-7913A
   G915=118,   // AD-7910S
   G918=119,   // AD-7913S
   Q960=120,   // AD-5960A
   Q963=121,   // AD-5963A
   Q965=122,   // AD-5960S
   Q968=123,   // AD-5963S
   G960=124,   // AD-7960A
   G963=125,   // AD-7963A
   G965=126,   // AD-7960S
   G968=127,   // AD-7963S
   Q700=128,   // AD-5700A
   Q701=129,   // AD-5701A
   Q703=130,   // AD-5703A
   Q705=131,   // AD-5700S
   Q706=132,   // AD-5701S
   Q708=133,   // AD-5703S
   Q70A=134,   // AD-5700H
   Q70B=135,   // AD-5701H
   Q70D=136,   // AD-5703H - unconfirmed
   G700=137,   // AD-7700A
   G701=138,   // AD-7701A
   G703=139,   // AD-7703A
   G705=140,   // AD-7700S
   G706=141,   // AD-7701S
   G708=142,   // AD-7703S
   G70A=143,   // AD-7700H
   G70B=144,   // AD-7701H
   G70D=145,   // AD-7703H
   Q68A=146,   // AD-5680H
   Q68B=147,   // AD-5681H
   Q68D=148,   // AD-5683H
   Q97A=149,   // AD-5970H
   Q97B=150,   // AD-5971H
   Q97D=151,   // AD-5973H
   G97A=152,   // AD-7970H
   G97B=153,   // AD-7971H
   G97D=154,   // AD-7971H
   Q280=155,   // AD-5280A
   Q281=156,   // AD-5281A
   Q283=157,   // AD-5283A
   Q285=158,   // AD-5280S
   Q286=159,   // AD-5281S
   Q288=160,   // AD-5283S
   G280=161,   // AD-7280A
   G281=162,   // AD-7281A
   G283=163,   // AD-7283A
   G285=164,   // AD-7280S
   G286=165,   // AD-7281S
   G288=166,   // AD-7283S
   Q69A=167,   // AD-5690H
   Q69B=168,   // AD-5691H
   Q69D=169,   // AD-5693H
   G69A=170,   // AD-7690H
   G69B=171,   // AD-7691H
   G69D=172,   // AD-7693H
   Q71A=173,   // AD-5710H
   Q71B=174,   // AD-5711H
   Q71D=175,   // AD-5713H
   G71A=176,   // AD-7710H
   G71B=177,   // AD-7711H
   G71D=178,   // AD-7713H
   Q300=179,   // AD-5300A
   Q301=180,   // AD-5301A
   Q303=181,   // AD-5303A
   Q305=182,   // AD-5300S
   Q306=183,   // AD-5301S
   Q308=184,   // AD-5303S
   G300=185,   // AD-7300A
   G301=186,   // AD-7301A
   G303=187,   // AD-7303A
   G305=188,   // AD-7300S - confirmed and identifies as AD-7280S
   G306=189,   // AD-7301S
   G308=190,   // AD-7303S
   Q800=191,   // AD-5800A
   Q801=192,   // AD-5801A
   Q803=193,   // AD-5803A
   Q805=194,   // AD-5800S
   Q806=195,   // AD-5801S
   Q808=196,   // AD-5803S
   Q80A=197,   // AD-5800H
   Q80B=198,   // AD-5801H
   Q80D=199,   // AD-5803H
   G800=200,   // AD-7800A
   G801=201,   // AD-7801A
   G803=202,   // AD-7803A
   G805=203,   // AD-7800S
   G806=204,   // AD-7801S
   G808=205,   // AD-7803S
   G80A=206,   // AD-7800H
   G80B=207,   // AD-7801H
   G80D=208,   // AD-7803H
   G25A=209,   // AD-7250H
   G25B=210,   // AD-7251H
   G25D=211,   // AD-7253H
   G27A=212,   // AD-7270H
   G27B=213,   // AD-7271H
   G27D=214,   // AD-7273H
   G29A=215,   // AD-7290H
   G29B=216,   // AD-7291H
   G29D=217,   // AD-7293H
   Q74A=218,   // AD-5740H
   Q74B=219,   // AD-5741H
   Q74D=220,   // AD-5743H
   G74A=221,   // AD-7740H
   G74B=222,   // AD-7741H
   G74D=223,   // AD-7743H
   Q76A=224,   // AD-5760H
   Q76B=225,   // AD-5761H
   Q76D=226,   // AD-5763H
   G76A=227,   // AD-7760H
   G76B=228,   // AD-7761H
   G76D=229,   // AD-7763H
   Q93A=230,   // AD-5930H
   Q93B=231,   // AD-5931H
   Q93D=232,   // AD-5933H
   G93A=233,   // AD-7930H
   G93B=234,   // AD-7931H
   G93D=235,   // AD-7933H
   Q325=236,   // AD-5320S
   Q326=237,   // AD-5321S
   Q328=238,   // AD-5323S
   G325=239,   // AD-7320S - confirmed and identifies as LiteOn iHAS124W
   G326=240,   // AD-7321S
   G328=241,   // AD-7323S
   Q72A=242,   // AD-5720H
   Q72B=243,   // AD-5721H
   Q72D=244,   // AD-5723H
   G72A=245,   // AD-7720H - identifies as AD-7717H
   G72B=246,   // AD-7721H
   G72D=247,   // AD-7723H
   Q98A=248,   // AD-5790H
   Q98B=249,   // AD-5791H
   Q98D=250,   // AD-5793H
   G98A=251,   // AD-7790H
   G98B=252,   // AD-7791H
   G98D=253,   // AD-7793H

   C500=1001,  // BC-5500A
   C505=1002,  // BC-5500S
   C600=1003,  // BC-5600A
   C605=1004,  // BC-5600S
   C64A=1005,  // BC-5640H

   B530=1006,  // BD-5730A - probably does not exist
   B535=1007,  // BD-5730S
   B74A=1008,  // BD-5740H
};

enum CompatibleLaptop {
   L_INVALID =-1,
   L1Master  =0,
   L1Slave   =1,
   L2Master  =2,
   L2Slave   =3,
   L251Master=4,
   L251Slave =5,
   L252Master=6,
   L252Slave =7,
   L253Master=8,
   L253Slave =9,
   L350Master=10,
   L350Slave =11,
   L351Master=12,
   L351Slave =13,
   L260Master=14,
   L260Slave =15,
   L261Master=16,
   L261Slave =17,
};

enum FlashType {
   FlashSizeInvalid,
   FlashSize640K,
   FlashSize768K,
   FlashSize1M,
   FlashSize2M,
   FlashSize2MOpti,
   FlashSize2MOpti2,
   FlashSize2MOpti3,
   FlashSize2MOpti4,
   FlashSize2MHDDVD,
   FlashSize2MOptiBD,
   FlashSize2MOptiBD2,
};

enum ConfigType {
   CT_DRIVEID=0,        // Drive ID
   CT_RPC=1,            // RPC-Data
   CT_RID=2,            // RID-Data
   CT_BOOKTYPE_NEC=3,   // Booktype Data
   CT_BOOKTYPE_RICOH=4, // Booktype Data Ricoh commands
};

struct XFlashCalib
{
   u8 ID[4]; // like K210/K350/L351
   u8 Drive[4]; // like 6500/1300/3540
   u8 RID[4]; // like NDxx (@0x4f10/0x5f10)
};

struct BootflashAppend
{
   u8 Firmware[32768];
   XFlashCalib CalibData;
   u32 checksum;
};
