#include "kernel.h"

#include <memory/pmm.h>
#include <memory/vmm.h>
#include <common/string.h>
#include <interrupts/idt.h>
#include <interrupts/irq.h>
#include <interrupts/isr.h>
#include <process/scheduler.h>
#include <process/process.h>


#include "graphics/term.h"

extern void* boot_pdpe;

mm_context_t kernel_memory_manager;

static void thread_a(void* arg) {
    ((void)arg);
}

static void thread_b(void* arg) {
    ((void)arg);
}

void kernel_main(multiboot_info_t* info) {
    vmm_early_init();

    term_init(info);
    term_print("[kernel_main] successfully loaded the kernel (bootloader='%s', cmdline='%s')\n", (char *)(uintptr_t)info->boot_loader_name, (char *)(uintptr_t)info->cmdline);

    pmm_init(info);
    vmm_init(info);

    isr_init();
    irq_init();
    idt_init();

    // kernel will be mapped to 0xffffffffffffffff - (MB(512) - GB(2) (ASLR of 1.5GB)
    // heap starts at 0xffffffff00000000 - (MB(512) - GB(2)) (ASLR of 1.5GB)
    // user mode heap will start at MB(512) - GB(2) (ASLR of 1.5GB)

    mm_context_init(&kernel_memory_manager, 0xFFFFFFFF00000000);

    scheduler_init();

    process_create(thread_a, true);
    process_create(thread_b, true);

    sti();

    while(true) {
        hlt();
    }
}
