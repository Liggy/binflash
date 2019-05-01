/**
**  dos32aspi.cpp
**  Wednesday. October 15, 2003
**
**  Talk thru an Aspi connection of Microsoft DOS 
**  using an ASPI library in 32 bit protected mode.
**
**  Grep views include:
**
**      grep "^[A-Za-z_]"   // routines
**      grep "[Ee]rr|fpr"   // errors traced
**/

/**
**  Link with standard C libraries.
**/

#include <stddef.h> /* offsetof ... */
#include <stdio.h> /* FILE ... */
#include <stdlib.h> /* calloc ... */
#include <string.h> /* memset ... */


/**
**  Link with the *.c* of ../plscsi/.
**/

#include "plscsi.h"

/**
**  Link with Microsoft Windows and the Adaptec SDK.
**/

#ifdef DOSASPI

#include <sys/types.h>
#include <sys/movedata.h>
#include <dpmi.h>
#include <go32.h>
#include <dos.h>
#include "dosaspi.h"

/**
**  Define some units.
**/

#define Ki      1024 /* x1024 = x400 = 1 << 10 */


/* Name the struct (SRB_Cmd SRB_Status SRB_HaId) common to all SRB's. */

typedef SRB_Abort SRB;

char const * theAspiLibName = "SCSIMGR$";
DWORD theScsiMgr;           /* SEGMENT:OFFSET */

/* Conventional Memory Buffers, allocated once, *NEVER* freed */

#define CM_srbSize (sizeof(SRB_ExecSCSICmd_Max))
DWORD   CM_srb = NULL;      /* SEGMENT:OFFSET */
int     CM_srbSelector = 0;
#define CM_BufSize (64*Ki)  /* 64 KB should be enough for everybody */
DWORD   CM_Buffer = NULL;   /* SEGMENT:OFFSET */
int     CM_BufSelector = 0;
DWORD   SRB_BufLenCopy = 0; /* We need to keep a copy of this */

/**
**  Describe a DOS Aspi connection to a device.
**/

#define MAX_CDB_LENGTH      0xFF
#define MAX_SENSE_LENGTH    0xFF

typedef struct SRB_ExecSCSICmd_Max SRB_ExecSCSICmd_Max;

struct SRB_ExecSCSICmd_Max
    {
    SRB_ExecSCSICmd     theCoreChars;
    char                theMaxCdbChars[MAX_CDB_LENGTH]; /* 6 more than necessary */
    char                theMaxSenseChars[MAX_SENSE_LENGTH + 2]; /* SENSE_LEN+2 more than necessary */
    };

typedef union AspiCommand AspiCommand;

union AspiCommand
    {
    SRB                     theSRB;
    SRB_HAInquiry           theSRB_HAInquiry;
    SRB_GDEVBlock           theSRB_GDEVBlock;
    SRB_ExecSCSICmd         theSRB_ExecSCSICmd;
    SRB_ExecSCSICmd_Max     theSRB_ExecSCSICmd_Max;
    SRB_Abort               theSRB_Abort;
    SRB_BusDeviceReset      theSRB_BusDeviceReset;
    };

struct Aspi /* connection */
    {
    AspiCommand     theAspiCommand;

    FILE *          theErrFile;

    int             theHa;
    int             theTarget;
    int             theLun;

    int             theMayCountDataFlag;
    int             theMaxHa;
    int             theMaxTarget;
    int             theMaxLun;

    int             theMaxSenseLength;

    INT             theExitInt;
    INT             theLength;
    char const *    theSenseChars;
    int             theSenseLength;
    };

/**
**  Dynamically link a library.
**
**  Return zero if not ok, else the bytes read.
**/

