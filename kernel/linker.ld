ENTRY(multiboot_main)
OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
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
	    *(.rodata)
	}

	.data :
	{
	    *(.data)
	}

	.bss :
	{
	    *(.bss)
	}

    kernel_end = .;

    kernel_size = kernel_end - kernel_start;

}