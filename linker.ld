SECTIONS
{
    . = 0x80000;
    /*We need to explicitly put boot section before all other sections*/
    .text : { 
        KEEP(*(.text.boot))
        *(.text*) 
    }
    .rodata : { *(.rodata*) }
    .data : { *(.data*) }
    .bss : { *(.bss*) }
}