DWORD loadLibrary(char const * name)
    {
    DWORD result = NULL;
    __dpmi_regs stackedRp;
    __dpmi_regs * rp = &stackedRp;
    unsigned short handle = 0;
    char stackedChars[8 + 1];
    char * chars = &stackedChars[0];
    int copyLength = 0;

    (void) memset(rp, '\0', sizeof *rp);
    (void) memset(chars, '\0', sizeof stackedChars);

    /* Decide the name. */

    copyLength = (strlen(name) + 1);
    if (!(copyLength <= sizeof stackedChars)) return 0;
    (void) memmove(chars, name, copyLength);

    /* Copy to DOS memory */

    chars[copyLength] = '\0';
    dosmemput(chars, copyLength+1, __tb);

    /* Only mostly zero the bits of the Cpu. */

    rp->x.ds = __tb >> 4;
    rp->x.es = __tb >> 4;
    rp->x.flags = 0x0200; /* x200 = I flag */   

    /* Open the Dos Aspi device. */

    rp->x.ax = 0x3D00; /* x21:3D00 = open to read */
    rp->x.dx = __tb & 0x0f;
    __dpmi_int(0x21, rp);

    if ((rp->x.flags & 0x0001) == 0) /* x1 = C flag */
        {
        handle = rp->x.ax;

        /* Read the Dos Aspi entry point via an Ioctl. */

        rp->x.ax = 0x4402; /* x21:4402 = ioctl read */
        rp->x.bx = handle;
        rp->x.cx = sizeof result;
        rp->x.dx = __tb & 0x0f;		// ((unsigned short) &result);
        __dpmi_int(0x21, rp);
        if ((rp->x.flags & 0x0001) != 0) /* x1 = C flag */
            {
            (void) fprintf(stderr, "// x.flags = x%X\n", (int) rp->x.flags);
            (void) fprintf(stderr, "// x.ax = x%X\n", (int) rp->x.ax);
            }
        else if (rp->x.ax != 0x0004) /* r_ax = count of bytes transferred */
            {
            if (rp->x.ax == 0x4402) /* x4402 = no change */
                {
                ; /* e.g. WinMe */
                }
            else
                {
                (void) fprintf(stderr, "// x.flags = x%X\n", (int) rp->x.flags);
                (void) fprintf(stderr, "// x.ax = x%X\n", (int) rp->x.ax);
                }
            }
            
        dosmemget(__tb,4,&result);

        /* Close the opened Dos Aspi device. */

        rp->x.ax = 0x3E00; /* x21:3E = close */
        rp->x.bx = handle;
        __dpmi_int(0x21, rp);
        if ((rp->x.flags & 0x0001) != 0) /* x1 = C flag */
            {
            (void) fprintf(stderr, "// x.flags = x%X\n", (int) rp->x.flags);
            (void) fprintf(stderr, "// x.ax = x%X\n", (int) rp->x.ax);
            }
        }

    if (result)
        {
                
        /* Allocate Conventional Memory Buffers */
        
        if ( (CM_srb == NULL) &&
             ((CM_srb = ((DWORD) __dpmi_allocate_dos_memory((CM_srbSize+15)>>4, 
              &CM_srbSelector)<<16)) == -1) )
            {
            CM_srb = NULL;
            (void) fprintf(stderr, "// CM srb allocation failure.\n");
            return NULL;
            }
        
        if ( (CM_Buffer == NULL) &&
             ((CM_Buffer = ((DWORD) __dpmi_allocate_dos_memory((CM_BufSize+15)>>4,
              &CM_BufSelector)<<16)) == -1) )
            {
            CM_Buffer = NULL;
            (void) fprintf(stderr, "// CM Buffer allocation failure.\n");
            return NULL;
            }
            
        }

    /* Return the bytes read. */

    return result;
    }

/**
** CallScsiMgr
**
** Return NULL if not OK, else return the conventional memory duplicata of the SRB
**/

