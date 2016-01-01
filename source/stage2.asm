%define stage2Base 0x0800
%define stage2Size 9
%define rdStartSector 0x50
%define rdBase 0x4000
%define rdSize 0x1
%define kernelBase 0x1000
%define kernelSize 0x30
%define BASE 0x8000
%define PD_START 0x0A000
	
%define stackTopPhys 0x40000

K_BASE equ 0xC0000000
;K_INDEX equ K_BASE >> 22
K_INDEX equ 0x300

[bits 16]
[ORG 8000h]
	
	call initizeVESA	

	mov eax,[ModeInfo_PhysBasePtr]
	mov [t_vidmem+BASE],eax
	
	;; Load Ramdisk: 
	;; Reset floppy drive (just for fun?)
          mov dx, 0x3F2
          mov al, 0
          out dx, al

	;; Read in ramdisk
	
	mov ax,rdBase
	mov es,ax
	mov bx,0000h
	mov cx,rdSize
	mov ax,rdStartSector
	call read_loop
	
	;; Load Kernel: 
	;; Reset floppy drive (just for fun?)
          mov dx, 0x3F2
          mov al, 0
          out dx, al

	;; Read in kernel
	
	mov ax,kernelBase
	mov es,ax
	mov bx,0000h
	mov cx,kernelSize
	mov ax,1+stage2Size
	call read_loop
	
	;; Floppy off
	
	mov dx, 0x3F2
	mov al, 0
	out dx, al
	
	;; End load kernel
	
	cli
	xor ax,ax
	mov ds,ax
	
	lgdt [gdtr]

	mov eax,cr0		; Copy cr0->eax
	or eax,1		; Set the last bit
	mov cr0,eax		; and put it back into cr0
	jmp code_selector:flush

t_vidmem: dd 0
	
%include 'init/vesa.asm'
%include 'init/vesainfo.inc'
%include 'init/gdt.asm'
%include 'init/loader.asm'
	
[bits 32]

flush:
	mov ax,data_selector
	mov ds,ax
	mov fs,ax
	mov gs,ax
	mov es,ax
	mov ss,ax
	mov esp,stackTopPhys
	jmp code_selector:.init

.init:
	;; 20090822 TESTING CODE
	;mov eax,VESA_Info
	;mov eax,Mode_Info
	;mov eax,0x12345678
;	mov eax,[t_vidmem]
	;xor eax,eax
	;mov ax,cs
;	mov ecx,0
;.ello:
;	mov edx,eax
;	and edx,0xF
;	mov bl,[adf+edx]
;	mov [0xb8000+2*ecx],bl
;	mov [0xb8001+2*ecx],byte 0x0F
;	shr eax,4
;	inc ecx
;	cmp ecx,8
;	jb .ello
	
;        jmp asd
	
;adf:	dd '0123456789ABCDEF'

;asd:	
	;; END 20090822 TESTING CODE
	
	;; Set up page tables:
	
	;; Zero pagedir
	;mov eax,[t_vidmem]
	;mov [eax],dword 0xFFFFFFFF
	;hlt
	mov eax,pd
	mov ecx,0
	
	.loop1:
		mov [eax+4*ecx],dword 0
		inc ecx
		cmp ecx,1024
		jb .loop1

;;; ID-map first 4MB
	;; get to the spot where the first PT will live
	mov ebx,pd+0x1000
	;; Write the privilege bits
	mov edx,ebx
	or edx,3
	mov [eax+0],edx
	;; Write the table:
	mov ecx,0
	.loop2:
		mov edx,ecx
		shl edx,12
		or edx,3
		mov [ebx+4*ecx],edx
		inc ecx
		cmp ecx,1024	;Are we at the end of the first PT (the 4MB mark)?
		jb .loop2

;;;  Translate-map the kernel
	;; Get to second PT
	mov ebx,pd+0x2000
	;; Write permission bits
	mov eax,pd
	mov edx,ebx
	or edx,3
	mov ecx,0xC00
	mov [eax+ecx],edx
	;; Write the table
	;; Want to map 0th entry to 0x10000 etc, so will use 
	mov ecx,0
	mov eax,0x10000
	.loop3:
		mov edx,eax
		or edx,3
		mov [ebx+4*ecx],edx
		add eax,0x1000
		inc ecx
		cmp ecx,1024
		jb .loop3
	
;;;  Map in kernel's space
	;; Get to second PT
	mov ebx,pd+0x3000
	;; Write permission bits
	mov eax,pd
	mov edx,ebx
	or edx,3
	mov ecx,0xC10
	mov [eax+ecx],edx
	;; Write the table
	;; Want to map 0th entry to 0x30000 etc, so will use 
	mov ecx,0
	mov eax,0x100000
	.loop5:
		mov edx,eax
		or edx,3
		mov [ebx+4*ecx],edx
		add eax,0x1000
		inc ecx
		cmp ecx,1024
		jb .loop5

;;;  Map in vidmem
	;; Get to third PT
	mov ebx,pd+0x4000
	;; Write permission bits
	mov eax,pd
	mov edx,ebx
	or edx,3
	;mov ecx,0xF80
	;; New 20090822 (to hopefully generalize the code to handle any PhysBasePtr value...)
	mov ecx,[t_vidmem]
	shr ecx,20
	;; End new 20090822
	mov [eax+ecx],edx
	;; Write the table
	;; Want to map 0th entry to 0x10000 etc, so will use 
	mov ecx,0
	;mov eax,0xF8000000
	mov eax,[t_vidmem]
	.loop4:
		mov edx,eax
		or edx,3
		mov [ebx+4*ecx],edx
		add eax,0x1000
		inc ecx
		cmp ecx,1024
		jb .loop4
;; Enable paging

	mov eax,pd
	mov cr3,eax
	
	mov eax,cr0
	or eax,0x80000000
	mov cr0,eax
	
;; TEST
	;mov eax,[ModeInfo_PhysBasePtr+BASE]
	;mov eax,[ModeInfo_PhysBasePtr+BASE]
	;mov ecx,0
	;.lllll: 
	;	mov [eax+4*ecx],dword 0xCCCCCCCC
	;	inc ecx
	;	cmp ecx,1200
	;	jb .lllll
	;mov eax, dword 0xF0000000
	;mov [eax],dword 0xFFFFFFFF
	;hlt
;; Call kernel:
	; Pass kernel the start of vidmem
	push dword [t_vidmem]
	lea ecx,[K_BASE]
	jmp ecx
	
align 0x1000
pd:
