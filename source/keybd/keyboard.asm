enter_key   equ 0
bspace	    equ 0
tab	    equ 0
ctrl_key    equ 0
lshift	    equ 0
rshift	    equ 0
prnscr	    equ 0
alt_key     equ 0
caps	    equ 0
f1	    equ 0
f2	    equ 0
f3	    equ 0
f4	    equ 0
f5	    equ 0
f6	    equ 0
f7	    equ 0
f8	    equ 0
f9	    equ 0
f10	    equ 0
f11	    equ 0
f12	    equ 0
numlock     equ 0
scroll	    equ 0
home	    equ 0
arrowup     equ 0
pgup	    equ 0
num_sub     equ 0
arrowleft   equ 0
center5     equ 0
arrowright  equ 0
num_plus    equ 0
_end	    equ 0
arrowdown   equ 0
pgdn	    equ 0
_ins	    equ 0
del	    equ 0

keymap:
	db 0
	db '1234567890-=', bspace
	db tab,'qwertyuiop[]',enter_key
	db ctrl_key,'asdfghjkl;',39,'`',lshift
	db '\','zxcvbnm,./',rshift,prnscr,alt_key,' '
	db caps,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,numlock
	db scroll,home,arrowup,pgup,num_sub,arrowleft,center5,arrowright
	db num_plus,_end,arrowdown,pgdn,_ins,del
input: 
	db 0

keyboard_isr: 
	
	push eax
	push ebx
	
	in al,60h
	mov [input],al

	mov al,20h
	out 20h,al

	pop ebx
	pop eax
ret

;==========================;
; al = char, ah = scancode ;
;==========================;

get_char: 
	mov ax,0xFF
	mov [input],al
	
	.waiting: 
		mov al,[input]
		cmp al,0xFF
	je .waiting
	
	mov ah,0
	movzx esi,ax
	mov al,[esi + keymap]
	mov ah,[input]
ret