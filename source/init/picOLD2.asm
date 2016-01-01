;=====;
; PIC ;
;=====;
pic_remap: 
	push eax
	push ebx
	push ecx
	push edx

	xor edx,edx
	xor ecx,ecx
	xor ebx,ebx
	
	;; Store masks: 

	inc edx
	
	in al,021h
	mov bl,al

	inc edx
	
	in al,0A1h
	mov cl,al
	
	;; Initialize master
	
	inc edx
	
	mov al,11h	
	out 020h,al		; Give the Master PIC (in port 20h) the "initialize" command

	inc edx
	
	xor ax,ax
	out 080h,al
	
	;; Initialize slave
	
	inc edx
	
	mov al,11h
	out 0A0h,al		; Give the Slave PIC (in port A0h) the "initialize" command
	
	inc edx
	
	xor ax,ax
	out 080h,al

	;; Remap master
	
	mov al,20h
	out 021h,al		; Set the interrupt vector offset of the Master PIC as 20h
	
	inc edx
	
	xor ax,ax
	out 080h,al

	;; Remap slave
	
	mov al,28h
	out 0A1h,al		; Set the interrupt vector offset of the Slave PIC as 28h

	inc edx
	
	xor ax,ax
	out 080h,al
	
	;; More initialization: 
	
	mov al,04h
	out 021h,al

	inc edx
	
	xor ax,ax
	out 080h,al
	
	inc edx
	
	mov al,02h
	out 0A1h,al

	inc edx
	
	xor ax,ax
	out 080h,al
	
	;; 
	
	mov al,01h
	out 021h,al

	xor ax,ax
	out 080h,al
	
	mov al,01h
	out 0A1h,al
	
	xor ax,ax
	out 080h,al

	;; Restore masks

	mov al,bl
	out 021h,al
	
	mov al,cl
	out 0A1h,al

	pop edx
	pop ecx
	pop ebx
	pop eax
ret
