ENTRY(kentry)
SECTIONS
{
    /**************** Per cpu variables ****************/
    . = 0x0;
    .cpu_local_data (NOLOAD) :
    {
        *(.cpu_local_data*)
    }
    __cpu_local_size = .;

    /**************** Kernel sections ****************/
    . = 0xffffffff80000000 + 0x01000000;

    .stivalehdr :
    {
        KEEP(*(.stivalehdr))
    }

    .text ALIGN(4K) :
    {
        *(.text*)
    }

    .rodata ALIGN(4K) :
    {
        *(.rodata*)
    }

    .data ALIGN(4K) :
    {
        *(.data*)

        g_bsp_cpu_local_buffer = .;
        FILL(0x00)
        . += ABSOLUTE(__cpu_local_size);
    }

    .bss ALIGN(4K) :
    {
        *(.bss*)
    }
}