int CallScsiMgr(BYTE* srb, FILE * fi = NULL)
    {
    __dpmi_regs r;

    if ((srb == NULL) || (CM_srb == NULL) || (CM_Buffer == NULL)) return -1;

    SRB_ExecSCSICmd * es = (SRB_ExecSCSICmd*)srb;

    (void) memset(&r, '\0', sizeof r);
    r.x.ds = CM_srb >> 16;
    r.x.es = CM_srb >> 16;
    r.x.cs = theScsiMgr>>16;
    r.x.ip = theScsiMgr&0xffff;
    
    /* Decide how much data we need to copy */
    
    switch(*srb) /* SRB_Cmd */
        {
   
        case SC_HA_INQUIRY:
        
            /* Copy the SRB Command */
        
            dosmemput(srb, sizeof(SRB_HAInquiry), CM_srb>>12);
            if (__dpmi_simulate_real_mode_procedure_retf_stack(&r, sizeof(DWORD)/2, &CM_srb))
                return -1;
            break;

	case SC_EXEC_SCSI_CMD:

            /* Filter out command parameters we don't want */

            if (es->SRB_PostProc) es->SRB_Status = SS_INVALID_SRB;
	    if (es->SRB_BufLen > CM_BufSize) es->SRB_Status = SS_BUFFER_TO_BIG;
	    
	    /*
	     * Check if a buffer command is already going on. If really reentrant,
	     * we should probably use a stack, but what the heck
	     */ 
	    
	    if (SRB_BufLenCopy) 
	        es->SRB_Status = SS_ASPI_IS_BUSY;
	    else
	        SRB_BufLenCopy = es->SRB_BufLen;
	    
	    /* Copy the SRB Command and set the Buffer pointer to our CM Buffer */

	    dosmemput(srb, sizeof(SRB_ExecSCSICmd_Max), CM_srb>>12);
            dosmemget(CM_srb>>12, sizeof(SRB_ExecSCSICmd_Max), srb);
	    
	    dosmemput(&CM_Buffer, 4, (CM_srb>>12)+offsetof(SRB_ExecSCSICmd, SRB_BufPointer));
	    if (es->SRB_Flags | SRB_DIR_OUT)
	        _dosmemputb(es->SRB_BufPointer, SRB_BufLenCopy, CM_Buffer>>12);

	    /* Don't execute commands with improper SRB_Status */   
	       
	    if ((es->SRB_Status) || (__dpmi_simulate_real_mode_procedure_retf_stack 
	            (&r, sizeof(DWORD)/2, &CM_srb)))
                return -1;
                
            break;
            
        default:
            return -1;
            break;
            
        }
        
	return 0;
}


/**
** GetFromScsiMgr
**
**/

int GetFromScsiMgr (BYTE* srb, FILE * fi = NULL)
{
    if ((srb == NULL) || (CM_srb == NULL) || (CM_Buffer == NULL)) return -1;

    BYTE* BufPointerCopy = NULL;
    BYTE  SRB_Status = 0;
    SRB_ExecSCSICmd * es = (SRB_ExecSCSICmd*)srb;

    switch(*srb) /* SRB_Cmd */
        {
   
        case SC_HA_INQUIRY:
        
      	    /* Copy the SRB back from CM */

            dosmemget(CM_srb>>12, sizeof(SRB_HAInquiry), srb);
            break;
            
      	case SC_EXEC_SCSI_CMD:

            /* Dont do anything is the status is still pending */
      	
      	    dosmemget(CM_srb>>12, 1, &SRB_Status);
      	    if (SRB_Status)      	
      	        {
      	        	
      	        /* Copy the SRB back from CM */

                BufPointerCopy = es->SRB_BufPointer;
      	        dosmemget(CM_srb>>12, sizeof(SRB_ExecSCSICmd_Max), srb);
      	        es->SRB_BufPointer = BufPointerCopy;

      	        /* Copy the buffer back from CM */
      	 
                if ((es->SRB_Status) && (es->SRB_Flags | SRB_DIR_IN) && (SRB_BufLenCopy))
                    {
	            dosmemget(CM_Buffer>>12, SRB_BufLenCopy, es->SRB_BufPointer);
	            SRB_BufLenCopy = 0;
	            }

                }
            else
                if (fi) fprintf(fi, "GetFromScsiMgr: SS_PENDING\n");
                
            break;
            
        default:
            if (fi) fprintf(fi, "GetFromScsiMgr: Command not implemented yet\n");
            return -1;
            break;
            
        }
    return 0;

}


/**
**  Startup.
**
**  Return negative to fail, else don't.
**/

