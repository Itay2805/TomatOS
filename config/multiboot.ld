ENTRY(multiboot_main)
OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

SECTIONS
{
	. = 0x0100000;
	kernel_physical_start = . - 0x0100000;

    /* ========== Multiboot ========== */
    .multiboot.header ALIGN(8) :
    {
        *(.multiboot.header)
    }

    .multiboot.text :
    {
	    *(.multiboott.text)
    }

    .multiboot.data :
    {
        *(.multiboot.data)
    }

    .multiboot.bss :
    {
        *(.multiboot.bss)
    }

    /* ========== Kernel code ========== */
    . += 0xFFFFFFFFC0000000;
	.text ALIGN(4K) : AT (ADDR (.text) - 0xFFFFFFFFC0000000)
	{
	    *(.text*)
	}

	/* usermode code (like trampolines) */
	kernel_user_text_start = .;
	.utext ALIGN(4K) : AT (ADDR (.utext) - 0xFFFFFFFFC0000000)
	{
	    *(.utext*)
	}
    kernel_user_text_end = .;

    .rodata ALIGN(4K) : AT (ADDR (.rodata) - 0xFFFFFFFFC0000000)
    {
        *(.rodata)
    }

	.data ALIGN(4K) : AT (ADDR (.data) - 0xFFFFFFFFC0000000)
	{
	    *(.data)
	}

	.bss ALIGN(4K) : AT (ADDR (.bss) - 0xFFFFFFFFC0000000)
	{
	    *(COMMON)
	    *(.bss)
	}

	kernel_physical_end = . - 0xFFFFFFFFC0000000;
	kernel_virtual_end = .;
}