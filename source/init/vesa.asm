initizeVESA: 
	push  cs
	pop   ds
	mov   ax,ds
	mov   es,ax
	;call dbg
	;------------------------------------------------------------
	; Purpose: Checks to see if VESA is available and if the desired mode is
	; available.
	; Inputs:  None
	; Outputs: VESAInfo and VESA_Info structures filled (if successful)
	;------------------------------------------------------------
	mov   dword [VESAInfo_Signature],'VBE2'
	mov   ax,4f00h			       ; Is Vesa installed ?                       
	mov   di,VESA_Info		       ; This is the address of how info block.
	int   10h
	
	cmp   ax,004Fh			       ; Is vesa installed ?,
	jne   near NoVesa2		       ; If not print a mesage & quit.

	;call dbg

	mov   ax,4f01h			       ; Get Vesa Mode information. 
	mov   di,Mode_Info		       ; This is the address of how info block.
	mov   cx,0x4115 		       ; 4112h = 32/24bit ; 0x4101 = 256bit ;4111h = 65535bit (640*480)
	and   cx,0xfff
	int   10h
	
	;call dbg

	cmp   dword [VESAInfo_Signature], 'VESA'
	jne   near NoVesa2
	
	;call dbg

	cmp   byte [VESAInfo_Version+1], 2
	jb    NoVesa2			       ; VESA version below 2.0

	;call dbg

	;set mode: 
	mov   ax,4f02h			       ; set vesa screen mode
	mov   bx,0x4115 		       ; 4112h = 32/24bit
	int   10h
ret



bb_print:
     lodsb
     or al,al 
     jz .done
     mov ah,0Eh
     int 10h
     jmp bb_print
     .done:
ret

dbg: 
	mov si,b_dbg
	call bb_print
ret

NoVesa2: 
	mov si,b_novesa
	call bb_print
jmp NoVesa2

b_novesa: db 'NO VESA!!!!',13,10,0
b_dbg: db 'ZOOM!',13,10,0
