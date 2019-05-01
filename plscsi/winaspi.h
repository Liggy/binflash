/******************************************************************************
**
**  Module Name:    winaspi.h
**
**  Description:    Header file for ASPI for Win16.  This header includes
**                  macro and type declarations, and can be included without
**                  modification when Microsoft Visual C++ with 16-bit
**                  compilation.  If you are using a different compiler then
**                  you MUST ensure that structures are packed onto byte
**                  alignments, and that C++ name mangling is turned off.
**
**  Notes:          This file created using 4 spaces per tab.
**
******************************************************************************/

#ifndef __WINASPI_H__
#define __WINASPI_H__

/*
** Make sure structures are packed and undecorated.
*/

#ifdef _MSC_VER
#define _PACK
#endif //__MSC_VERP

#ifdef DJGPP
#define _PACK
#endif //DJGPP

#ifdef _PACK
#pragma pack(1)
#endif //_PACK

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

//*****************************************************************************
//      %%% SCSI MISCELLANEOUS EQUATES %%%
//*****************************************************************************

#define SENSE_LEN                   14          // Default sense buffer length
#define SRB_DIR_SCSI                0x00        // Direction determined by SCSI
#define SRB_POSTING                 0x01        // Enable ASPI posting
#define SRB_ENABLE_RESIDUAL_COUNT   0x04        // Enable residual byte count reporting
#define SRB_DIR_IN                  0x08        // Transfer from SCSI target to host
#define SRB_DIR_OUT                 0x10        // Transfer from host to SCSI target

//*****************************************************************************
//      %%% ASPI Command Definitions %%%
//*****************************************************************************

#define SC_HA_INQUIRY               0x00        // Host adapter inquiry
#define SC_GET_DEV_TYPE             0x01        // Get device type
#define SC_EXEC_SCSI_CMD            0x02        // Execute SCSI command
#define SC_ABORT_SRB                0x03        // Abort an SRB
#define SC_RESET_DEV                0x04        // SCSI bus device reset
#define SC_SET_HA_PARMS             0x05        // Set HA parameters
#define SC_GET_DISK_INFO            0x06        // Get Disk information

//*****************************************************************************
//      %%% SRB Status %%%
//*****************************************************************************

#define SS_PENDING                  0x00        // SRB being processed
#define SS_COMP                     0x01        // SRB completed without error
#define SS_ABORTED                  0x02        // SRB aborted
#define SS_ABORT_FAIL               0x03        // Unable to abort SRB
#define SS_ERR                      0x04        // SRB completed with error

#define SS_INVALID_CMD              0x80        // Invalid ASPI command
#define SS_INVALID_HA               0x81        // Invalid host adapter number
#define SS_NO_DEVICE                0x82        // SCSI device not installed

#define SS_INVALID_SRB              0xE0        // Invalid parameter set in SRB
#define SS_OLD_MANAGER              0xE1        // ASPI manager doesn't support Windows
#define SS_BUFFER_ALIGN             0xE1        // Buffer not aligned (replaces OLD_MANAGER in Win32)
#define SS_ILLEGAL_MODE             0xE2        // Unsupported Windows mode
#define SS_NO_ASPI                  0xE3        // No ASPI managers resident
#define SS_FAILED_INIT              0xE4        // ASPI for windows failed init
#define SS_ASPI_IS_BUSY             0xE5        // No resources available to execute cmd
#define SS_BUFFER_TO_BIG            0xE6        // Buffer size to big to handle!
#define SS_MISMATCHED_COMPONENTS    0xE7        // The DLLs/EXEs of ASPI don't version check
#define SS_NO_ADAPTERS              0xE8        // No host adapters to manage
#define SS_INSUFFICIENT_RESOURCES   0xE9        // Couldn't allocate resources needed to init
#define SS_ASPI_IS_SHUTDOWN         0xEA        // Call came to ASPI after PROCESS_DETACH
#define SS_BAD_INSTALL              0xEB        // The DLL or other components are installed wrong

