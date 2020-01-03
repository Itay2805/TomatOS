ENTRY(tboot_main)
OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

SECTIONS
{
    /* ========== Multiboot ========== */
	. = 0x0200000;
	kernel_physical_start = .;

    .tboot.text ALIGN(4K) :
    {
	    *(.tboot.text)
    }

    /* ========= Kernel code ========= */
    . += 0xFFFFFFFFC0000000;
	.text ALIGN(4K) : AT (ADDR (.text) - 0xFFFFFFFFC0000000)
	{
	    *(.text*)
	}

    .rodata ALIGN(4K) : AT (ADDR (.rodata) - 0xFFFFFFFFC0000000)
    {
        *(.rodata*)

        /* Constructors */
        __init_array_start = .;
        KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP (*(.init_array .ctors))
        __init_array_end = .;
    }

	.data ALIGN(4K) : AT (ADDR (.data) - 0xFFFFFFFFC0000000)
	{
	    *(.data*)
	}

	.bss ALIGN(4K) : AT (ADDR (.bss) - 0xFFFFFFFFC0000000)
	{
	    *(COMMON)
	    *(.bss*)
	}

	kernel_physical_end = . - 0xFFFFFFFFC0000000;
	kernel_virtual_end = .;
}