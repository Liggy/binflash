; Program  : addDev.asm
; Licence  : Freeware
; Purpose  : Load a driver without using config.sys
; Language : MASM > 4.0, TASM
; Source   : DOS 9/91 S.266ff
; URL      : http://www-user.tu-chemnitz.de/~heha/cgi-bin/viewzip.cgi/hs_freeware/dosmisc.zip/SRC/ADDEV.ASM
;
; Assemble with:
;     MASM                         TASM
; masm addDev,,,;               tasm addDev,,,;
; link addDev;                  tlink addDev /t
; exe2bin addDev addDev.com


code segment byte
     assume cs:code,ds:code
     assume es:code,ss:code
org 100h
start: jmp init
           ; Pseudostapel
           dw 0020h dup (?)
stack_:    dw ?
           ; Daten- und Variablenbereich
msg1       db 10,13
           db 'AddDev V1.0    (c)  DOS '
           db 'international 9/91     ',10,13
           db 10,'$'
           ; /!\ Those constants must have fixed length /!\
           ; /!\ or delDev will get very confused...    /!\
msg2       db 'Driver not installed.     $'
msg3       db 'Driver installed.   $'
msg4       db 'Wrong DOS-Version.  $'
dibseg     dw ?
dibofs     dw ?
blkdevadr  dw ?
lastdrvadr dw ?
nuldevadr  dw ?
cdsbase    dw ?
cdssize    dw ?
parblock   dw ?,?
intvekseg  dw ?
databk     db 11h,-1,00h,0fh dup (00h)
           dw offset filename,?,?
call1      dw ?,?
call2      dw ?,?
lastadr    dw ?,?
regds      dw ?
reges      dw ?
regss      dw ?
endadr     dw ?
dosver     db ?
cdsptr     dw ?,?
filename   db 128 dup (-1)
anzblkdev  db ?
unit       db ?
intadr     dw ?
           ; Treiber laden und installieren
loaddrv    proc   near
           push   cs
           pop    es
           cld
           mov    si,0080h
           lodsb
           ; Dateiname des Treibers holen
           or     al,al
           jz     error
           call   ignspc
           jc     error
           mov    di,offset filename
getfile:   lodsb
           cmp    al,20h
           jz     fileok
           cmp    al,0dh
           jz     fileok
           stosb
           jmp    short getfile
fileok:    dec    si
           xor    al,al
           stosb
           push   di
           call   ignspc
           jc     paramsok
           ; Treiberparameter holen
getparams: lodsb
           cmp    al,0dh
           jz     paramsok
           stosb
           jmp    short getparams
paramsok:  mov    al,0dh
           stosb
           push   ds
           push   es
           ; Treiber als Overlay laden
           mov    bx,offset parblock
           mov    dx,offset filename
           mov    ax,4b03h
           int    21h
           pop    es
           pop    ds
           jnc    loadok
           pop    di
           ; bei Auftreten eines Fehlers:
           ; Speicher freigeben und zum DOS
error:     push   cs
           pop    ds
           mov    ah,09h
           mov    dx,offset msg2
           int    21h
           mov    es,intvekseg
           mov    ah,49h
           int    21h
           mov    ax,4c00h
           int    21h
loadok:    pop    di
           mov    byte ptr [di-1],20h
           mov    ax,parblock
           mov    es,ax
           ; Adressen der Strategie- und
           ; Interrupt-Routine ermitteln
           mov    ax,es:[6]
           mov    call1,ax
           mov    ax,es:[8]
           mov    call2,ax
           mov    si,offset databk
           ; Parameter-Block vorbereiten
           mov    [si+14h],cs
           mov    es,dibseg
           mov    di,blkdevadr
           mov    al,es:[di]
           mov    [si+16h],al
           push   cs
           pop    es
           mov    bx,offset databk
           push   ds
           ; Strategieroutine aufrufen
           call   dword ptr cs:call1
           ; Interruptroutine aufrufen
           call   dword ptr cs:call2
           ; Treiber installieren
           pop    ds
           mov    si,offset databk
           mov    ax,[si+3]
           test   ah,80h
           jz     drvinitok
errinit:   xor    ax,ax
           mov    es,ax
           xor    di,di
           xor    si,si
           mov    ds,intvekseg
           cld
           mov    cx,0200h
           cli
           rep    movsw
           sti
           jmp    error
drvinitok: les    bx,[si+0eh]
           mov    lastadr,bx
           mov    lastadr+2,es
           add    bx,0020h
           mov    intadr,bx
           ; Interruptvektoren nach
           ; énderungen dursuchen
           cli
           mov    regds,ds
           mov    reges,es
           mov    regss,ss
           mov    ax,es
           mov    ss,ax
           mov    dx,bx
           inc    bx
           mov    es,intvekseg
           xor    ax,ax
           mov    ds,ax
           xor    si,si
           xor    di,di
           xor    cx,cx
