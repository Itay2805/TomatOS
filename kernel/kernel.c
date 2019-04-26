#include "kernel.h"

#include <memory/pmm.h>
#include <memory/vmm.h>
#include <common/string.h>
#include <interrupts/idt.h>
#include <interrupts/irq.h>
#include <interrupts/isr.h>
#include <process/scheduler.h>
#include <process/process.h>
#include <common/common.h>
#include <process/spinlock.h>
#include <cpu/cpuid.h>
#include <cpu/msr.h>
#include <process/syscall.h>
#include <memory/gdt.h>


#include "graphics/term.h"

extern void* boot_pdpe;

mm_context_t kernel_memory_manager;

static void thread_a(void* arg) {
    ((void)arg);
    while(true) {
        // syscall(0xBABE);
    }
}
static void thread_a_end() {}

static void thread_b(void* arg) {
    ((void)arg);
    while(true) {
        // syscall(0xCAFE);
    }
}
static void thread_b_end() {}

void kernel_main(multiboot_info_t* info) {
    vmm_early_init();

    term_init(info);
    term_print("[kernel_main] successfully loaded the kernel (bootloader='%s', cmdline='%s')\n", (char *)(uintptr_t)info->boot_loader_name, (char *)(uintptr_t)info->cmdline);

    int vendor;
    char name[13] = {0};
    get_cpu_vendor(&vendor, name);
    term_print("[kernel_main] cpu vendor='%s'\n", name);

    pmm_init(info);
    vmm_init(info);

    isr_init();
    irq_init();
    idt_init();

    // kernel will be mapped to 0xffffffffffffffff - (MB(512) - GB(2) (ASLR of 1.5GB)
    // heap starts at 0xffffffff00000000 - (MB(512) - GB(2)) (ASLR of 1.5GB)
    // user mode heap will start at MB(512) - GB(2) (ASLR of 1.5GB)

    mm_context_init(&kernel_memory_manager, 0xFFFFFFFF00000000);

    syscall_init();

    scheduler_init();

    process_t* pa = process_create(NULL, false);
    pa->threads[0].cpu_state.rbp = GB(4);
    pa->threads[0].cpu_state.rsp = GB(4);
    pa->threads[0].cpu_state.rip = GB(1);
    pa->threads[0].start = (thread_start_f) GB(1);
    for(uint64_t i = 0; i <= ALIGN_UP((uint64_t)thread_a_end - (uint64_t)thread_a, KB(4)); i += KB(4)) {
        vmm_allocate(pa->address_space, (void *) GB(1) + i, PAGE_ATTR_EXECUTE | PAGE_ATTR_USER | PAGE_ATTR_WRITE);
    }
    vmm_allocate(pa->address_space, (void *) GB(4) - KB(4), PAGE_ATTR_USER | PAGE_ATTR_WRITE);
    vmm_set(pa->address_space);
    memcpy((void *) GB(1), thread_a, (uint64_t)thread_a_end - (uint64_t)thread_a);
    vmm_set(kernel_address_space);





    process_t* pb = process_create(NULL, false);
    pb->threads[0].cpu_state.rbp = GB(4);
    pb->threads[0].cpu_state.rsp = GB(4);
    pb->threads[0].cpu_state.rip = GB(1);
    pb->threads[0].start = (thread_start_f) GB(1);
    for(uint64_t i = 0; i <= ALIGN_UP((uint64_t)thread_b_end - (uint64_t)thread_b, KB(4)); i += KB(4)) {
        vmm_allocate(pb->address_space, (void *) GB(1) + i, PAGE_ATTR_EXECUTE | PAGE_ATTR_USER | PAGE_ATTR_WRITE);
    }
    vmm_allocate(pb->address_space, (void *) GB(4) - KB(4), PAGE_ATTR_USER | PAGE_ATTR_WRITE);
    vmm_set(pb->address_space);
    memcpy((void *) GB(1), thread_b, (uint64_t)thread_b_end - (uint64_t)thread_b);
    vmm_set(kernel_address_space);

    sti();

    while(true) {
        hlt();
    }
}