static int aspiStartup(Aspi * aspi)
    {
    static int theAspiStartupInt = 0;

    /* Maybe quickly & gracefully do nothing. */

    if (theAspiStartupInt != 0) return theAspiStartupInt;
    theAspiStartupInt = -1;

    /* Require that the structs of "dosaspi.h" be packed. */

    static int aspiCdbOffset = offsetof(SRB_ExecSCSICmd, CDBByte);
    if (aspiCdbOffset != 0x40)
        {
        FILE * fi = aspi->theErrFile;
        (void) fprintf(fi, "// aspiStartup: struct SRB_ExecSCSICmd not packed\n");
        return -1;
        }

    /* Link the library per se. */

    theScsiMgr = loadLibrary(theAspiLibName);
    if (theScsiMgr == NULL)
        {
        FILE * fi = aspi->theErrFile;
        (void) fprintf(fi, "// aspiStartup.loadLibrary: \"%s\" not found\n", theAspiLibName);
        return -1;
        }

    /* Succeed. */

    theAspiStartupInt = 1;
    return 0;
    }

/**
**  Guess hints always, then ask for substitute hints.
**
**  Return zero if ok, else negative.
**/

int aspiExamine(Aspi * aspi, int ha)
    {
    DWORD CMsrb;
    if (aspi == NULL) return -1;
    if (ha != (BYTE) ha) return -1;

    /* Guess results. */

    aspi->theMayCountDataFlag = 0;
    aspi->theMaxHa = 0;
    aspi->theMaxTarget = (8 - 1);
    aspi->theMaxLun = (8 - 1);

    /* Fail if link failed. */

    if (aspiStartup(aspi) < 0) return -1;

    /* Erase an entire speech. */

    AspiCommand aspiCommand;
    AspiCommand * ac = &aspiCommand;
    SRB * srb = &ac->theSRB;
    SRB_HAInquiry * hs = &ac->theSRB_HAInquiry; /* "hs" = "HAI Srb" */

    (void) memset(ac, '\0', sizeof *ac);
    hs->SRB_Cmd = SC_HA_INQUIRY;

    /* Examine a particular haId. */

    hs->SRB_HaId = (BYTE) ha;
    hs->SRB_55AASignature = 0xAA55;
    hs->SRB_ExtBufferSize = 0x0004;

    /* Call Aspi. */

    FILE * fi = aspi->theErrFile;

    if (CallScsiMgr ((BYTE*) srb, fi)) return -1;
    if (GetFromScsiMgr((BYTE*) srb, fi)) return -1;

    /* Fail else fall through. */

    int hsStatus = hs->SRB_Status;
    if (hsStatus != SS_COMP) /* x01 = SS_COMP */
        {
        FILE * fi = aspi->theErrFile;
        (void) fprintf(fi, "// aspiExamine.SRB_Status = x%lX (%ld)\n",
            (long) hsStatus, (long) hsStatus);
        return -1;
        }

    /* Limit the range of Aspi device names to try by default. */

    aspi->theMaxHa = (hs->HA_Count - 1);
    if (hs->SRB_55AASignature == 0x55AA)
        {
        if ((hs->HA_ExtBuffer[0] & 0x04) != 0)
            {
            aspi->theMaxTarget = (16 - 1);
            }
        if ((hs->HA_ExtBuffer[0] & 0x08) != 0)
            {
            aspi->theMaxTarget = (32 - 1);
            }

        /* Guess whether we may count data or not. */

        if ((hs->HA_ExtBuffer[0] & 0x02) != 0)
            {
            aspi->theMayCountDataFlag = 1;
            }
        }

    /* Succeed. */

    return 0;
    }

/**
**  Construct a new Aspi connection.
**
**  Return null to fail, else don't.
**/

Aspi * newAspi(void)
    {

    /* Alloc and zero a new struct. */

    Aspi * aspi = (Aspi *) calloc(1, sizeof (Aspi));
    if (aspi == NULL) return NULL;

    /* Choose defaults, say closed. */

    aspi->theErrFile = stderr;

    aspi->theHa = 0;
    aspi->theTarget = 0;
    aspi->theLun = -1;

    aspi->theMayCountDataFlag = -1;
    aspi->theMaxHa = -1;
    aspi->theMaxTarget = -1;
    aspi->theMaxLun = -1;

    aspi->theMaxSenseLength = -1;

    aspi->theExitInt = -1;
    aspi->theLength = -1;
    aspi->theSenseChars = NULL;
    aspi->theSenseLength = -1;

    /* Succeed. */

    return aspi;
    }

