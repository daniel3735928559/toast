ENTRY (begin)
OUTPUT_FORMAT("binary")

SECTIONS {
   . = 0xC0000000;

   .text : AT(ADDR(.text) - 0xC0000000) {
       *(.text)
       *(.rodata*)
   }

   .data ALIGN (0x1000) : AT(ADDR(.data) - 0xC0000000) {
       *(.data)
   }

   .bss : AT(ADDR(.bss) - 0xC0000000) {
       _sbss = .;
       *(COMMON)
       *(.bss)
       _ebss = .;
   }
}


/*SECTIONS{
    . = 0xC0100000;

    .text :{
        *(.text)
    }

    .data ALIGN (0x1000) : {
        *(.data)
        *(.rodata)
    }

    .bss : {
        _sbss = .;
        *(COMMON)
        *(.bss)
        _ebss = .;
    }
}

*/
