/*
**  stuc.cpp
**  Monday. May 24, 2004
**
**  Talk thru a SCSITaskUserClient to a device.
**
**/

/*
** Version History
** 0.0                  Initial private incomplete release.
** 0.1  22-MAY-2004     Added wildcard matching. USB hang corrected.
** 0.2  23-MAY-2004     Enhanced port, all functions implemented.
** 0.3  24-MAY-2004     Multiple bug fixes.
** 0.4  25-MAY-2004     Authoring device are unmounted.
** 0.5  26-MAY-2004     Allow for path specifications (e.g. /Volumes/SomeCD or
**                        /dev/disk0)
** 0.6  29-MAY-2004     Corrected unmounting too much volumes.
** 0.7  05-JUN-2004     Corrected stucSay() when direction is X0_DATA_NOT.
** 
*/

/*
** Specifying devices to be opened
**
** This can be done either by the device inquiry (vendor, product, revision) or
**   by giving a path to any file on the device.
**
** For path-based specifications:
**      2 characters (optional) to select the Nth matching device ("A:" to "Z:")
**      2 characters (optional) to prevent unmounting devices ("+:")
**      n characters for a path (the leading '/' is mandatory) of any file
**        on the device.
** in the above order.
** In this case, the "A:" to "Z:" is intended e.g. to select between members of
**   a RAID disk set. This was only partially tested though, as there's no SCSI
**   passthru to disks.
** If the path describes a device (e.g. /dev/disk1), this device is opened. Note
**   that, again, A: to Z: prefix may be used to select between members of a
**   RAID disk set.
**
** For inquiry-based specifications:
**      2 characters (optional) to select the Nth matching device ("A:" to "Z:")
**      2 characters (optional) to prevent unmounting devices ("+:")
**      8 characters for the vendor identification,
**      16 characters for the product identification,
**      4 characters for the product revision level
** in the above order.
** 
** The resulting string can be truncated to any length. Truncated characters are
**   handled as always matching. The string can be truncated up to the empty
**   string, but then, all characters being handled as matching, all devices
**   match.
** 
** Any character specified as '?' is handled as always matching.
** 
** Optionnally, this device string can be prefixed by a case insensitive letter
**   (A to Z) followed by a colon.
**   This allows selecting among identical devices.
**   The letter A stands for the first matching device, B for the second etc.
** 
** Optionally, the second prefix, an optional '+', is used to instruct PLScsi
**   to try to unmount any volumes that would prevent opening the device. Of
**   course, if one of these volumes can't be unmounted, PLScsi won't be able to
**   access the device. You may try this option mounting a CD then comparing:
**             "plscsi -w"  and  "plscsi -w +:"
** 
** Truncating the identification string to an empty string, and prefixing it
**   with the "A:" to "Z:" can be used to approximatively "mimic" a Windows
**   drive naming convention. "A:" will be the first drive that can be handled
**   by PLScsi, "B:" will be the second etc.
** 
** Note that MacOS X has no way to predict or enforce device ordering, it may
**   vary with devices being (un)plugged in any order, accross reboots etc,
**   so using A: to Z: must be used with EXTREME CAUTION accross successive
**   plscsi invocations.
** 
** Example 1:
**      "B:+:PIONEER DVD-RW  DVR-107D1.13"
**      (#:+:<vendor><product ident.><fw>)
** This will match the second PIONEER DVR-107 drive whose firmware is 1.13.
** No PIONEER DVR-107 with a firmware that is not 1.13 will be matched.
** If any volumes is mounted from this device, the device won't be opened.
** 
** Example 2:
**      "PIONEER DVD-RW  DVR-10?"
**      (<vendor><product ident.><fw>)
** This will match the first PIONEER DVR-103, 104, 105 etc drive.
** 
** Example 3:
**      "/Volumes/SomeCD"
** This will match the volume that is mounted on /Volumes/SomeCD, while ALL
**   volumes mounted from this device will be unmounted.
** 
** Example 4:
**      "/dev/disk5"
** This will match the physical device that includes /dev/disk5
** 
** PLScsi on MacOS X supports ATAPI, USB 1 & 2, FireWire 400 & 800, parallel
** SCSI. SATA has not been tested, but it should work, as should every
** transport that supports sending SCSI CDB in MacOS X.
** 
** Due to MacOS X limitations, only "authoring devices" and devices not handled
** by OS X can be matched. This means that e.g. CDÐROM drives with no write
** capability can NOT be accessed through PLScsi on MacOS X.
** Read Apple's documentation for more information about this limitation.
**
** As far as I've tested, there are some case where unmounting volumes that are
** in the process of being mounted will hang the process. Further invocations
** of PLScsi can also hang when trying to match that device.
*/