//*****************************************************************************
//      %%% Host Adapter Status %%%
//*****************************************************************************

#define HASTAT_OK                   0x00        // Host adapter did not detect an                                                                                                                       // error
#define HASTAT_SEL_TO               0x11        // Selection Timeout
#define HASTAT_DO_DU                0x12        // Data overrun data underrun
#define HASTAT_BUS_FREE             0x13        // Unexpected bus free
#define HASTAT_PHASE_ERR            0x14        // Target bus phase sequence                                                                                                                            // failure
#define HASTAT_TIMEOUT              0x09        // Timed out while SRB was                                                                                                                                      waiting to beprocessed.
#define HASTAT_COMMAND_TIMEOUT      0x0B        // Adapter timed out processing SRB.
#define HASTAT_MESSAGE_REJECT       0x0D        // While processing SRB, the                                                                                                                            // adapter received a MESSAGE
#define HASTAT_BUS_RESET            0x0E        // A bus reset was detected.
#define HASTAT_PARITY_ERROR         0x0F        // A parity error was detected.
#define HASTAT_REQUEST_SENSE_FAILED 0x10        // The adapter failed in issuing

//*****************************************************************************
//          %%% SRB - HOST ADAPTER INQUIRY - SC_HA_INQUIRY (0) %%%
//*****************************************************************************

typedef struct                                  // Offset
{                                               // HX/DEC
    BYTE        SRB_Cmd;                        // 00/000 ASPI command code = SC_HA_INQUIRY
    BYTE        SRB_Status;                     // 01/001 ASPI command status byte
    BYTE        SRB_HaId;                       // 02/002 ASPI host adapter number
    BYTE        SRB_Flags;                      // 03/003 ASPI request flags
    WORD        SRB_55AASignature;              // 04/004 Signature to signal extended inquiry
    WORD        SRB_ExtBufferSize;              // 06/006 Size of extended buffer, if any
    BYTE        HA_Count;                       // 08/008 Number of host adapters present
    BYTE        HA_SCSI_ID;                     // 09/009 SCSI ID of host adapter
    BYTE        HA_ManagerId[16];               // 0A/010 String describing the manager
    BYTE        HA_Identifier[16];              // 1A/026 String describing the host adapter
    BYTE        HA_Unique[16];                  // 2A/042 Host Adapter Unique parameters
    BYTE        HA_ExtBuffer[8];                // 3A/058 Extended inquiry data
}
SRB_HAInquiry, *PSRB_HAInquiry, FAR *LPSRB_HAInquiry;

//*****************************************************************************
//          %%% SRB - GET DEVICE TYPE - SC_GET_DEV_TYPE (1) %%%
//*****************************************************************************

typedef struct                                  // Offset
{                                               // HX/DEC
    BYTE        SRB_Cmd;                        // 00/000 ASPI command code = SC_GET_DEV_TYPE
    BYTE        SRB_Status;                     // 01/001 ASPI command status byte
    BYTE        SRB_HaId;                       // 02/002 ASPI host adapter number
    BYTE        SRB_Flags;                      // 03/003 ASPI request flags
    DWORD       SRB_Hdr_Rsvd;                   // 04/004 Reserved, MUST = 0
    BYTE        SRB_Target;                     // 08/008 Target's SCSI ID
    BYTE        SRB_Lun;                        // 09/009 Target's LUN number
    BYTE        SRB_DeviceType;                 // 0A/010 Target's peripheral device type
}
SRB_GDEVBlock, *PSRB_GDEVBlock, FAR *LPSRB_GDEVBlock;

//*****************************************************************************
//          %%% SRB - EXECUTE SCSI COMMAND - SC_EXEC_SCSI_CMD (2) %%%
//*****************************************************************************

