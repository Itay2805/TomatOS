ENTRY(multiboot_main)
OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
SECTIONS
{
	. = 0x0100000;
    kernel_start = .;
	
    .multiboot_header :
    {
	    *(.multiboot_header)
    }

	.text :
	{
	    *(.text*)
	    *(.rodata*)
	    . = ALIGN(4096);
	}

	.data :
	{
	    *(.data)
	    . = ALIGN(4096);
	}

	.bss :
	{
	    *(.bss)
	    *(.COMMON)
	    . = ALIGN(4096);
	}

    kernel_end = .;
}