chkint:    mov    ax,[si]
           cmp    ax,es:[di]
           jnz    modified
           mov    ax,[si+2]
           cmp    ax,es:[di+2]
           jz     intok
modified:  inc    ch
           mov    ss:[bx],cl
           mov    ax,[si]
           mov    ss:[bx+1],ax
           mov    ax,[si+2]
           mov    ss:[bx+3],ax
           mov    ax,es:[di]
           mov    ss:[bx+5],ax
           mov    ax,es:[di+2]
           mov    ss:[bx+7],ax
           add    bx,9
intok:     add    si,4
           add    di,4
           inc    cl
           jnz    chkint
           xchg   bx,dx
           mov    ss:[bx],ch
           mov    ss,cs:regss
           mov    es,cs:reges
           mov    ds,cs:regds
           sti
           add    dx,0020h
           mov    endadr,dx
           mov    ax,parblock
           mov    es,ax
           mov    ax,es:[4]
           test   ah,80h
           jz     blockdev
           jmp    moddvchan
           ; Speziealbehandlung fÅr Block-
           ; treiber durchfÅhren
blockdev:  mov    es,dibseg
           mov    bx,blkdevadr
           mov    ah,es:[bx]
           mov    anzblkdev,ah
           mov    cl,databk+0dh
           add    ah,cl
           mov    bx,lastdrvadr
           cmp    ah,es:[bx]
           jbe    anzdrvok
           ; Fehler, wenn zu viele GerÑte
           jmp    error
anzdrvok:  xor    ch,ch
           mov    unit,00h
patchok:   mov    bx,cdsbase
           mov    es,dibseg
           les    bx,es:[bx]
           mov    cdsptr,bx
           mov    cdsptr+2,es
           lds    si,dword ptr databk+18
           mov    es,word ptr cs:databk+16
           mov    bp,cs:endadr
patchloop: push   cx
           push   ds
           push   si
           mov    si,[si]
           push   es
           push   bp
           push   es
           push   bp
           ; BPB in DPB konvertieren
           mov    ah,53h
           int    21h
           pop    bp
           pop    es
           push   es
           push   bp
           ; GerÑtebezeichnung und Subunit
           ; in DPB eintragen
           mov    al,cs:anzblkdev
           mov    es:[bp],al
           mov    al,cs:unit
           mov    es:[bp+1],al
           push   bp
           cmp    cs:dosver,4
           jb     adrok
           inc    bp
adrok:     mov    word ptr es:[bp+12h],0
           ; Treiberadr. in DPB eintragen
           mov    ax,cs:parblock
           mov    es:[bp+14h],ax
           mov    byte ptr es:[bp+23],-1
           mov    word ptr es:[bp+24],-1
           mov    word ptr es:[bp+26],-1
           pop    bp
           cli
           mov    ss,cs:dibseg
           mov    bx,cs:dibofs
           ; letztes DPB suchen und neuen
           ; DPB in DPB-Liste einfÅgen
dpbloop:   mov    dx,ss:[bx]
           mov    ss,ss:[bx+2]
           mov    bx,dx
           cmp    cs:dosver,4
           jb     adrok1
           inc    bx
adrok1:    add    bx,0018h
           cmp    word ptr ss:[bx],-1
           jz     lastdpb
           jmp    short dpbloop
lastdpb:   mov    ss:[bx],bp
           mov    ss:[bx+2],es
           mov    ax,cs
           mov    ss,ax
           sti
           ; CDS-Eintrag initialisieren
           mov    al,cs:anzblkdev
           xor    ah,ah
           mov    dx,cs:cdssize
           mul    dx
           mov    bx,cs:cdsptr
           add    bx,ax
           mov    ds,cs:cdsptr+2
           mov    word ptr [bx+43h],4000h
           pop    word ptr [bx+45h]
           pop    word ptr [bx+47h]
           mov    word ptr [bx+49h],-1
           mov    word ptr [bx+4bh],-1
           mov    word ptr [bx+4dh],-1
           mov    word ptr [bx+4fh],2
           cmp    cs:dosver,4
           jb     cdsok
           mov    byte ptr [bx+51h],0
           mov    word ptr [bx+52h],0
           mov    word ptr [bx+54h],0
           mov    byte ptr [bx+56h],0
