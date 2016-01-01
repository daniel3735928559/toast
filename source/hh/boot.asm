%define BASE 0x0900		;%define BASE 0x800
[BITS 16]
[ORG 7C00h]

jmp start

; Messages: 

b_welcome: db 'Begin booting!',13,10,0
b_loading: db 'Loading OS',0
b_reading: db '.',0
b_done: db 'Loaded!',13,10,0
b_read_error: db 'Error. Retrying...',13,10,0
b_message: db 'We are here!',13,10,0

;============================;
; Bios print -- si = message ;
;============================;

bios_print: 
     lodsb
     or al,al 
     jz .done
     mov ah,0Eh
     int 10h
     jmp bios_print
     .done: 
ret

start:
     xor ax, ax
     mov ds, ax
     mov si,b_welcome
     call bios_print


reset:             ; Reset the floppy drive
     mov ax, 0     ; int 13h with ax = 0 is the desired reset command
     mov dl, 0     ; dl = 0 is an input of int 13h, telling it that the drive we're resetting is drive 0--the floppy drive
     int 13h       ;Call the interrupt
     jc reset      ;If we get a problem, try again
     ;call a20
     jmp a20
     ;jmp read

a20_on: db 1
a20:
          in al,0x64
          test al,2
          jnz a20
          mov al,0xD1
          out 0x64,al
     .d6:
          in al,0x64
          and ax,2
          jnz .d6
          mov al, 0xDF
          out 0x60,al

     .a20_check:
          mov al,byte [fs:0]
          mov ah,al
          not al
          xchg al,byte [gs:0x10]
          cmp ah,byte [fs:0]
          mov [gs:0x10], al
          jz floppy_off
          mov byte [a20_on],0 

floppy_off:
          mov dx, 0x3F2
          mov al, 0
          out dx, al

;FLOPPY READING: 

    mov si,b_loading
    call bios_print

    mov ax, BASE
    mov es, ax
    mov bx, 0000h
    
    mov cx, 40                      ; Sectors to read
    mov ax, 1                       ; Current sector

read_loop:
    pusha
    mov si,b_reading
    call bios_print
    popa
    pusha
    call read_sector
    popa
    add bx, 512
    inc ax
    dec cx
    jnz read_loop
    
    jmp hello

;
; Read Sector
; ax: logical sector to read
; es:bx: address to load to
;
read_sector:
    call lba_to_chs
   
    ;recalibrate
    xor ah, ah
    int 13h
   
    mov ah, 2
    mov al, 1
    mov dl, 0
    int 13h
    
    ret 
      
      
lba_to_chs:
    push bx

    xor dx, dx
    mov bx, 18
    div bx

    mov cx, dx
    inc cx

    xor dx, dx
    mov bx, 2
    div bx

    mov ch, al
    mov dh, dl

    pop bx
    ret


hello: 


     mov si,b_done
     call bios_print

     mov dx, 0x3F2
     mov al, 0
     out dx, al

     mov ah,06h
     mov al,00h
     mov cx,0000h
     mov dx,184Fh
     int 10h

     jmp BASE:0000h

times 510-($-$$) db 0
dw 0xAA55
