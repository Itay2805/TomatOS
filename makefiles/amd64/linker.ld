ENTRY(kentry)
SECTIONS
{
    . = 0xffffffff80000000 + 0x01000000;

    .stivalehdr ALIGN(4K) :
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
    }

    .bss ALIGN(4K) :
    {
        *(.bss*)
    }
}