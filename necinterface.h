struct BooktypeInfo
{
   Mediatype mediatype;
   Mediatype booktype;
   u16 changesleft;
   u16 version;
   bool usetemp;
   bool ricoh;
};

class NECDRIVE : virtual public DRIVE
{
private:
   static const u32 BufferNeeded=0x8000;

   bool FirmwareToFile(Scsi*,FILE*,u32,u32,bool,u16*,u8=0);
   DriveType IdentifyDrive(Scsi*);
   DriveType GetDriveTypeFromBuffer(u8*,size_t,u16*);
   int GetStepCount(DriveType,bool);
   u16 RetrieveBootcodeVersion(Scsi* scsi, DriveType drivetype);

   FlashingError Flash640K(Scsi*, u8*, DriveType);
   FlashingError Flash768K(Scsi*, u8*, DriveType);
   FlashingError Flash1M(Scsi*, u8*, DriveType);
   FlashingError Flash2M(Scsi*, u8*, DriveType);
   FlashingError Flash2MOpti(Scsi*, u8*, DriveType);
   FlashingError Flash2MOpti2(Scsi*, u8*, DriveType);
   FlashingError Flash2MOpti3(Scsi*, u8*, DriveType);
   FlashingError Flash2MOpti4(Scsi*, u8*, DriveType);
   FlashingError Flash2MOptiBD(Scsi*, u8*, DriveType);
   FlashingError Flash2MOptiBD2(Scsi*, u8*, DriveType);

   ErrorCode DoRegionReset(Scsi*,bool);

public:
// Constructor & Destructor
   NECDRIVE(CallbackPrototype=NULL);
   ~NECDRIVE();

// Interface information
   const char* GetInterfaceVersion() const;
   const char* GetInternalVersion() const;
   const char* GetManufacturer() const;

// Drive capabilities
   const bool CanDump() const;
   const bool CanFlash() const;
   const bool CanBitset() const;
   const bool NeedDriveTypeDump() const;
   const bool NeedDriveTypeFlash() const;

// The real work
   u8* ReadFirmware(char*,u32*,DriveType,u16);
   bool IdentifyFirmware(u8*,size_t,Inquiry*);
   ErrorCode FlashFirmware(char*,char*);
   ErrorCode DumpFirmware(char*,char*);
   ErrorCode IdentifyNewDrive(char*,char*);

   ErrorCode SetBooktype(char*,char*,char*,char*);
   ErrorCode SetBooktypeRW(char*,char*);
   ErrorCode SetBooktype(char*,Mediatype,Mediatype,bool);
   ErrorCode SetBooktypeRW(char*,Mediatype);
   ErrorCode ClearBTSettings(char*);
   ErrorCode QueryBooktype(char*);
   ErrorCode QuerySingleBooktype(char*,Mediatype,bool,BooktypeInfo*);

   ErrorCode ResetRegion(char*,char*,bool);
};
