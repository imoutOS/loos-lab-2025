/***********************************************************/
/* Configure memory regions */
MEMORY
{
    /* 2GB for Kernel ELF kernel code,data,bss */
    ELF  (rwx)         : ORIGIN = 0xffff800000000000, LENGTH = 0x200000
    /* 1TB for Kernel Direct Map Area */
    DMAP (rw)          : ORIGIN = 0xffff800000200000, LENGTH = 0x40000000
    /* others for None Direct Map Area */
}
ENTRY(_start)
OUTPUT_ARCH(riscv)

SECTIONS
{
    .start :
    {
        _start_system = .;
        *(.text._start)
    } > ELF

    .text :
    {
        __text_start__ = .;
        *(.text .text.*)
        . = ALIGN(16);
        *(.rodata .rodata.*)
        . = ALIGN(16);
        *(.srodata .srodata.*)
        __text_end__ = .;
    } > ELF

    .data :
    {
        . = ALIGN(16);
        __data_start__ = .;
        *(.sdata .sdata.*)
        . = ALIGN(16);
        *(.data .data.*)
        __data_end__ = .;
    } > ELF

    .bss (NOLOAD) :
    {
        . = ALIGN(16);
        __bss_start__ = .;
        *(.sbss .sbss.*)
        . = ALIGN(16);
        *(.bss .bss.*)
        __bss_end__ = .;
    } > ELF

    .dmap (NOLOAD) :
    {
       __dmap_start__ = .;
       . += 0x40000000;
       __dmap_end__ = .;
    } > DMAP

    PROVIDE(rmem_start = .);
}

/***********************************************************/