#include <limits.h> /* INT_MAX ... */
#include <string.h> /* strlcat ... */
#include <ctype.h> /* isdigit ... */
#include <stdio.h> /* FILE */

#include "plscsi.h"

#ifdef STUC

#include <sys/mount.h> /* getmntinfo ... */
#include <sys/syslimits.h> /* PATH_MAX ... */

#include <IOKit/IOKitLib.h> /* IOKit */
#include <IOKit/IOCFPlugIn.h> /* IOCFPlugInInterface ... */
#include <IOKit/IOBSD.h> /* kIOBSDNameKey ... */
#include <CoreServices/CoreServices.h>

#if 0 // 0 to compile on OS X < 10.3, 1 else.
#include <IOKit/cdb/IOSCSILib.h>
#include <IOKit/cdb/IOCDBLib.h>
#include <IOKit/scsi-commands/SCSITaskLib.h>
#else // >= 10.3
#include <IOKit/scsi/SCSITaskLib.h>
#endif


// this is the private STUC structure to handle all requests
typedef struct Stuc {
  unsigned long Timeout;
  int SenseLength;
  UInt32 ActualCount;
  FILE *ErrorFile;
  SCSITaskInterface **Task;
  SCSITaskDeviceInterface **Device;
  int SenseValid;
  UInt8 Sense[256];
  char Name[PATH_MAX + 16];
  };

// private prototypes
static bool matchFromInquiry(const char *inq, const char *name, int size);
static bool match(Stuc *stuc, const char *name);
static UInt32 DoCommand(Stuc *stuc, UInt8 *cdb, unsigned long cdblength, int direction, UInt8 *buffer, unsigned long bufferlength, unsigned long timeout, UInt32 *actuallength);
static long toMilliseconds(int s, int ns);
static void TryToUnmountAllPartitions(io_service_t object);
static int stucOpenByDevice(Stuc *stuc, char const *name, int count, int eject);
static int stucOpenByPath(Stuc *stuc, char const *name, int count, int eject);
static int stucOpenByInquiry(Stuc *stuc, char const *name, int count, int eject);


/*
** Try to unmount all volumes mounted from the device specified by 'object'
**
** No return, failure to unmount are discarded.
**/

void TryToUnmountAllPartitions(io_service_t object)
{
struct statfs *fsStats=NULL;
unsigned int numMounts=0;
unsigned int index=0;
OSStatus error=noErr;
FSRef volFSRef={{0}};
FSCatalogInfo volumeInfo={0};
CFStringRef bsdName;
char strBSD[PATH_MAX];
size_t strBSDLength;
CFStringEncoding encoding = CFStringGetSystemEncoding();

// Get the BSD name for the object
bsdName = (CFStringRef) IORegistryEntrySearchCFProperty(object, kIOServicePlane, CFSTR(kIOBSDNameKey), kCFAllocatorDefault, kIORegistryIterateRecursively);

if (bsdName)
  {
  if (CFStringGetCString(bsdName, strBSD, sizeof(strBSD), encoding) && (strBSDLength = strlen(strBSD)))
    {
    // Get the mount info for all the mountpoints. We will traverse that data
    // and try and find any mountpoints which have the diskX identifier in them.
    numMounts = getmntinfo(&fsStats, MNT_NOWAIT);
    if (numMounts)
      {
      for (index = 0; index < numMounts; index++)
        {
        // Compare the BSD dev node to the one we have. Avoid matching disk1 and disk10!
        if (!memcmp(strBSD, &fsStats[index].f_mntfromname[5], strBSDLength) && (!isdigit(fsStats[index].f_mntfromname[5+strBSDLength]))) /* Starting at [5] gets rid of the /dev/ */
          {
          // Ask Carbon to unmount the partition.
          error = FSPathMakeRef((UInt8 *) fsStats[index].f_mntonname, &volFSRef, NULL);
          if (error == noErr)
            {
            error = FSGetCatalogInfo(&volFSRef, kFSCatInfoVolume, &volumeInfo, NULL, NULL, NULL);
            if (error == noErr) error = FSUnmountVolumeSync(volumeInfo.volume, 0, NULL);
            }
          }
        }
      }
    }
  }
}

