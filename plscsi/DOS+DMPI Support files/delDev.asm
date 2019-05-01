; Program  : delDev.asm
; Licence  : Freeware
; Purpose  : Unload a driver loaded with adddev
; Language : MASM > 4.0, TASM
; Source   : DOS 9/91 S.266ff
; URL      : http://www-user.tu-chemnitz.de/~heha/cgi-bin/viewzip.cgi/hs_freeware/dosmisc.zip/SRC/DELDEV.ASM
;
; Assemble with:
;       MASM                          TASM
; masm delDev,,,;                 tasm delDev,,,;    
; link delDev;                    tlink delDev /t
; exe2bin delDev.exe delDev.com
;
code       segment byte
           assume cs:code,ds:code
           assume es:code,ss:code
           org 0100h
start:     jmp init
           ;Pseudostack
           dw 80h dup (?)
stack_     dw ?
           ; Daten- und Variablenbereich
msg1       db 10,13
           db 'DelDev V1.0    (c)  DOS '
           db 'international 10/91    ',10,13
           db 10,'$'
msg2       db 'Interrupt overlay detected!'
           db 10,13
msg3       db 'Driver not removed.$'
msg4       db 'Wrong DOS Version.$'
msg5       db 10,13,'Program terminated '
           db 'prematurely.$'
msg6       db 'Driver removed.$'
dibofs     dw ?
dibseg     dw ?
blkdevadr  dw ?
nuldevadr  dw ?
cdsbase    dw ?
cdssize    dw ?
dosver     db ?
cdsptr     dw ?,?
anzdev     db ?
tabseg     dw ?
drive      db ?
devname    db 20h dup (0aah)
devlen     db ?
devadr     dw ?,?
devflg     db ?
firstdrv   db ?
anzdrv     db ?
intofs     dw ?
intseg     dw ?
           ; Vorzeitiges Programmende

quit       proc    near
           mov     ah,09h
           mov     dx,offset msg5
           int     21h
           mov     ax,4c00h
           int     21h
quit       endp
           ; Hauptprogramm
main       proc    near
           cld
           mov     ah,48h
           mov     bx,1000h
           int     21h
           jc      quit
           ; alle nachtrÑglich eingebundenen
           ; GerÑtetreiber scannen
           mov     tabseg,ax
           mov     anzdev,0
           mov     es,ax
           xor     di,di
           mov     bx,nuldevadr
           mov     ds,dibseg
           lds     si,[bx]
devloop:   push    ds
           push    si
           mov     ax,ds
           sub     ax,59h
           mov     ds,ax
           ; /!\ ADDRESSES BELOW MUST MATCH THE /!\ 
           ; /!\ DATA SECTION OF ADDDDEV.ASM    /!\
           cmp     word ptr [si+17ah],"rD"
           jnz     nodev
           cmp     word ptr [si+17ch],"vi"
           jnz     nodev
           cmp     word ptr [si+17eh],"re"
           jnz     nodev
           cmp     word ptr [si+182h],"to"
           jz      devfound
nodev:     jmp     nextdev
devfound:  add     si,0204h
           cld
           push    si
           xor     cl,cl
chkloop:   lodsb
           cmp     al,20h
           jz      endoffile
           cmp     al,"\"
           jnz     chkloop
           inc     cl
           jmp     short chkloop
endoffile: pop     si
           or      cl,cl
           jz      nameok
ignloop:   lodsb
           cmp     al,"\"
           jnz     ignloop
           dec     cl
           jnz     ignloop
nameok:    xor     cl,cl
           mov     bx,di
           inc     di
copyname:  lodsb
           cmp     al,20h
           jz      endcopy
           call    upcase
           stosb
           inc     cl
           jmp     short copyname
endcopy:   mov     byte ptr es:[di],"$"
           inc     di
           mov     es:[bx],cl
           mov     si,01e9h
           mov     dh,[si]
           pop     si
           pop     ds
           push    ds
           push    si
           mov     es:[di],si
           mov     es:[di+2],ds
           mov     byte ptr es:[di+4],00h
           test    word ptr [si+04h],8000h
           jnz     chardev
           mov     byte ptr es:[di+4],-1
           add     dh,41h
           mov     es:[di+5],dh
           add     dh,[si+0ah]
           dec     dh
           mov     es:[di+6],dh
chardev:   add     di,07h
           inc     cs:anzdev
nextdev:   pop     si
           pop     ds
           mov     bx,si
           lds     si,[bx]
           cmp     si,-1
           jz      enddevlop
           jmp     devloop
enddevlop: mov     ax,cs
           mov     ds,ax
           mov     es,ax
           cmp     anzdev,0
           jnz     devok
           mov     ax,4c00h
           int     21h
devok:     mov     drive,-1
           mov     si,0080h
           xor     cx,cx
           mov     di,offset devname
           lodsb
           or      al,al
           jnz     parpresnt
           ; keine oder falsche Parameter
           ; angegeben
