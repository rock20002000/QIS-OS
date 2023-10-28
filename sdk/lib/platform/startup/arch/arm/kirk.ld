OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm",
	      "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

/* refer to https://github.com/iDalink/ld-linker-script/tree/master/08%20PHDRS%E6%8C%87%E4%BB%A4 */

PHDRS {
    text PT_LOAD;
    data PT_LOAD;
    bss  PT_LOAD;
}

SECTIONS
{
    . = SEGMENT_START("text-segment", 0x08048000);

    . = ALIGN(0x1000);

    .init :
    {
        KEEP (*(SORT_NONE(.init)))
    } :text

    .text :
    {
        KEEP (*(SORT_NONE(.init)))
        *(.text);
        *(.text.*);
        *(.gnu.warning)
        *(.glue_7t) *(.glue_7) *(.vfp11_veneer) *(.v4_bx)
        __text_end = .;
    } :text

    .rodata : {
        *(.rodata);
        *(.rodata.*);
    } :text

    . = ALIGN(0x1000);
    .procenv : {
        *(.procenv);
        KEEP(*(.procenv));
        __rodata_end = .;
    } :text

    . = ALIGN(0x1000);
    .got : {
        __got = .;
        *(.got.plt) *(.igot.plt) *(.got) *(.igot)
    } :data

    .data : {
        __data = .;
        *(.data);
        *(.data.*);
        *(.data1) ;
        __data_end = .;
    } :data

    . = ALIGN(0x1000);
    __zeroed_pages = .;

    .bss : {
        __cmdline = .;
        __stack = .;
        . += 0x0F000;
        __stack_end = .;

        __bss = .;
        *(.bss);
        *(.bss.*);
        __bss_end = .;
    } :bss

    . = ALIGN(0x1000);
    __heap = .;

    __zeroed_pages_end = .;
    __straight_mapping = .;
    __free_vaddr = .;

    . = 0xC0000000;
    __free_vaddr_end = .;
}