cdsok:     pop    bp
           pop    es
           add    bp,0022h
           pop    si
           pop    ds
           inc    cs:anzblkdev
           inc    cs:unit
           add    si,02h
           pop    cx
           dec    cx
           jz     endpatch
           ; fortfahren, wenn Treiber
           ; mehrere GerÑte unterstÅtzt
           jmp    patchloop
endpatch:  push   cs
           pop    ds
           mov    endadr,bp
           mov    es,dibseg
           mov    bx,blkdevadr
           ; Anz. GerÑte in DIB schreiben
           mov    al,anzblkdev
           mov    es:[bx],al
moddvchan: mov    es,dibseg
           mov    di,nuldevadr
           ; Treiber in Treiberliste einf.
           push   es:[di]
           push   es:[di+2]
           mov    bx,parblock
           cld
           xor    ax,ax
           stosw
           mov    ax,bx
           stosw
           mov    ds,bx
           pop    ds:[2]
           pop    ds:[0]
           push   cs
           pop    ds
           ; Speicherbereiche freigeben
           mov    es,intvekseg
           mov    ah,49h
           int    21h
           mov    ah,09h
           mov    dx,offset msg3
           int    21h
           mov    dx,endadr
           mov    cl,04h
           shr    dx,cl
           inc    dx
           add    dx,lastadr+2
           mov    ax,cs
           sub    dx,ax
           inc    dx
           mov    ax,3100h
           int    21h
loaddrv    endp
ignspc     proc   near
           ; Leerzeichen Åberlesen
           mov    al,[si]
           cmp    al,20h
           jz     ignore
           cmp    al,0dh
           jz     noparams
           clc
           ret
ignore:    inc    si
           jmp    short ignspc
noparams:  stc
           ret
ignspc     endp
           ; Initialisierung
init       proc   near
           cli
           mov    ax,cs
           mov    ds,ax
           mov    es,ax
           mov    ss,ax
           mov    sp,offset stack_
           sti
           mov    ah,09h
           mov    dx,offset msg1
           int    21h
           mov    es,cs:[002ch]
           mov    ah,49h
           int    21h
           mov    ah,30h
           int    21h
           mov    dosver,al
           cmp    al,03h
           jae    dosvok
           jmp    baddos
dosvok:    push   ax
           mov    ah,52h
           int    21h
           mov    dibseg,es
           mov    dibofs,bx
           pop    ax
           xchg   ah,al
           cmp    ax,0300h
           jz     dosv30
           cmp    ah,03h
           jnz    dosv4x
dosv3x:    push   bx
           add    bx,0020h
           mov    blkdevadr,bx
           inc    bx
           mov    lastdrvadr,bx
           inc    bx
           mov    nuldevadr,bx
           pop    bx
dosv3:     add    bx,0016h
           mov    cdsbase,bx
           mov    cdssize,51h
           jmp    short initok
dosv30:    push   bx
           add    bx,0010h
           mov    blkdevadr,bx
           add    bx,000bh
           mov    lastdrvadr,bx
           add    bx,000dh
           mov    nuldevadr,bx
           pop    bx
           jmp    short dosv3
dosv4x:    push   bx
           add    bx,0020h
           mov    blkdevadr,bx
           inc    bx
           mov    lastdrvadr,bx
           inc    bx
           mov    nuldevadr,bx
           pop    bx
           add    bx,0016h
           mov    cdsbase,bx
           mov    cdssize,58h
initok:    push   cs
           pop    es
           mov    ah,4ah
           mov    bx,offset init
           mov    cl,04h
           shr    bx,cl
           inc    bx
           mov    dx,cs
           add    dx,bx
           mov    parblock,dx
           mov    parblock+2,dx
           mov    call1+2,dx
           mov    call2+2,dx
           push   bx
           int    21h
           mov    ah,48h
           mov    bx,-1
           int    21h
           sub    bx,0400h
           pop    ax
           add    bx,ax
           mov    ah,4ah
           int    21h
           mov    ax,5800h
           int    21h
           push   ax
           mov    ax,5801h
           mov    bx,0002h
           int    21h
           mov    ah,48h
           mov    bx,0040h
           int    21h
           mov    intvekseg,ax
           pop    bx
           mov    ax,5801h
           int    21h
           cld
           push   ds
           push   es
           mov    es,intvekseg
           xor    ax,ax
           mov    ds,ax
           xor    si,si
           xor    di,di
           mov    cx,0200h
           cli
           rep    movsw
           sti
           pop    es
           pop    ds
           jmp    loaddrv
baddos:    mov    ah,09h
           mov    dx,offset msg4
           int    21h
           mov    ax,4c00h
           int    21h
init       endp
code       ends
           end    start