typedef struct                                  // Offset Structure for 6-byte CDBs
{                                               // HX/DEC
    BYTE        SRB_Cmd;                        // 00/000 ASPI command code = SC_EXEC_SCSI_CMD
    BYTE        SRB_Status;                     // 01/001 ASPI command status byte
    BYTE        SRB_HaId;                       // 02/002 ASPI host adapter number
    BYTE        SRB_Flags;                      // 03/003 ASPI request flags
    DWORD       SRB_Hdr_Rsvd;                   // 04/004 Reserved, MUST = 0
    BYTE        SRB_Target;                     // 08/008 Target's SCSI ID
    BYTE        SRB_Lun;                        // 09/009 Target's LUN number
    DWORD       SRB_BufLen;                     // 0A/010 Data Allocation Length
    BYTE        SRB_SenseLen;                   // 0E/014 Sense Allocation Length
    BYTE        FAR *SRB_BufPointer;            // 0F/015 Data Buffer Pointer
    DWORD       SRB_Rsvd1;                      // 13/019 Reserved, MUST = 0
    BYTE        SRB_CDBLen;                     // 17/023 CDB Length = 6
    BYTE        SRB_HaStat;                     // 18/024 Host Adapter Status
    BYTE        SRB_TargStat;                   // 19/025 Target Status
    VOID        FAR *SRB_PostProc;              // 1A/026 Post routine
    BYTE        SRB_Rsvd2[34];                  // 1E/030 Reserved, MUST = 0
    BYTE        CDBByte[6];                     // 40/064 SCSI CDB
    BYTE        SenseArea6[SENSE_LEN];          // 46/070 Request Sense buffer
}
SRB_ExecSCSICmd6, *PSRB_ExecSCSICmd6, FAR *LPSRB_ExecSCSICmd6;

typedef struct                                  // Offset Structure for 10-byte CDBs
{                                               // HX/DEC
    BYTE        SRB_Cmd;                        // 00/000 ASPI command code = SC_EXEC_SCSI_CMD
    BYTE        SRB_Status;                     // 01/001 ASPI command status byte
    BYTE        SRB_HaId;                       // 02/002 ASPI host adapter number
    BYTE        SRB_Flags;                      // 03/003 ASPI request flags
    DWORD       SRB_Hdr_Rsvd;                   // 04/004 Reserved, MUST = 0
    BYTE        SRB_Target;                     // 08/008 Target's SCSI ID
    BYTE        SRB_Lun;                        // 09/009 Target's LUN number
    DWORD       SRB_BufLen;                     // 0A/010 Data Allocation Length
    BYTE        SRB_SenseLen;                   // 0E/014 Sense Allocation Length
    BYTE        FAR *SRB_BufPointer;            // 0F/015 Data Buffer Pointer
    DWORD       SRB_Rsvd1;                      // 13/019 Reserved, MUST = 0
    BYTE        SRB_CDBLen;                     // 17/023 CDB Length = 10
    BYTE        SRB_HaStat;                     // 18/024 Host Adapter Status
    BYTE        SRB_TargStat;                   // 19/025 Target Status
    VOID        FAR *SRB_PostProc;              // 1A/026 Post routine
    BYTE        SRB_Rsvd2[34];                  // 1E/030 Reserved, MUST = 0
    BYTE        CDBByte[6];                     // 40/064 SCSI CDB
    BYTE        SenseArea10[SENSE_LEN];         // 50/070 Request Sense buffer
}
SRB_ExecSCSICmd10, *PSRB_ExecSCSICmd10, FAR *LPSRB_ExecSCSICmd10;