/*
** Check if user specified name matches device's inquiry
** Handles wildcard. Case insensitive.
**
** return 0 is inquiry does not match request, non-zero if it matches
**/

bool matchFromInquiry(const char *inq, const char *name, int size)
{
int i;

if (size < 8) return 0;
inq += 8;
size -= 8;

i = strlen(name)-1;
if (i > (size-1)) i = size-1;

for (; i >= 0; i--)
  {
  switch(name[i])
    {
  case '?':  // ever matching
    break;
  default:
    if (toupper(name[i]) != toupper(inq[i])) // case insensitive
      return 0;
    break;
    }
  }
return 1;
}

/*
** Check if the device in the stuc->Task matches user request
**
** return 0 is inquiry does not match request, non-zero if it matches
**/

bool match(Stuc *stuc, const char *name)
{
UInt32 sns;
UInt32 actualCount;
UInt8 cdb[6];
UInt8 buf[96]; // must be <= 255 

if ((!name) || (name[0] == 0)) return 1; // name is empty, we match everything

cdb[0] = 0x12;
cdb[1] = 0x00;
cdb[2] = 0x00;
cdb[3] = 0x00;
cdb[4] = sizeof(buf);
cdb[5] = 0x00;

sns = DoCommand(stuc, cdb, 6, X1_DATA_IN, buf, sizeof(buf), stuc->Timeout, &actualCount);

if (sns) return 0;

return matchFromInquiry((char *) buf, name, actualCount);
}

/*
** Send a CDB and optionally handle the data in/out phase.
**
** Return 0 if success, or a sense summary (if applicable), or 0xFFFFFFFF for any other error
**/

UInt32 DoCommand(Stuc *stuc, UInt8 *cdb, unsigned long cdblength, int direction, UInt8 *buffer, unsigned long bufferlength, unsigned long timeout, UInt32 *actuallength)
{
IOVirtualRange range;
IOReturn ior;
SCSIServiceResponse ssr;
SCSITaskStatus taskstatus;
int DataTransferDirection;
UInt64 count;

if (!stuc) return 0xffffffff;
if (!stuc->Task) return 0xffffffff;

switch(direction)
  {
default: 
case X0_DATA_NOT:
  DataTransferDirection = kSCSIDataTransfer_NoDataTransfer;
  bufferlength = 0;
  break;
case X1_DATA_IN:
  DataTransferDirection = kSCSIDataTransfer_FromTargetToInitiator;
  break;
case X2_DATA_OUT:
  DataTransferDirection = kSCSIDataTransfer_FromInitiatorToTarget;
  break;
  }

range.address = (IOVirtualAddress) buffer;
range.length = bufferlength;

ior = (*stuc->Task)->SetCommandDescriptorBlock(stuc->Task, cdb, cdblength);
if (ior != kIOReturnSuccess) return 0xFFFFFFFF;

ior = (*stuc->Task)->SetScatterGatherEntries(stuc->Task, &range, 1, bufferlength, DataTransferDirection);
if (ior != kIOReturnSuccess) return 0xFFFFFFFF;

ior = (*stuc->Task)->SetTimeoutDuration(stuc->Task, timeout ? timeout : 10000);
if (ior != kIOReturnSuccess) return 0xFFFFFFFF;

stuc->SenseValid = 0;
ior = (*stuc->Task)->SetAutoSenseDataBuffer(stuc->Task, (SCSI_Sense_Data *) stuc->Sense, stuc->SenseLength);
if (ior != kIOReturnSuccess) return 0xFFFFFFFF;

if (actuallength) *actuallength = 0;
ior = (*stuc->Task)->ExecuteTaskSync(stuc->Task, (SCSI_Sense_Data *) stuc->Sense, &taskstatus, &count);
if (ior != kIOReturnSuccess) return 0xFFFFFFFF;

ior = (*stuc->Task)->GetSCSIServiceResponse(stuc->Task, &ssr);
if (ior != kIOReturnSuccess) return 0xFFFFFFFF;

if ( (ssr != kSCSIServiceResponse_TASK_COMPLETE) &&
     (ssr != kSCSIServiceResponse_LINK_COMMAND_COMPLETE) &&
     (ssr != kSCSIServiceResponse_FUNCTION_COMPLETE) ) return 0xFFFFFFFF;

if (actuallength) *actuallength = count & 0xFFFFFFFF;
if (taskstatus == kSCSITaskStatus_CHECK_CONDITION)
  {
  stuc->SenseValid = 1;
  return ( ((* (SCSI_Sense_Data *) stuc->Sense).SENSE_KEY & 0x0F) << 16 |
           ((* (SCSI_Sense_Data *) stuc->Sense).ADDITIONAL_SENSE_CODE) <<  8 |
           ((* (SCSI_Sense_Data *) stuc->Sense).ADDITIONAL_SENSE_CODE_QUALIFIER) );
  }

return 0x00000000;
}



