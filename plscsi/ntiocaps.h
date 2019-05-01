/**
 * This part taken from ScsiIf-nt.cc of cdrdao from Andreas Mueller (GPL):
 *    http://cvs.sourceforge.net/viewcvs.py/cdrdao/cdrdao/dao/ScsiIf-nt.cc?rev=1.2
 **/

/*  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */




/** Compile at most once. */

#ifndef NTIOCAPS_H
#define NTIOCAPS_H NTIOCAPS_H

/** Link as C, not C++. */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _IO_SCSI_CAPABILITIES 
{
    ULONG Length;
    ULONG MaximumTransferLength;
    ULONG MaximumPhysicalPages;
    ULONG SupportedAsynchronousEvents;
    ULONG AlignmentMask;
    BOOLEAN TaggedQueuing;
    BOOLEAN AdapterScansDown;
    BOOLEAN AdapterUsesPio;
} IO_SCSI_CAPABILITIES, *PIO_SCSI_CAPABILITIES;

#define IOCTL_SCSI_BASE                 FILE_DEVICE_CONTROLLER
#define IOCTL_SCSI_GET_CAPABILITIES     CTL_CODE(IOCTL_SCSI_BASE, 0x0404, METHOD_BUFFERED, FILE_ANY_ACCESS)

/** Finish linking as C, not C++. */

#ifdef __cplusplus
}
#endif

/** Finish compiling at most once. */

#endif /* NTIOCAPS_H */

/* end of file */