/**
**  Redirect the stderr of this Aspi connection.
**/

void aspiSetErr(Aspi * aspi, FILE * fi)
    {
    if (aspi == NULL) return;
    if (fi == NULL) fi = stderr;
    aspi->theErrFile = fi;
    }

/**
**  Open this Aspi connection with a new name.
**
**  Return zero to succeed, else don't.
**/

int aspiOpen(Aspi * aspi, char const * name)
    {
    if (aspi == NULL) return -1;
    if (name == NULL) return -1;

    /* Try to open "$ha.$target.$lun". */

    int exitInt = -1;

    int ha = 0;
    int target = 0;
    int lun = 0;
    char etcChar = '\0';
    if (sscanf(name, "%d.%d.%d%c", &ha, &target, &lun, &etcChar) == 3)
        {
        exitInt = 0;
        }

    /* Accept only names that SendASPI32Command can express. */

    if (exitInt == 0)
        {
        exitInt = -1;
        if ((0 <= ha) && (ha <= 0xFF))
        if ((0 <= target) && (target <= 0xFF))
        if ((0 <= lun) && (lun <= 0xFF))
            {
            exitInt = 0;
            }
        }

    /* Commit to a name else die. */

    if (exitInt != 0) return exitInt;

    aspi->theHa = ha;
    aspi->theTarget = target;
    aspi->theLun = lun;

    /* Fail if link failed. */

    if (aspiStartup(aspi) < 0) return -1;

    /* Try to get hints. */

    (void) aspiExamine(aspi, aspi->theHa);

    /* Succeed. */

    return 0;
    }

/**
**  Limit the count of sense bytes copied In by aspiSay.
**
**  Return zero if ok, else negative.
**/

int aspiLimitSense(Aspi * aspi, int maxSenseLength)
    {
    if (aspi == NULL) return -1;

    if ((0 <= maxSenseLength) && (maxSenseLength <= MAX_SENSE_LENGTH))
        {
        aspi->theMaxSenseLength = maxSenseLength;
        return 0;
        }

    return -1;
    }

/**
**  Decide the parameters for SendASPI32Command.
**
**  Return zero if ok, else negative.
**/

static int aspiSetCommand(Aspi * aspi,
        char const * cdbChars, int cdbLength, char * dataChars, INT maxLength, int direction)
    {
    if (aspi == NULL) return -1;

    /* Fetch implicit args. */

    int maxSenseLength = aspi->theMaxSenseLength;

    /* Constrain args. */

    if ((cdbChars == NULL) && (cdbLength != 0)) return -1;
    if ((cdbLength < 0) || (MAX_CDB_LENGTH < cdbLength)) return -1;
    if ((dataChars == NULL) && (maxLength != 0)) return -1;
    if ((maxSenseLength < 0) || (MAX_SENSE_LENGTH < maxSenseLength)) return -1;

    if (cdbLength != (BYTE) cdbLength) return -1;
    if (maxSenseLength != (BYTE) maxSenseLength) return -1;

    /* Erase the entire speech. */

    AspiCommand * ac = &aspi->theAspiCommand;
    (void) memset(ac, '\0', sizeof *ac);

    SRB_ExecSCSICmd * es = &ac->theSRB_ExecSCSICmd; /* es = escSrb = SRB_ExecSCSICmd */
    es->SRB_Cmd = SC_EXEC_SCSI_CMD; /* x02 = SC_EXEC_SCSI_CMD */

    /* Ask to copy all the Cdb Out. */

    (void) memmove(&es->CDBByte[0], cdbChars, cdbLength);
    es->SRB_CDBLen = (BYTE) cdbLength;

    /* Ask to copy all the Data In or Out. */

    switch (direction)
        {

        case X2_DATA_OUT:
            es->SRB_BufPointer = (BYTE *) dataChars;
            es->SRB_BufLen = maxLength;
            es->SRB_Flags = SRB_DIR_OUT; /* x10 = SRB_DIR_OUT */
            break;

        case X1_DATA_IN:
            es->SRB_BufPointer = (BYTE *) dataChars;
            es->SRB_BufLen = maxLength;
            es->SRB_Flags = SRB_DIR_IN; /* x08 = SRB_DIR_IN */
            break;

        /* Ask to copy No Data in the last of many possible ways. */

        case X0_DATA_NOT:
        default:
            es->SRB_BufPointer = (BYTE *) dataChars; /* maybe NULL */
            es->SRB_BufPointer = NULL;
            es->SRB_BufLen = 0;
            es->SRB_BufLen = maxLength; /* often 0 */
            es->SRB_Flags = SRB_DIR_OUT|SRB_DIR_IN; /* x18 = SRB_DIR_OUT|SRB_DIR_IN */
            es->SRB_Flags = SRB_DIR_OUT; /* x10 = SRB_DIR_OUT */
            es->SRB_Flags = SRB_DIR_IN; /* x08 = SRB_DIR_IN */
            es->SRB_Flags = 0;
            break;
        }

    /* Ask to count data bytes, if possible. */

    if (aspi->theMayCountDataFlag)
        {
        es->SRB_Flags |= SRB_ENABLE_RESIDUAL_COUNT; /* x04 = ... EN ... RESID */
        }

    /* Ask to copy some Sense data In, if available. */

    es->SRB_SenseLen = ((BYTE) maxSenseLength);

    /* Leave much zeroed. */

    ; /* es->SRB_HaId es->SRB_Target es->SRB_Lun */
    ; /* es->SRB_PostProc */
    ; /* es->SRB_Status es->SRB_HaStat es->SRB_TargStat */
    ; /* es->SRB_Hdr_Rsvd es->SRB_Rsvd1 es->SRB_Rsvd2 es->SRB_Rsvd3 */

    return 0;
    }