/*
**  Construct a new Stuc connection.
**
**  Return null to fail, else don't.
**/

Stuc *newStuc(void)
{
Stuc *x;

x = (Stuc *) malloc(sizeof(Stuc));
if (!x) return x;

x->Task = NULL;
x->Device = NULL;
x->Timeout = 30000; // 30 s
x->SenseValid = 0;
x->SenseLength = sizeof(SCSI_Sense_Data);
x->ErrorFile = stderr;
x->ActualCount = 0;
x->Name[0] = 0;

return x;
}

/*
**  Open this Stuc connection indicated by a /dev/... path.
**
**  Return zero to succeed, else don't.
**/

int stucOpenByDevice(Stuc *stuc, char const *name, int count, int eject)
{
IOReturn ior;
CFMutableDictionaryRef dictionary;
mach_port_t master;
io_iterator_t iterator1;
io_object_t object1;
io_iterator_t iterator2;
io_object_t object2;
IOCFPlugInInterface **plugininterface;
MMCDeviceInterface **mmcinterface;
SCSITaskDeviceInterface **scsitaskinterface;
SCSITaskInterface **task;
SCSICmd_INQUIRY_StandardData inquiryBuffer;
SCSITaskStatus taskStatus;
SCSI_Sense_Data senseDataBuffer;
SInt32 score;

stuc->Task = NULL;
stuc->Device = NULL;

ior = IOMasterPort(bootstrap_port, &master);
if ( (ior != kIOReturnSuccess) || (!master) ) return -1;

// check that it is a device name
if (strncmp(name, "/dev/", 5)) return -1;

dictionary = IOBSDNameMatching(master, 0, name+5); /* Starting at [5] gets rid of the /dev/ */
if (!dictionary) return -1;

ior = IOServiceGetMatchingServices(master, dictionary, &iterator1);
if ( (ior == kIOReturnSuccess) && iterator1 && (object1 = IOIteratorNext(iterator1)) ) // we expect a single object
  {
  IOObjectRelease(iterator1);
  ior = IORegistryEntryCreateIterator(object1, kIOServicePlane, kIORegistryIterateParents | kIORegistryIterateRecursively, &iterator2);
  if ((ior == kIOReturnSuccess) && iterator2)
    {
    while ( (object2 = IOIteratorNext(iterator2)) )
      {
      ior = IOCreatePlugInInterfaceForService(object2, kIOMMCDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugininterface ,&score);
      if (ior == kIOReturnSuccess)
        {
        ior = (*plugininterface)->QueryInterface(plugininterface, CFUUIDGetUUIDBytes(kIOMMCDeviceInterfaceID), (LPVOID *) &mmcinterface);
        if (ior == kIOReturnSuccess)
          {
          ior = (*mmcinterface)->Inquiry(mmcinterface, &inquiryBuffer, sizeof(inquiryBuffer), &taskStatus, &senseDataBuffer);
          if ( (ior == kIOReturnSuccess) && (taskStatus == kSCSITaskStatus_GOOD) )
            {
            if (!count--)
              {
              scsitaskinterface = (*mmcinterface)->GetSCSITaskDeviceInterface(mmcinterface);
              if (scsitaskinterface)
                {
                ior = (*scsitaskinterface)->ObtainExclusiveAccess(scsitaskinterface);
                if ( (ior == kIOReturnBusy) && eject)
                  {
                  TryToUnmountAllPartitions(object2);
                  // then retry to obtain exclusive access (our last hope)
                  ior = (*scsitaskinterface)->ObtainExclusiveAccess(scsitaskinterface);
                  }
                if (ior == kIOReturnSuccess)
                  {
                  task = (*scsitaskinterface)->CreateSCSITask(scsitaskinterface);
                  if (task)
                    {
                    stuc->Task = task;
                    stuc->Device = scsitaskinterface;
                    }
                  }
                }
              if (!stuc->Task)
                {
                (*scsitaskinterface)->ReleaseExclusiveAccess(scsitaskinterface);
                (*scsitaskinterface)->Release(scsitaskinterface);
                }
              }
            }
          }
        (*plugininterface)->Release(plugininterface);
        }
      IOObjectRelease(object2);
      }
    IOObjectRelease(iterator2);
    }
  IOObjectRelease(object1);
  }
if (stuc->Task) return 0;
return -1;
}

