%define stage2Base 0x0800
%define stage2Size 9
%define kernelBase 0x1000
%define kernelSize 0x55

read_loop:
    ;pusha
    ;mov si,b_reading
    ;call bios_print
    ;popa
    pusha
    call read_sector
    popa
    add bx, 512
    inc ax
    dec cx
    jnz read_loop
    
ret

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
