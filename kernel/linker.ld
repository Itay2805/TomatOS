ENTRY(kentry)
SECTIONS
{
    /****************   CPU Locals    ****************/
    . = 0x0;
    .cpu_local_data (NOLOAD) :
    {
        KEEP(*(.cpu_local_data*))
    }
    __cpu_local_size = .;

    /**************** Kernel sections ****************/
    . = 0xffffffff80000000 + 0x01000000;

    .stivale2hdr :
    {
        KEEP(*(.stivale2hdr))
    }

    . = ALIGN(4K);
    __kernel_start_text = .;
    .text ALIGN(4K) :
    {
        *(.text*)
    }
    . = ALIGN(4K);
    __kernel_end_text = .;

    .rodata ALIGN(4K) :
    {
        *(.rodata*)
    }
    . = ALIGN(4K);
    __kernel_end_rodata = .;

    .data ALIGN(4K) :
    {
        *(.stivale_stack*)
        *(.data*)
    }

    .bss ALIGN(4K) :
    {
        *(.bss*)
    }
    . = ALIGN(4K);
    __kernel_end_data = .;
}