exit:      call    devout
           mov     ax,4c00h
           int     21h
parpresnt: lodsb
           cmp     al,0dh
           jz      exit
           cmp     al,20h
           jz      parpresnt
           dec     si
parloop:   lodsb
           cmp     al,0dh
           jz      endofpar
           cmp     al,20h
           jz      getdrv
           call    upcase
           stosb
           inc     cl
           jmp     short parloop
getdrv:    lodsb
           cmp     al,0dh
           jz      endofpar
           cmp     al,":"
           jnz     getdrv
           mov     al,[si-2]
           call    upcase
           mov     drive,al
endofpar:  mov     devlen,cl
           or      cl,cl
           jz      exit
           ; gewÅnschten Treiber suchen
           mov     es,tabseg
           xor     di,di
           mov     si,offset devname
           mov     ch,anzdev
searchdev: push    cx
           push    si
           push    di
           cmp     cl,es:[di]
           jnz     notfound
           xor     ch,ch
           inc     di
           repz    cmpsb
           jz      found
notfound:  pop     di
           mov     al,es:[di]
           xor     ah,ah
           add     di,ax
           add     di,09h
           pop     si
           pop     cx
           dec     ch
           jnz     searchdev
           jmp     exit
found:     inc     di
           cmp     byte ptr es:[di+4],0
           jz      foundok
           mov     al,drive
           cmp     al,es:[di+5]
           jnz     notfound
foundok:   pop     di
           pop     si
           pop     cx
           mov     al,es:[di]
           xor     ah,ah
           add     di,ax
           add     di,02h
           lds     si,es:[di]
           mov     cs:devadr,si
           mov     cs:devadr+2,ds
           mov     ax,ds
           sub     ax,59h
           mov     ds,ax
           mov     ax,ds:[0286h]
           mov     cs:intofs,ax
           mov     ax,ds:[01f5h]
           mov     cs:intseg,ax
           mov     al,es:[di+4]
           mov     cs:devflg,al
           mov     al,es:[di+5]
           mov     cs:firstdrv,al
           mov     ah,es:[di+6]
           sub     ah,al
           inc     ah
           mov     cs:anzdrv,ah
           ; prÅfen, ob verÑnderte Interrupts
           ; vorliegen
           lds     si,dword ptr cs:intofs
           xor     ax,ax
           mov     es,ax
           lodsb
           mov     cl,al
           xor     ch,ch
           or      al,al
           jnz     chkint
           jmp     delete
chkint:    lodsb
           xor     ah,ah
           shl     ax,1
           shl     ax,1
           mov     di,ax
           cmpsw
           jnz     errint
           cmpsw
           jnz     errint
           add     si,04h
           loop    chkint
           jmp     delete
           ; Treiber kann nicht entfernt
           ; werden, da anderes Programm
           ; die geÑnderten Int.-Vektoren
           ; nochmals geÑndert hat !!!
errint:    push    cs
           pop     ds
           mov     ah,09h
           mov     dx,offset msg2
           int     21h
           mov     ax,4c00h
           int     21h
delete:    mov     si,cs:intofs
           lodsb
           or      al,al
           jz      noints
           mov     cl,al
           xor     ch,ch
           cli
putint:    lodsb
           xor     ah,ah
           shl     ax,1
           shl     ax,1
           mov     di,ax
           add     si,04h
           movsw
           movsw
           loop    putint
           sti
noints:    mov     ax,cs
           mov     ds,ax
           mov     es,ax
           cmp     devflg,00h
           jnz     special
           jmp     deldev
           ; Spezialbehandlung fÅr
           ; BlockgerÑtetreiber
special:   mov     cl,firstdrv
           sub     cl,42h
           xor     ch,ch
           push    cx
           ; 1.) Drive Parameter Blocks
           ;     aus DPB-Kette ausklinken
           les     di,dword ptr dibofs
           les     di,es:[di]
           or      cl,cl
           jz      dpbfound
nextdpb:   cmp     dosver,4
           jb      nextok
           inc     di
nextok:    les     di,es:[di+18h]
           loop    nextdpb
dpbfound:  add     di,0018h
           cmp     dosver,4
           jb      nextok1
           inc     di
nextok1:   mov     ax,-1
           stosw
           stosw
           pop     ax
           inc     ax
           ; 2.) énderungen in der Current
           ;     Directory Structure CDS
           ;     rÅckgÑngig machen
           mul     cdssize
           les     di,dword ptr cdsptr
           add     di,ax
           mov     cl,anzdrv
           xor     ch,ch