/*
**  Open this Stuc connection indicated by a path.
**
**  Return zero to succeed, else don't.
**/

int stucOpenByPath(Stuc *stuc, char const *name, int count, int eject)
{
int err;
struct statfs fs;
IOReturn ior;
CFMutableDictionaryRef dictionary;
mach_port_t master;
io_iterator_t iterator1;
io_object_t object1;
io_iterator_t iterator2;
io_object_t object2;
IOCFPlugInInterface **plugininterface;
MMCDeviceInterface **mmcinterface;
SCSITaskDeviceInterface **scsitaskinterface;
SCSITaskInterface **task;
SCSICmd_INQUIRY_StandardData inquiryBuffer;
SCSITaskStatus taskStatus;
SCSI_Sense_Data senseDataBuffer;
SInt32 score;

stuc->Task = NULL;
stuc->Device = NULL;

err = statfs(name, &fs);
if (!err)
  {
  ior = IOMasterPort(bootstrap_port, &master);
  if ( (ior != kIOReturnSuccess) || (!master) ) return -1;

  // check that it is mounted from a device
  if (strncmp(fs.f_mntfromname, "/dev/", 5)) return -1;

  dictionary = IOBSDNameMatching(master, 0, &fs.f_mntfromname[5]); /* Starting at [5] gets rid of the /dev/ */
  if (!dictionary) return -1;

  ior = IOServiceGetMatchingServices(master, dictionary, &iterator1);
  if ( (ior == kIOReturnSuccess) && iterator1 && (object1 = IOIteratorNext(iterator1)) ) // we expect a single object
    {
    IOObjectRelease(iterator1);
    ior = IORegistryEntryCreateIterator(object1, kIOServicePlane, kIORegistryIterateParents | kIORegistryIterateRecursively, &iterator2);
    if ((ior == kIOReturnSuccess) && iterator2)
      {
      while ( (object2 = IOIteratorNext(iterator2)) )
        {
        ior = IOCreatePlugInInterfaceForService(object2, kIOMMCDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugininterface ,&score);
        if (ior == kIOReturnSuccess)
          {
          ior = (*plugininterface)->QueryInterface(plugininterface, CFUUIDGetUUIDBytes(kIOMMCDeviceInterfaceID), (LPVOID *) &mmcinterface);
          if (ior == kIOReturnSuccess)
            {
            ior = (*mmcinterface)->Inquiry(mmcinterface, &inquiryBuffer, sizeof(inquiryBuffer), &taskStatus, &senseDataBuffer);
            if ( (ior == kIOReturnSuccess) && (taskStatus == kSCSITaskStatus_GOOD) )
              {
              if (!count--)
                {
                scsitaskinterface = (*mmcinterface)->GetSCSITaskDeviceInterface(mmcinterface);
                if (scsitaskinterface)
                  {
                  ior = (*scsitaskinterface)->ObtainExclusiveAccess(scsitaskinterface);
                  if ( (ior == kIOReturnBusy) && eject)
                    {
                    TryToUnmountAllPartitions(object2);
                    // then retry to obtain exclusive access (our last hope)
                    ior = (*scsitaskinterface)->ObtainExclusiveAccess(scsitaskinterface);
                    }
                  if (ior == kIOReturnSuccess)
                    {
                    task = (*scsitaskinterface)->CreateSCSITask(scsitaskinterface);
                    if (task)
                      {
                      stuc->Task = task;
                      stuc->Device = scsitaskinterface;
                      }
                    }
                  }
                if (!stuc->Task)
                  {
                  (*scsitaskinterface)->ReleaseExclusiveAccess(scsitaskinterface);
                  (*scsitaskinterface)->Release(scsitaskinterface);
                  }
                }
              }
            }
          (*plugininterface)->Release(plugininterface);
          }
        IOObjectRelease(object2);
        }
      IOObjectRelease(iterator2);
      }
    IOObjectRelease(object1);
    }
  }
if (stuc->Task) return 0;
return -1;
}

