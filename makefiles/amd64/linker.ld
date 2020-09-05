ENTRY(kentry)
SECTIONS
{
    /**************** Per cpu variables ****************/
    . = 0x0;
    .cpu_local_data (NOLOAD) :
    {
        KEEP(*(.cpu_local_data*))
    }
    __cpu_local_size = .;

    /**************** Kernel sections ****************/
    . = 0xffffffff80000000 + 0x01000000;

    .stivalehdr :
    {
        KEEP(*(.stivalehdr))
    }

    __kernel_start_text = .;
    .text ALIGN(4K) :
    {
        *(.text*)
    }
    __kernel_end_text = .;

    .rodata ALIGN(4K) :
    {
        *(.rodata*)
    }
    __kernel_end_rodata = .;

    .data ALIGN(4K) :
    {
        *(.stivale_stack*)
        *(.data*)

        g_bsp_cpu_local_buffer = .;
        FILL(0x00)
        . += ABSOLUTE(__cpu_local_size);
    }

    .drivers ALIGN(4K) :
    {
        g_drivers = .;gg
        *(.drivers.priority*)
        *(.drivers*)
        g_drivers_end = .;
    }

    .bss ALIGN(4K) :
    {
        *(.bss*)
    }
    __kernel_end_data = .;
}