/**
**  Call Aspi and wait indefinitely for the call to return.
**
**  Return zero if all ok, else negative.
**/

static void aspiCall(Aspi * aspi)
    {
    DWORD CM_srb;
    AspiCommand * ac = &aspi->theAspiCommand;
    SRB_ExecSCSICmd * es = &ac->theSRB_ExecSCSICmd; /* es = escSrb = SRB_ExecSCSICmd */
    volatile BYTE * pvb = &es->SRB_Status;

    es->SRB_HaId = aspi->theHa;
    es->SRB_Target = aspi->theTarget;
    es->SRB_Lun = aspi->theLun;

    *pvb = SS_PENDING; /* x00 = SS_PENDING */
    
    FILE * fi = aspi->theErrFile;
        
    if (CallScsiMgr ((BYTE*) es, fi))
        return;
     
    do
        GetFromScsiMgr((BYTE*) es, fi);
    while (*pvb == SS_PENDING);

    }

/**
**  Speak a sentence of Scsi through an Aspi connection.
**
**  That is, copy the cdbLength of the cdbChars Out, copy between zero
**  and the maxLength of the dataChars In or Out, copy status In, and
**  copy sense bytes In if available.
**
**  Return zero if all was ok, positive to say specifically how many
**  less than all data bytes were copied, else negative.  Also return
**  zero if all was ok except that the number of data bytes copied is
**  unknown.
**/