/*
**  Open this Stuc connection indicated by an inquiry string.
**
**  Return zero to succeed, else don't.
**/

int stucOpenByInquiry(Stuc *stuc, char const *name, int count, int eject)
{
mach_port_t master;
io_iterator_t iterator;
io_object_t object;
IOReturn ior;
CFMutableDictionaryRef dictionary;
CFMutableDictionaryRef subdictionary;
IOCFPlugInInterface **plugininterface;
MMCDeviceInterface **mmcinterface;
SCSITaskDeviceInterface **scsitaskinterface;
SCSITaskInterface **task;
SCSICmd_INQUIRY_StandardData inquiryBuffer;
SCSITaskStatus taskStatus;
SCSI_Sense_Data senseDataBuffer;
SInt32 score;


stuc->Task = NULL;
stuc->Device = NULL;

ior = IOMasterPort(bootstrap_port, &master);
if ( (ior != kIOReturnSuccess) || (!master) )
  {
  return -1;
  }


// First, try finding matching authoring devices
dictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, NULL, NULL);
if (!dictionary) return -1;

subdictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, NULL, NULL);
if (!subdictionary) return -1;

CFDictionarySetValue(subdictionary, CFSTR(kIOPropertySCSITaskDeviceCategory), CFSTR(kIOPropertySCSITaskAuthoringDevice) );
CFDictionarySetValue(dictionary, CFSTR(kIOPropertyMatchKey), subdictionary);

ior = IOServiceGetMatchingServices(master, dictionary, &iterator);

if ((ior == kIOReturnSuccess) && iterator)
  {
  while ( (object = IOIteratorNext(iterator)) )
    {
    ior = IOCreatePlugInInterfaceForService(object, kIOMMCDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugininterface ,&score);
    if (ior == kIOReturnSuccess)
      {
      ior = (*plugininterface)->QueryInterface(plugininterface, CFUUIDGetUUIDBytes(kIOMMCDeviceInterfaceID), (LPVOID *) &mmcinterface);
      if (ior == kIOReturnSuccess)
        {
        ior = (*mmcinterface)->Inquiry(mmcinterface, &inquiryBuffer, sizeof(inquiryBuffer), &taskStatus, &senseDataBuffer);
        if ( (ior == kIOReturnSuccess) && (taskStatus == kSCSITaskStatus_GOOD) )
          {
          if ( (matchFromInquiry((char *) &inquiryBuffer, name, sizeof(inquiryBuffer))) && (!count--))
            {
            scsitaskinterface = (*mmcinterface)->GetSCSITaskDeviceInterface(mmcinterface);
            if (scsitaskinterface)
              {
              ior = (*scsitaskinterface)->ObtainExclusiveAccess(scsitaskinterface);
              if ( (ior == kIOReturnBusy) && eject)
                {
                TryToUnmountAllPartitions(object);
                // then retry to obtain exclusive access (our last hope)
                ior = (*scsitaskinterface)->ObtainExclusiveAccess(scsitaskinterface);
                }
              if (ior == kIOReturnSuccess)
                {
                task = (*scsitaskinterface)->CreateSCSITask(scsitaskinterface);
                if (task)
                  {
                  stuc->Task = task;
                  stuc->Device = scsitaskinterface;
                  }
                }
              }
            if (!stuc->Task)
              {
              (*scsitaskinterface)->ReleaseExclusiveAccess(scsitaskinterface);
              (*scsitaskinterface)->Release(scsitaskinterface);
              }
            }
          }
        }
      (*plugininterface)->Release(plugininterface);
      }
    IOObjectRelease(object);
    if (stuc->Task) break; // A matching device was found, stop searching for one.
    }
  IOObjectRelease(iterator);
  }
if (stuc->Task) return 0;

// Second, try finding matching non-authoring devices
dictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, NULL, NULL);
if (!dictionary)
  {
  return -1;
  }

subdictionary = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, NULL, NULL);
if (!subdictionary)
  {
  return -1;
  }

CFDictionarySetValue(subdictionary, CFSTR(kIOPropertySCSITaskDeviceCategory), CFSTR(kIOPropertySCSITaskUserClientDevice) );
CFDictionarySetValue(dictionary, CFSTR(kIOPropertyMatchKey), subdictionary);

ior = IOServiceGetMatchingServices(master, dictionary, &iterator);

stuc->Task = NULL;
stuc->Device = NULL;

