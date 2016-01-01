[bits 32]

	
%define STACK_SIZE 0x3000
%define BASE 0xC0000000
	
extern kmain
extern monitor_init
global begin

begin:
	pop eax
	mov [vidmem],eax

	;; Apply a new upstairs GDT so that we don't have to reply on that old one downstairs
	
	;; 20090902: Insert code for filling out TSS entry properly here? 
	
	mov eax,tss_start
	mov [gdt_tss+2],word ax
	shr eax,16
	mov [gdt_tss+4],byte al
	mov [gdt_tss+7],byte ah

	mov eax,tss_end-tss_start+1 ;Or possibly just tss_end?  Depends on whether the limit is the upper bound or the size...
	mov [gdt_tss+0],word ax
	shr eax,16
	mov bl,[gdt_tss+6]
	and al,0x0F
	or bl,al
	mov [gdt_tss+6],byte bl
	
	lgdt [gdtr]

	jmp code_selector:flush

%include 'init/gdt.asm'
	
flush:
	mov ax,data_selector
	mov ds,ax
	mov fs,ax
	mov gs,ax
	mov es,ax
	mov ss,ax
	;mov esp,stack+STACK_SIZE
	jmp code_selector:.init

.init:

	mov ax,0x2B
	ltr ax
	
	call pic_remap
	
	mov al,0xFF
	out 0x21,al
	out 0xA1,al
	lidt [idtr]
	sti
	;; What is this code for?
	mov cl,1
	mov ah,1
	shl ah,cl
	xor ah,0xFF

	in al, 0x21
	and al, ah
	out 0x21, al
	;nop;Causes triple fault at 0xC00000a7 (?!)  Remove to make OK again (!?!)
	push dword tss_start
	push dword 0x40000
	push dword [vidmem]
	call kmain
	;mov eax,0x7000000
	;mov [eax],dword 0xff
	;mov eax,0xC1000000
	;mov [eax],dword 0xff
	jmp $

%include 'init/pic.asm'	
%include 'init/isr.asm'
%include 'init/idt.asm'

tss_phys_start:	dd 0x12345678	;NEW 20091224
vidmem:	 dd 0x12345678
	
align 32
	
tss_start:
	times 26 dd 0
tss_end:
	
stack:
	dd 0xabcdef
	    ;times (0x1000+STACK_SIZE) db 0      ; reserve 16k stack on a quadword boundary
