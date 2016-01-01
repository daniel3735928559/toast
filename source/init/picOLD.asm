%define ICW_1 0x11				; 00010001 binary. Enables initialization mode and we are sending ICW 4

	%define PIC_1_CTRL 0x20				; Primary PIC control register
	%define PIC_2_CTRL 0xA0				; Secondary PIC control register
	
	%define PIC_1_DATA 0x21				; Primary PIC data register
	%define PIC_2_DATA 0xA1				; Secondary PIC data register
	
	%define IRQ_0	0x20				; IRQs 0-7 mapped to use interrupts 0x20-0x27
	%define IRQ_8	0x28				; IRQs 8-15 mapped to use interrupts 0x28-0x36
	
;=====;		
; PIC ;	
;=====;	
pic_remap: 
	push eax
				; Send ICW 1 - Begin initialization -------------------------
	
				; Setup to initialize the primary PIC. Send ICW 1
	
	mov	al, ICW_1
	out	PIC_1_CTRL, al
	
				; Send ICW 2 - Map IRQ base interrupt numbers ---------------
	
				; Remember that we have 2 PICs. Because we are cascading with this second PIC, send ICW 1 to second PIC command register
	
	out	PIC_2_CTRL, al
	
				; send ICW 2 to primary PIC
	
	mov	al, IRQ_0
	out	PIC_1_DATA, al
	
				; send ICW 2 to secondary controller
	
	mov	al, IRQ_8
	out	PIC_2_DATA, al
	
				; Send ICW 3 - Set the IR line to connect both PICs ---------
	
				; Send ICW 3 to primary PIC
	
	mov	al, 0x4			; 0x04 => 0100, second bit (IR line 2)
	out	PIC_1_DATA, al		; write to data register of primary PIC
	
				; Send ICW 3 to secondary PIC
	
	mov	al, 0x2			; 010=> IR line 2
	out	PIC_2_DATA, al		; write to data register of secondary PIC
	
				; Send ICW 4 - Set x86 mode --------------------------------
	
	mov	al, 1			; bit 0 enables 80x86 mode
	
				; send ICW 4 to both primary and secondary PICs
	
	out	PIC_1_DATA, al
	out	PIC_2_DATA, al
	
				; All done. Null out the data registers
	
	mov	al, 0
	out	PIC_1_DATA, al
	out	PIC_2_DATA, al
	
	pop eax
ret