if ((ior == kIOReturnSuccess) && iterator)
  {
  while ( (object = IOIteratorNext(iterator)) )
    {
    ior = IOCreatePlugInInterfaceForService(object, kIOSCSITaskDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugininterface ,&score);
    if (ior == kIOReturnSuccess)
      {
      ior = (*plugininterface)->QueryInterface(plugininterface, CFUUIDGetUUIDBytes(kIOSCSITaskDeviceInterfaceID), (LPVOID *) &scsitaskinterface);
      if (ior == kIOReturnSuccess)
        {
        ior = (*scsitaskinterface)->ObtainExclusiveAccess(scsitaskinterface);
        if (ior == kIOReturnSuccess)
          {
          task = (*scsitaskinterface)->CreateSCSITask(scsitaskinterface);
          if (task)
            {
            stuc->Task = task;
            stuc->Device = scsitaskinterface;
            if (!match(stuc, name) || (count--))
              {
              (*task)->Release(task);
              stuc->Task = NULL;
              stuc->Device = NULL;
              }
            }
          }
        if (!stuc->Task)
          {
          (*scsitaskinterface)->ReleaseExclusiveAccess(scsitaskinterface);
          (*scsitaskinterface)->Release(scsitaskinterface);
          }
        }
      (*plugininterface)->Release(plugininterface);
      }
    IOObjectRelease(object);
    if (stuc->Task) break; // A matching device was found, stop searching for one.
    }
  IOObjectRelease(iterator);
  }

if (stuc->Task) return 0;

return -1;
}

/*
**  Open this Stuc connection.
**
**  Return zero to succeed, else don't.
**/

int stucOpen(Stuc *stuc, char const *name)
{
int count;
int eject;

if (!stuc) return -1;
if (!name) return -1;

stucClose(stuc);
strlcpy(stuc->Name, name, sizeof(stuc->Name));

count = 0;
if (name[0] && (name[1] == ':'))
  {
       if ( (name[0] >= 'a') && (name[0] <= 'z')) { count = name[0] - 'a'; name += 2; }
  else if ( (name[0] >= 'A') && (name[0] <= 'Z')) { count = name[0] - 'A'; name += 2; }
  else count = 0; // illegal syntax, ignore prefix.
  }
eject = 1;
if ( (name[0] == '+') && (name[1] == ':') )
  {
  eject = 0;
  name += 2;
  }

switch(name[0])
  {
case '/':
  if (!strncmp(name, "/dev/", 5) && name[5]) return stucOpenByDevice(stuc, name, count, eject);
  return stucOpenByPath(stuc, name, count, eject);
default:
  return stucOpenByInquiry(stuc, name, count, eject);
  }
return -1;
}

/*
**  Close this Stuc connection.
**/

void stucClose(Stuc *stuc)
{
if (!stuc) return;
if (stuc->Task) (*stuc->Task)->Release(stuc->Task);
if (stuc->Device)
  {
  (*stuc->Device)->ReleaseExclusiveAccess(stuc->Device);
  (*stuc->Device)->Release(stuc->Device);
  }
stuc->Task = NULL;
stuc->Device = NULL;
stuc->SenseValid = 0;
}

/*
**  Limit the count of sense bytes copied In by stucSay.
**
**  Return zero if ok, else negative.
**/

int stucLimitSense(Stuc *stuc, int length)
{
if (length < 0) return -1;
if (stuc) stuc->SenseLength = length;
return 0;
}

/*
**  Convert to ms from (s, ns) else return negative.
**/

long toMilliseconds(int s, int ns)
{
long ms;
    
if (s > (LONG_MAX/1000)) return -1;
ms = s * 1000;
if ((0 <= ns) && (ns < 1000000))
  {
  int msOfNs = ((ns + 999999 - 1) / 1000000);
  if ((0 <= ms) && (ms <= (LONG_MAX - msOfNs)))
    {
    ms += msOfNs;
    return ms;
    }
  }
return -1;
}

/*
**  Specify the min time to wait before cancelling stucSay.
**
**  Let negative time mean allow the max possible time.
**
**  Return zero if ok, else negative.
**/

int stucLimitSeconds(Stuc *stuc, INT s, INT ns)
{
long time;

if (!stuc) return -1;

/* Round up to whole milliseconds, let negative mean max. */

time = toMilliseconds(s, ns);
if (time < 0) time = (28 * 60 * 60 * 1000); /* 28 * 60 * 60 * 1000 ms = 28 hours = 1.125 days */

/* Set a positive limit and succeed, else fail. */
if (0 < time)
  {
  stuc->Timeout = time;
  return 0;
  }
return -1;
}

