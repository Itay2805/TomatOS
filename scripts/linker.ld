ENTRY(kernel_main)
OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

SECTIONS
{
    /* ========== Per Cpu variables ========== */
    . = 0x0;
    .pcpu_data (NOLOAD) : {
        *(.pcpu_data*)
    }
    kernel_pcpu_data_size = .;

    .stivalehdr (NOLOAD) : {
        .stivalehdr
    }

    /* ========= Kernel code ========= */
    . = 0xffffffff80100000;
    kernel_start = .;

	.text ALIGN(4K) :
	{
	    *(.text*)
	}

	kernel_text_end = .;

    .rodata ALIGN(4K) :
    {
        *(.rodata*)
    }

    kernel_rodata_end = .;

	.data ALIGN(4K) :
	{
	    *(.data*)

	    bsp_pcpu_data = .;
	    FILL(0x00)
	    . += ABSOLUTE(kernel_pcpu_data_size);
	}

	.bss ALIGN(4K) :
	{
	    *(COMMON)
	    *(.bss*)
	}

	kernel_end = .;
}