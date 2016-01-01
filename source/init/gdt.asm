;=====;
; GDT ;
;=====;
gdt:            ; Address for the GDT
gdt_null:	db 00000000b,00000000b,00000000b,00000000b,00000000b,00000000b,00000000b,00000000b
gdt_code:	db 11111111b,11111111b,00000000b,00000000b,00000000b,10011010b,11001111b,00000000b
gdt_data:	db 11111111b,11111111b,00000000b,00000000b,00000000b,10010010b,11001111b,00000000b
gdt_user_code:	db 11111111b,11111111b,00000000b,00000000b,00000000b,11111010b,11001111b,00000000b
gdt_user_data:	db 11111111b,11111111b,00000000b,00000000b,00000000b,11110010b,11001111b,00000000b
gdt_tss:	db 00000000b,00000000b,00000000b,00000000b,00000000b,10001001b,01000000b,00000000b
gdt_end:        ; Used to calculate the size of the GDT

data_selector equ gdt_data - gdt
code_selector equ gdt_code - gdt

gdtr:        	                ; The GDT descriptor
        dw gdt_end - gdt - 1    ; Limit (size)
        dd gdt                  ; Address of the GDT
