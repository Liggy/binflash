// Information received after inquiry
struct Inquiry
{
   char vendor[9];
   char identification[17];
   char revision[5];
   char specific[21];

   u8 devicetype;
};


struct RegionInfo
{
   bool RPC2;
   u8 UserChanges;
   u8 VendorChanges;
   u8 Region;
};


// For Callback functions
typedef int (*CallbackPrototype)(MessageType, const char*, ...);
typedef void (*EnumPrototype)(const char*,const Inquiry*);


class DRIVE
{
private:
   u8 cdb[CDB_SIZE];

protected:
#ifdef _DEBUG
   bool debug;
#endif

   DRIVE();
   virtual ~DRIVE();

   CallbackPrototype callback;
   void SetCallback(CallbackPrototype=NULL);
   Scsi* OpenDevice(char*);
   Mediatype GetMediatypeFromString(char*);
   const char* GetMediatypeAsString(Mediatype);

protected:
   bool GetInquiry(Scsi*, Inquiry *driveinfo);
   bool IsUnitReady(Scsi*);

public:
   void SetDebug(bool);
   bool init();	// initialize SCSI interface
   const char* GetDriveVersion() const;
   int GetDiscBooktype(Scsi*);
   int GetDisctype(Scsi*);
   ErrorCode GetRegion(char*,RegionInfo* =NULL);

   virtual DriveType IdentifyDrive(Scsi*)=0;
   ErrorCode ScanForDevices(bool,EnumPrototype=NULL);
};
