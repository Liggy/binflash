/**
**  dosaspi.h
**  Monday, July 8, 2002
**
**  Define Dos Aspi in terms of Win16 Aspi.
**
**  Except do not correct sizeof SRB_ExecSCSICmd10.CDBByte,
**  thus leave the offsetof SRB_ExecSCSICmd10.SenseArea10 incorrect.
**
**  Note: This file often does not actually appear in the Adaptec SDK,
**  but ../docs/aspidos.txt can be read to mean this.
**/

/* Compile this at most once. */

#ifndef DOSASPI_H
#define DOSASPI_H DOSASPI_H

/* Understand the classic Adaptec SDK in Windows terms. */

#ifdef DJGPP /* Don't need this whole mumbo jumbo in 32 bit mode */
#define far
#define pascal
#endif

#define FAR     far /* sizeof pointer is large */
#define PASCAL  pascal /* caller pushes in order, callee pops */

typedef void            VOID;   /* why use a keyword when you have one */
typedef unsigned char   BYTE;   /* eight bits, unsigned */
typedef unsigned short  WORD;   /* sixteen bits, unsigned */
typedef unsigned long   DWORD;  /* thirty-two bits, unsigned */

/* Include the classic Adaptec SDK. */

#include "winaspi.h"

/* Say how to send down an Srb. */

typedef void FAR ScsiMgr(BYTE FAR *); /* Not WORD FAR PASCAL */

/* Name the struct (SRB_Cmd ... CDBByte) common to all ESC SRB's. */

typedef SRB_ExecSCSICmd6 SRB_ExecSCSICmd;

#endif /* DOSASPI_H */

/* end of file */