typedef struct                                  // Offset Structure for 12-byte CDBs
{                                               // HX/DEC
    BYTE        SRB_Cmd;                        // 00/000 ASPI command code = SC_EXEC_SCSI_CMD
    BYTE        SRB_Status;                     // 01/001 ASPI command status byte
    BYTE        SRB_HaId;                       // 02/002 ASPI host adapter number
    BYTE        SRB_Flags;                      // 03/003 ASPI request flags
    DWORD       SRB_Hdr_Rsvd;                   // 04/004 Reserved, MUST = 0
    BYTE        SRB_Target;                     // 08/008 Target's SCSI ID
    BYTE        SRB_Lun;                        // 09/009 Target's LUN number
    DWORD       SRB_BufLen;                     // 0A/010 Data Allocation Length
    BYTE        SRB_SenseLen;                   // 0E/014 Sense Allocation Length
    BYTE        FAR *SRB_BufPointer;            // 0F/015 Data Buffer Pointer
    DWORD       SRB_Rsvd1;                      // 13/019 Reserved, MUST = 0
    BYTE        SRB_CDBLen;                     // 17/023 CDB Length = 12
    BYTE        SRB_HaStat;                     // 18/024 Host Adapter Status
    BYTE        SRB_TargStat;                   // 19/025 Target Status
    VOID        FAR *SRB_PostProc;              // 1A/026 Post routine
    BYTE        SRB_Rsvd2[34];                  // 1E/030 Reserved, MUST = 0
    BYTE        CDBByte[12];                    // 40/064 SCSI CDB
    BYTE        SenseArea12[SENSE_LEN];         // 4C/076 Request Sense buffer
}
SRB_ExecSCSICmd12, *PSRB_ExecSCSICmd12, FAR *LPSRB_ExecSCSICmd12;

//*****************************************************************************
//          %%% SRB - ABORT AN SRB - SC_ABORT_SRB (3) %%%
//*****************************************************************************

typedef struct                                  // Offset
{                                               // HX/DEC
    BYTE        SRB_Cmd;                        // 00/000 ASPI command code = SC_ABORT_SRB
    BYTE        SRB_Status;                     // 01/001 ASPI command status byte
    BYTE        SRB_HaId;                       // 02/002 ASPI host adapter number
    BYTE        SRB_Flags;                      // 03/003 ASPI request flags
    DWORD       SRB_Hdr_Rsvd;                   // 04/004 Reserved, MUST = 0
    VOID        FAR *SRB_ToAbort;               // 08/008 Pointer to SRB to abort
}
SRB_Abort, *PSRB_Abort, FAR *LPSRB_Abort;

//*****************************************************************************
//          %%% SRB - BUS DEVICE RESET - SC_RESET_DEV (4) %%%
//*****************************************************************************

typedef struct                                  // Offset
{                                               // HX/DEC
    BYTE        SRB_Cmd;                        // 00/000 ASPI command code = SC_RESET_DEV
    BYTE        SRB_Status;                     // 01/001 ASPI command status byte
    BYTE        SRB_HaId;                       // 02/002 ASPI host adapter number
    BYTE        SRB_Flags;                      // 03/003 ASPI request flags
    DWORD       SRB_Hdr_Rsvd;                   // 04/004 Reserved, MUST = 0
    BYTE        SRB_Target;                     // 08/008 Target's SCSI ID
    BYTE        SRB_Lun;                        // 09/009 Target's LUN number
    BYTE        SRB_ResetRsvd1[14];             // 0A/010 Reserved, MUST = 0
    BYTE        SRB_HaStat;                     // 18/024 Host Adapter Status
    BYTE        SRB_TargStat;                   // 19/025 Target Status
    VOID        FAR *SRB_PostProc;              // 1A/026 Post routine
    BYTE        SRB_ResetRsvd2[34];             // 1E/030 Reserved, MUST = 0
}
SRB_BusDeviceReset, *PSRB_BusDeviceReset, FAR *LPSRB_BusDeviceReset;

//*****************************************************************************
//          %%% PROTOTYPES - User Callable ASPI for Win16 Functions %%%
//*****************************************************************************

typedef BYTE far *LPSRB;

WORD FAR PASCAL SendASPICommand( LPSRB );
WORD FAR PASCAL GetASPISupportInfo();

/*
** Restore compiler default packing and close off the C declarations.
*/

#ifdef _PACK
#pragma pack()
#endif //_PACK

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__WINASPI_H__
