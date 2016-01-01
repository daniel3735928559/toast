%macro USER_IDT_ENTRY 1
dw ((BASE + isr%1 - $$) & 0xFFFF);	dw ((isr%1 - $$) & 0xFFFF)
	dw code_selector
	db 0
	db 0xEE
dw ((BASE + isr%1 - $$) >> 16);	dw ((isr%1 - $$) >> 16)
%endmacro

%macro IDT_ENTRY 1
dw ((BASE + isr%1 - $$) & 0xFFFF);	dw ((isr%1 - $$) & 0xFFFF)
	dw code_selector
	db 0
	db 0x8E
dw ((BASE + isr%1 - $$) >> 16);	dw ((isr%1 - $$) >> 16)
%endmacro

;=====;
; IDT ;
;=====;
	
idtr: 
	dw idt_end - idt_start - 1
	dd idt_start

idt_start: 
	IDT_ENTRY 0 
	IDT_ENTRY 1
	IDT_ENTRY 2
	IDT_ENTRY 3
	IDT_ENTRY 4
	IDT_ENTRY 5
	IDT_ENTRY 6
	IDT_ENTRY 7
	IDT_ENTRY 8
	IDT_ENTRY 9
	IDT_ENTRY 10 
	IDT_ENTRY 11
	IDT_ENTRY 12
	IDT_ENTRY 13
	IDT_ENTRY 14
	IDT_ENTRY 15
	IDT_ENTRY 16
	IDT_ENTRY 17
	IDT_ENTRY 18
	IDT_ENTRY 19
	IDT_ENTRY 20
	IDT_ENTRY 21
	IDT_ENTRY 22
	IDT_ENTRY 23
	IDT_ENTRY 24
	IDT_ENTRY 25
	IDT_ENTRY 26
	IDT_ENTRY 27
	IDT_ENTRY 28
	IDT_ENTRY 29
	IDT_ENTRY 30
	IDT_ENTRY 31
	IDT_ENTRY 32
	IDT_ENTRY 33
	USER_IDT_ENTRY 34
	USER_IDT_ENTRY 35
idt_end: 