INT aspiSay(Aspi * aspi,
        char const * cdbChars, int cdbLength, char * dataChars, INT maxLength, int direction)
    {
    if (aspi == NULL) return -1;

    /* Fail by default. */
    /* And say the data length and sense length are unknown. */

    aspi->theExitInt = -1;
    aspi->theLength = -1;
    aspi->theSenseChars = NULL;
    aspi->theSenseLength = -1;

    /* Remember what to say. */

    int mayCountDataFlag = aspi->theMayCountDataFlag;
    int maxSenseLength = aspi->theMaxSenseLength;
    INT exitInt = aspiSetCommand(aspi,
        cdbChars, cdbLength, dataChars, maxLength, direction);
    if (exitInt != 0) return exitInt;

    /* Say it. */

    aspiCall(aspi);

    /* Fetch more results. */

    AspiCommand * ac = &aspi->theAspiCommand;
    SRB_ExecSCSICmd * es = &ac->theSRB_ExecSCSICmd; /* es = escSrb = SRB_ExecSCSICmd */

    int esStatus = es->SRB_Status;
    int esHaStat = es->SRB_HaStat;
    int esTargStat = es->SRB_TargStat;
    INT esBufLen = es->SRB_BufLen;
    int esSenseLen = es->SRB_SenseLen;

    /* Guess no error reported with error means no error. */
    /* Win98 Aspi requires this. */

    if (esStatus == SS_COMP) /* x01 = SS_COMP */
        {
        esHaStat = HASTAT_OK; /* x00 = HASTAT_OK */
        esTargStat = 0x00; /* x00 = STATUS_GOOD */
        }

    /* Accept x12 HASTAT_DO_DU as a not-specific complaint of residue. */
    /* Tbd: list where x12 HASTAT_DO_DU occurs. */

    if (esStatus == SS_ERR) /* x04 = SS_ERR */
    if (esHaStat == HASTAT_DO_DU) /* x12 = HASTAT_DO_DU */
        {
        esHaStat = HASTAT_OK; /* x00 = HASTAT_OK */
        if ((!mayCountDataFlag) || ((maxLength - esBufLen) == 0))
            {
            esBufLen = 0;
            mayCountDataFlag = 1;
            }
        }

    /* Guess timeout nonsensically reported with sense means no timeout. */
    /* Tbd: list where sense with timeout occurs. */

    if (esStatus == SS_ERR) /* x04 = SS_ERR */
    if (esHaStat == HASTAT_SEL_TO) /* x11 = HASTAT_SEL_TO */
    if (esTargStat == 0x02) /* x02 = STATUS_CHKCOND */
        {
        esHaStat = HASTAT_OK; /* x00 = HASTAT_OK */
        }

    /* Say whether the (esStatus, esHaStat, esTargStat) is meaningful. */
    /* Disregard (esHaStat, esTargStat) with SS_COMP e.g. Win98 Aspi. */

    int meaningful = 0;
    if (esStatus == SS_COMP) /* x01 = SS_COMP */
        {
        meaningful = 1;
        }
    if (esStatus == SS_ERR) /* x04 = SS_ERR */
    if (esHaStat == HASTAT_OK) /* x00 = HASTAT_OK */
    if (esTargStat == 0x02) /* x02 = STATUS_CHKCOND */
        {
        meaningful = 1;
        }

    /* Share & assess the results. */

    exitInt = -1;
    if (meaningful)
        {
        exitInt = 0;
        if (mayCountDataFlag)
            {
            exitInt = -1;
            INT residue = esBufLen;
            if ((0 <= residue) && (residue <= maxLength))
                {
                aspi->theLength = (maxLength - residue);
                exitInt = residue;
                }
            }
        if (esStatus == SS_ERR) /* x04 = SS_ERR */
        if (esHaStat == HASTAT_OK) /* x00 = HASTAT_OK e.g. exclude x14 = HASTAT_PHASE_ERR */
        if (esTargStat == 0x02) /* x02 = STATUS_CHKCOND */
            {
            int senseLength = esSenseLen;
            if (0 < exitInt) exitInt = -X0100_RESIDUE;
            exitInt = -((-exitInt) | X0002_SENSE);

            if ((0 <= senseLength) && (senseLength <= maxSenseLength))
                {
                char const * senseChars = (char const *) &es->CDBByte[cdbLength];
                aspi->theSenseChars = senseChars;
                aspi->theSenseLength = senseLength;
                }
            else
                {
                exitInt = -1; /* sense not intelligible */
                }
            }
        }

    /* Proceed. */

    aspi->theExitInt = exitInt;
    if (exitInt == -1)
        {
        FILE * fi = aspi->theErrFile;
        (void) fprintf(fi,
            "// aspiSay: x %02lX:%02lX:%02lX %lX %lX (status BufLen SenseLen)\n",
            (long) es->SRB_Status,
            (long) es->SRB_HaStat,
            (long) es->SRB_TargStat,
            (long) es->SRB_BufLen,
            (long) es->SRB_SenseLen);
        }
    return exitInt;
    }

/**
**  Count the data bytes copied In or Out by aspiSay.
**
**  Guess the elseLength if the actual length of data bytes copied is
**  unknown.  Also if the aspi is null.
**/

