;=====;
; PIC ;
;=====;
pic_remap: 
	push eax

	mov al,11h	
	out 020h,al		; Give the Master PIC (in port 20h) the "initialize" command
	mov al,11h
	out 0A0h,al		; Give the Slave PIC (in port A0h) the "initialize" command
	
	mov al,20h
	out 021h,al		; Set the interrupt vector offset of the Master PIC as 20h
	mov al,28h
	out 0A1h,al		; Set the interrupt vector offset of the Slave PIC as 28h
	
	mov al,04h
	out 021h,al
	mov al,02h
	out 0A1h,al
	
	mov al,01h
	out 021h,al
	mov al,01h
	out 0A1h,al

	pop eax
ret
