ENTRY(_entry);


MEMORY {
    ram (rwx):      ORIGIN = 0xC1000000, LENGTH = 0x2000000
}

SECTIONS {

    .text : ALIGN(32) {
        *(.text);
        *(.text.*);
    } > ram

    .rodata : ALIGN(32) {
        *(.rodata);
        *(.rodata.*);
    } > ram

    .data : ALIGN(32) {
        
       . = ALIGN(0x1000); 
       __vdso_data_start = .;
       KEEP(*(.data.vdso.datapage))
       __vdso_data_end = .;
       . = ALIGN(0x1000);
       __vdso_text_start = .;
       KEEP(*(.data.vdso.text))
       __vdso_text_end = .;

       . = ALIGN(0x1000);
        *(.data);
        *(.data.*);
    } > ram

    .bss : ALIGN(32) {
        __bss = .;
        *(.bss);
        *(.bss.*);
        __bss_end = .;
    } > ram

      .vectors  :  {
             
        . = ALIGN(4);
        PROVIDE( _vectors_start = . );
        *(.vectors)

        . = _vectors_start + SIZEOF(.vectors);

       /*. = ALIGN(0x400);*/
        _vectors_end = .;
    } > ram

    .stack : {
        . = ALIGN(4096);
        KEEP(*(.stack.start))      
        PROVIDE(statckSTART = .);

        . = . + 0x8000;
        . = ALIGN(4096);
        KEEP(*(.stack.und))
        PROVIDE(stackUND = .);

        . = . + 0x8000;
        . = ALIGN(4096);
        KEEP(*(.stack.abt))
        PROVIDE(stackABT = .);

        . = . + 0x8000;
        . = ALIGN(4096);
        KEEP(*(.stack.irq))
        PROVIDE(stackIRQ = .);

        . = . + 0x8000;
        . = ALIGN(4096);
        KEEP(*(.stack.fiq))
        PROVIDE(stackFIQ = .);   

        PROVIDE(stack_end = .);
    } >ram

    /DISCARD/ :{
        *(*.eh_frame);
        *(.note.gnu*);
    }

}