INT aspiGetLength(Aspi * aspi, INT elseLength)
    {
    if (aspi == NULL) return elseLength;
    INT length = aspi->theLength;
    if (length < 0) length = elseLength;
    return length;
    }

/**
**  Copy some or all of the sense bytes copied by aspiSay.
**
**  Guess the elseLength were copied if the length copied is unknown,
**  except guess 0 if aspi is null.
**
**  Copy the min of the charsLength with the actual or guessed copy
**  length.
**
**  Return the count of bytes copied.
**/

int aspiGetSense(Aspi * aspi, char * chars, int charsLength, int elseLength)
    {
    if (aspi == NULL) return 0;

    /* Guess the elseLength if the length of sense bytes copied is unknown. */

    int length = aspi->theSenseLength;
    if (length < 0) length = elseLength;

    /* Copy the min of the charsLength with the actual or guessed copy length. */

    int copyLength = length;
    if (charsLength < copyLength) copyLength = charsLength;
    if ((chars != NULL) && (0 < copyLength))
        {
        (void) memmove(chars, aspi->theSenseChars, copyLength);
        }

    /* Return the count of bytes copied. */

    return copyLength;
    }

/**
**  Read the first else the next well-known device path name.
**
**  That is, read names from the list:
**
**      0.0.0 0.0.1 .. 0.1.0 .. 1.0.0 .. 255.255.255
**
**  Except do not exceed theMaxHa, theMaxTarget, theMaxLun.
**
**  Copy a name of up to charsLength to the chars.
**
**  Return the positive length of chars assigned, else zero to say the
**  name was too long (or if chars is null), else negative if no more
**  names exist (or if aspi is null).
**/

int aspiReadName(Aspi * aspi, char * chars, int charsLength)
    {
    if (aspi == NULL) return -1;

    /* Startup if need be. */

    if (aspiStartup(aspi) < 0) return -1;
    if (aspi->theMaxHa < 0) (void) aspiExamine(aspi, aspi->theHa);
    if (aspi->theMaxHa < 0) return -1;

    /* Buffer a name. */

    char name[] = "255.255.255";

    /* Run til break. */

    for (;;)
        {

        /* Read a "$ha.$target.$lun" name. */

        ++aspi->theLun;
        if (aspi->theMaxLun < aspi->theLun)
            {
            aspi->theLun = 0;
            ++aspi->theTarget;
            if (aspi->theMaxTarget < aspi->theTarget)
                {
                aspi->theTarget = 0;
                ++aspi->theHa;

                /* Else read past the last name. */

                if (aspi->theMaxHa < aspi->theHa) return -1;

                /* Try to get hints once per Ha. */

                (void) aspiExamine(aspi, aspi->theHa);
                if (aspi->theMaxHa < aspi->theHa) return -1;
                }
            }

        int ok = 0;
        if ((0 <= aspi->theHa) && (aspi->theHa<= aspi->theMaxHa))
        if ((0 <= aspi->theTarget) && (aspi->theTarget<= aspi->theMaxTarget))
        if ((0 <= aspi->theLun) && (aspi->theLun <= aspi->theMaxLun))
            {
            ok = 1;
            }
        if (!ok)
            {
            FILE * fi = aspi->theErrFile;
            (void) fprintf(fi, "aspiReadName: impossible name read");
            return -1;
            }

        /* Print the "$ha.$target.$lun" name. */

        (void) sprintf(&name[0], "%d.%d.%d",
            aspi->theHa, aspi->theTarget, aspi->theLun);

        break;
        }

    /* Copy the name if it fits. */

    int copyLength = (strlen(name) + 1);

    if (chars != NULL)
    if (copyLength <= charsLength)
        {
        (void) memmove(chars, &name[0], copyLength);
        return copyLength;
        }

    /* Else copy nothing and return zero. */

    return 0;
    }

/**
**  Accept a -X option of a Scsi command line.
**
**  Return zero if ok, else negative.
**/

int aspiSwallowArg(Aspi * aspi, char const * arg)
    {
    if (aspi == NULL) return -1;
    if (arg == NULL) return -1;

    if (stricmp(arg, "DOS") == 0)
        {
        return 0;
        }

    return -1;
    }

#endif /* DOSASPI */

/* end of file */
