#include "kernel.h"

#include <memory/pmm.h>
#include <memory/vmm.h>
#include <common/string.h>
#include <interrupts/idt.h>
#include <interrupts/irq.h>
#include <interrupts/isr.h>


#include "graphics/term.h"

extern void* boot_pdpe;

mm_context_t kernel_memory_manager;

void kernel_main(multiboot_info_t* info) {
    vmm_early_init();

    term_init(info);
    term_print("[kernel_main] successfully loaded the kernel (bootloader='%s', cmdline='%s')\n", (char *)(uintptr_t)info->boot_loader_name, (char *)(uintptr_t)info->cmdline);

    pmm_init(info);
    vmm_init(info);

    isr_init();
    irq_init();
    idt_init();

    mm_context_init(&kernel_memory_manager, 0xFFFFFFFF00000000);

    sti();

    while(true) {
        hlt();
    }
}
