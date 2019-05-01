struct BookNECResult
{
   u8 mediatype;
   u8 booktype;
   u16 changesleft;
   u16 version;
   bool usetemp;
};

const char* GetInternalVersion();

const char* GetDriveID(DriveType);
DriveType IdentifyDrive(Scsi*,u8* buffer=0);
u8 GetIDFromDriveType(Scsi*,DriveType);
u8 GetIDFromDrive(Scsi*,DriveType);
u8 GetIDFromBuffer(u8*,DriveType);
CompatibleLaptop GetCompatibleLaptop(u8);
u16 GetLaptopBootcode(CompatibleLaptop);
FlashType GetFlashSizeFromDriveType(DriveType);
u32 GetBootcodeSizeFromDriveType(DriveType);
DriveType GetBaseHardware(DriveType,u8 extend);
DriveType CheckXFlash(Scsi*, DriveType);
DriveType SpecialChecks(Scsi*, DriveType);
bool IsLaptopDrive(DriveType);
u32 GetSettingsArea(DriveType,ConfigType area);

bool ReadBoot(Scsi*,u8*,bool,u32);
bool ReadRAM(Scsi*,u8*,u32,u16,bool,u8=0);
bool WriteRAM(Scsi*,u8*,u32,u8,bool,u8=0);
bool WriteRAMOpti(Scsi*,u8*,u32,u8,bool,u8=0);
bool WriteRAMOpti3(Scsi* scsi,u8* buffer,u32 address,u32 length,bool bank,u8 area,bool calcchecksum=false);
bool WriteCalib(Scsi*,u8*,u32,u16,bool);

bool IsSafeMode(Scsi*);
bool SetSafeMode(Scsi*,DriveType,bool,bool);
bool SetSafeMode(Scsi*,char*,bool,bool);

bool SendChecksum(Scsi* scsi,bool bank, u8 index, u16 checksum, u8 area=0,u8 bd=0);
bool SendChecksumOpti4(Scsi* scsi,bool bank, u16 checksum, u32 length, u8 area);

bool EraseBank(Scsi*,bool);
bool EraseSingleBlock(Scsi* scsi,bool,int);
bool FlashSingleBlock(Scsi* scsi,bool,int);
bool EraseBlock(Scsi* scsi,bool bank,u8 block,u8 area=0);
bool FlashBlock(Scsi* scsi,bool bank,u8 block,u8 area=0);
bool EraseBlockOptiarc(Scsi* scsi,bool bank,u8 block);
bool EraseRegion(Scsi* scsi,bool bank,u32 address,u32 length,u8 area=0);
bool FlashRegion(Scsi* scsi,bool bank,u32 address,u32 length,u8 area=0);
bool EraseRegionOptiarc(Scsi* scsi,bool bank,u32 address,u32 length);
bool UnlockBootflash(Scsi*);

bool GetStrategyVersions(Scsi*,u8*,u32);

int GetBooktypeSettingRicoh(Scsi*,Mediatype);
bool SetBooktypeRicoh(Scsi*,Mediatype,Mediatype);
bool SetBooktypeRWRicoh(Scsi*,Mediatype);

bool GetBooktypeSettingNEC(Scsi*,Mediatype,bool,BookNECResult*);
bool SetBooktypeNEC(Scsi*,Mediatype,Mediatype,bool);
bool ClearBooktypeSettingsNEC(Scsi*);

bool SelectTempBooktypeNEC(Scsi*,Mediatype,bool);

bool RPCVendorReset(Scsi*);
bool RPCGlobalReset(Scsi*,DriveType,bool);
bool RPCGlobalReset(Scsi*,u8*,bool);