cdsloop:   push    cx
           push    di
           add     di,03h
           xor     al,al
           mov     cx,70
           rep     stosb
           pop     di
           pop     cx
           mov     word ptr es:[di+43h],0
           mov     word ptr es:[di+45h],0
           mov     word ptr es:[di+47h],0
           mov     word ptr es:[di+49h],-1
           mov     word ptr es:[di+4bh],-1
           mov     word ptr es:[di+4dh],-1
           mov     word ptr es:[di+4fh],2
           cmp     dosver,4
           jb      nextcds
           mov     byte ptr es:[di+51h],0
           mov     word ptr es:[di+52h],0
           mov     word ptr es:[di+54h],0
           mov     word ptr es:[di+56h],0
nextcds:   add     di,cdssize
           loop    cdsloop
           mov     es,dibseg
           mov     di,blkdevadr
           mov     al,anzdrv
           sub     es:[di],al
           ; Entfernung eines GerÑtetreibers
           ; aus dem RAM ( Zeichen- oder
           ; BlockgerÑtetreiber )
deldev:    lds     bx,dword ptr cs:devadr
           mov     cx,[bx]
           mov     dx,[bx+2]
           mov     ax,ds
           push    cs
           pop     ds
           mov     es,dibseg
           mov     di,nuldevadr
search:    cmp     bx,es:[di]
           jnz     devnotok
           cmp     ax,es:[di+2]
           jz      devnok
devnotok:  les     di,es:[di]
           jmp     short search
           ; Treiber aus DOS-Treiberkette
           ; ausklinken
devnok:    mov     es:[di],cx
           mov     es:[di+2],dx
           mov     dx,offset msg3
           sub     ax,59h
           mov     es,ax
           ; Speicher des Treibers
           ; freigeben
           mov     ah,49h
           int     21h
           jc      ausga
           mov     dx,offset msg6
ausga:     mov     ah,09h
           int     21h
           mov     ax,4c00h
           int     21h
main       endp

upcase     proc    near
           ; Kleinbuchstaben in Gro·buch-
           ; staben wandeln
           cmp     al,"a"
           jb      charok
           cmp     al,"z"
           ja      charok
           sub     al,20h
charok:    ret
upcase     endp

devout     proc    near
           ; alle nachgeladenen Treiber
           ; auf dem Standard-AusgabegerÑt
           ; ausgeben
           mov     cl,anzdev
           xor     ch,ch
           mov     ds,tabseg
           xor     si,si
outloop:   mov     dx,si
           inc     dx
           mov     ah,09h
           int     21h
           mov     al,[si]
           xor     ah,ah
           add     si,ax
           add     si,09h
           mov     ah,02h
           cmp     byte ptr [si-3],0
           jz      noblk
           mov     dl,20h
           int     21h
           int     21h
           mov     dl,[si-2]
           int     21h
           mov     dl,":"
           int     21h
           mov     dh,[si-1]
           cmp     dh,[si-2]
           jz      noblk
           mov     dl,"-"
           int     21h
           mov     dl,dh
           int     21h
           mov     dl,":"
           int     21h
noblk:     mov     dl,0dh
           int     21h
           mov     dl,0ah
           int     21h
           loop    outloop
           push    cs
           pop     ds
           ret
devout     endp
           ; Initialisierung

init       proc    near
           cli
           mov     ax,cs
           mov     ds,ax
           mov     es,ax
           mov     ss,ax
           mov     sp,offset stack_
           sti
           mov     ah,09h
           mov     dx,offset msg1
           int     21h
           mov     ah,30h
           int     21h
           mov     dosver,al
           cmp     al,03h
           jae     dosvok
           jmp     baddos
dosvok:    push    ax
           mov     ah,52h
           int     21h
           mov     dibseg,es
           mov     dibofs,bx
           pop     ax
           xchg    ah,al
           cmp     ax,0300h
           jz      dosv30
           cmp     ah,03h
           jnz     dosv4x
dosv3x:    push    bx
           add     bx,0020h
           mov     blkdevadr,bx
           add     bx,02h
           mov     nuldevadr,bx
           pop     bx
dosv3:     add     bx,0016h
           mov     cdsbase,bx
           mov     cdssize,51h
           jmp     short initok
dosv30:    push    bx
           add     bx,0010h
           mov     blkdevadr,bx
           add     bx,0018h
           mov     nuldevadr,bx
           pop     bx
           jmp     short dosv3
dosv4x:    push    bx
           add     bx,0020h
           mov     blkdevadr,bx
           add     bx,02h
           mov     nuldevadr,bx
           pop     bx
           add     bx,0016h
           mov     cdsbase,bx
           mov     cdssize,58h
initok:    mov     bx,cdsbase
           les     di,es:[bx]
           mov     cdsptr,di
           mov     cdsptr+2,es
           push    cs
           pop     es
           mov     bx,offset init
           mov     cl,04h
           shr     bx,cl
           inc     bx
           mov     ah,4ah
           int     21h
           jmp     main
baddos:    mov     ah,09h
           mov     dx,offset msg4
           int     21h
           mov     ax,4c00h
           int     21h
init       endp
code       ends
           end     start