/*
**  Speak a sentence of Scsi through a Stuc connection.
**
**  Return zero if all was ok, positive to say specifically how many
**  less than all data bytes were copied, else negative.  Also return
**  zero if all was ok except that the number of data bytes copied is
**  unknown.
**/

INT stucSay(Stuc *stuc, char const *cdb, int cdblen, char *buffer, INT maxLength, int dir)
{
UInt32 sns;
static char dummy;

if (dir == X0_DATA_NOT) maxLength = 0;
if ( (maxLength == 0) && (!buffer) ) buffer = &dummy;

stuc->ActualCount = maxLength;

if (stuc)
  sns = DoCommand(stuc, (UInt8 *) cdb, cdblen, dir, (UInt8 *) buffer, maxLength, stuc->Timeout, &stuc->ActualCount);

if (sns)  return -1;

if ((dir == X1_DATA_IN) || (dir == X2_DATA_OUT))
  return maxLength - stuc->ActualCount;

return 0;
}

/*
**  Copy some or all of the sense bytes copied by stucSay.
**
**  Return the count of bytes copied.
**/

int stucGetSense(Stuc *stuc, char *buffer, int charsLength, int elseLength)
{
if (!stuc) return 0;

if (charsLength < 0) charsLength = elseLength;

if (stuc->SenseValid)
  {
  if (charsLength > stuc->SenseLength) charsLength = stuc->SenseLength;
  memmove(buffer, (char *) stuc->Sense, charsLength);
  }
else
  {
  charsLength = 0;
  }

return charsLength;
}

/*
**  Redirect the stderr of this Stuc connection.
**/

void stucSetErr(Stuc *stuc, FILE *file)
{
if (!stuc) return;
if (!file) file = stderr;
stuc->ErrorFile = file;
}

/*
**  Count the data bytes copied In or Out by stucSay.
**
**  Guess the elseLength if the actual length of data bytes copied is
**  unknown.  Also if the stuc is null.
**/

INT stucGetLength(Stuc *stuc, INT elseLength)
{
if (!stuc) return elseLength;
return stuc->ActualCount;
}

/*
**  Read the first else the next device path name.
**
**  Copy a name of up to charsLength to the chars.
**
**  Return the positive length of chars assigned, else zero to say the
**  name was too long (or if chars is null), else negative if no more
**  names exist (or if stuc is null).
**/

int stucReadName(Stuc *stuc, char *chars, int charsLength)
{
char tmp[PATH_MAX + 16];
int copyChars;

if (!stuc) return -1;

stucClose(stuc);

if (!chars) return 0;

if (stuc->Name[0] == 0)
  {
  stuc->Name[0] = 'A';
  stuc->Name[1] = ':';
  stuc->Name[2] = 0;
  }
else if ( stuc->Name[1] == ':' && (isalpha(toupper(stuc->Name[0]))) )
  {
  if ( (stuc->Name[0] == 'Z') || (stuc->Name[0] == 'z') )
    return -1;

  if ((stuc->Name[0] >= 'a') && (stuc->Name[0] < 'z'))
    stuc->Name[0]++;
  else if ((stuc->Name[0] >= 'A') && (stuc->Name[0] < 'Z'))
    stuc->Name[0]++;
  }
else
  { // name has no count prefix, add one requesting for the 2nd matching device
  strlcpy(tmp, stuc->Name, sizeof(tmp));
  stuc->Name[0] = 'B';
  stuc->Name[1] = ':';
  stuc->Name[2] = 0;
  strlcat(stuc->Name, tmp, sizeof(stuc->Name));
  }
copyChars = strlen(stuc->Name)+1;
if (copyChars > charsLength) copyChars = charsLength;
memmove(chars, stuc->Name, copyChars);
chars[copyChars-1]=0;

return copyChars;
}

/*
**  Accept a -X option of a Scsi command line.
**
**  Return zero if ok, else negative.
**/

int stucSwallowArg(Stuc *stuc, char const *arg)
{
if (!stuc) return -1;
if (!arg) return -1;

if (!strcmp(arg, "STUC")) return 0; // STUC stands for SCSI Task User Client, an OS X acronym.
return -1;
}

#endif /* STUC */
