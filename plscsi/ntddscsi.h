/**
 * ntddscsi.h
 *
 * This source file for Win 2K/XP exists to make:
 *
 *	#include "ntddscsi.h"
 *
 * work, with both Microsoft Visual C++, and
 * also in the gcc of:
 *
 *	http://www.mingw.org/
 *
 * Most of the names and the values equated here
 * match those you can find in the version of
 * Microsoft's "ntddk/inc/ntddscsi.h" that is tagged:
 *
 *	BUILD Version: 0001
 *	Copyright (c) 1990-1999  Microsoft Corporation
 *
 * That version in turn matches what you can view
 * in such places as:
 *
 *	Windows DDK: System Support for Buses: DeviceIoControl struct SCSI_PASS_THROUGH
 *	http://msdn.microsoft.com/library/en-us/buses/hh/buses/scsi_struct_34tu.asp
 *
 *	Windows DDK: System Support for Buses: DeviceIoControl struct SCSI_PASS_THROUGH_DIRECT
 *	http://msdn.microsoft.com/library/en-us/buses/hh/buses/scsi_struct_3qnm.asp
 *
 * Microsoft's ntddk/ version is more complete.  This
 * version contains just enough names to make
 * DeviceIoControl work, and may be more closely
 * tied to the 32-bit versions of Windows.
 *
 * Microsoft's version also implicitly depends on:
 *
 *	#include <devioctl.h>
 *
 * We did try the alternative: -I /ntddk/inc/
 * but we saw an explosion of gcc -Wall complaints
 * in <ctype.h>, in <jni.h>, etc.
 */

/** Compile at most once. */

#ifndef NTDDSCSI_H
#define NTDDSCSI_H NTDDSCSI_H

/** Link as C, not C++. */

#ifdef __cplusplus
extern "C" {
#endif

/** Define enough to talk IOCTL_SCSI_PASS_THROUGH. */

#define IOCTL_SCSI_PASS_THROUGH		0x4D004

#define SCSI_IOCTL_DATA_OUT		0
#define SCSI_IOCTL_DATA_IN		1
#define SCSI_IOCTL_DATA_UNSPECIFIED	2

typedef struct ScsiPassThrough {
	USHORT			Length;
	UCHAR			ScsiStatus;
	UCHAR			PathId;
	UCHAR			TargetId;
	UCHAR			Lun;
	UCHAR			CdbLength;
	UCHAR			SenseInfoLength;
	UCHAR			DataIn;	
	ULONG			DataTransferLength;
	ULONG			TimeOutValue;
	ULONG_PTR		DataBufferOffset;
	ULONG			SenseInfoOffset;
	UCHAR			Cdb[16];
} SCSI_PASS_THROUGH;

/** Define enough more to talk IOCTL_SCSI_PASS_THROUGH_DIRECT. */

#define IOCTL_SCSI_PASS_THROUGH_DIRECT	0x4D014

typedef struct ScsiPassThroughDirect {
	USHORT			Length;
	UCHAR			ScsiStatus;
	UCHAR			PathId;
	UCHAR			TargetId;
	UCHAR			Lun;
	UCHAR			CdbLength;
	UCHAR			SenseInfoLength;
	UCHAR			DataIn;
	ULONG			DataTransferLength;
	ULONG			TimeOutValue;
	PVOID			DataBuffer;
	ULONG			SenseInfoOffset;
	UCHAR			Cdb[16];
} SCSI_PASS_THROUGH_DIRECT;

/** Finish linking as C, not C++. */

#ifdef __cplusplus
}
#endif

/** Finish compiling at most once. */

#endif /* NTDDSCSI_H */

/* end of file */

