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
#include <cpu/cpuid.h>
#include <cpu/msr.h>
#include <process/syscall.h>
#include <memory/gdt.h>
#include <providers/zero/zero_provider.h>


#include "graphics/term.h"

extern void* boot_pdpe;

mm_context_t kernel_memory_manager;

static void thread_a(void* arg) {
    ((void)arg);
    resource_t volatile resource = 0;
    char buffer[10];
    char name_buffer[5];
    name_buffer[0] = 'z';
    name_buffer[1] = 'e';
    name_buffer[2] = 'r';
    name_buffer[3] = 'o';
    name_buffer[4] = 0;
    resource_descriptor_t descriptor = {
        .scheme = name_buffer
    };
    asm ("int $0x80" : : "a"(SYSCALL_OPEN), "D"(&descriptor), "S"(&resource));
    while(true) {
        asm ("int $0x80" : : "a"(0xBABE));
        asm ("int $0x80" : : "a"(SYSCALL_READ), "D"(resource), "S"(buffer), "d"(sizeof(buffer)));
    }
}
static void thread_a_end() {}

static void thread_b(void* arg) {
    ((void)arg);
    resource_t resource = 0;
    char buffer[10];
    char name_buffer[5];
    name_buffer[0] = 'z';
    name_buffer[1] = 'e';
    name_buffer[2] = 'r';
    name_buffer[3] = 'o';
    name_buffer[4] = 0;
    resource_descriptor_t descriptor = {
            .scheme = name_buffer
    };
    asm volatile ("int $0x80" : : "a"(SYSCALL_OPEN), "D"(&descriptor), "S"(&resource));
    while(true) {
        asm ("int $0x80" : : "a"(0xCAFE));
        asm volatile ("int $0x80" : : "a"(SYSCALL_WRITE), "D"(resource), "S"(buffer), "d"(sizeof(buffer)));
    }
}
static void thread_b_end() {}

static void thread_kernel(void* arg) {
    resource_t resource = 0;
    char buffer[12];
    resource_descriptor_t descriptor = {
            .scheme = "zero",
            .port = 0xBABE
    };
    asm volatile ("int $0x80" : : "a"(SYSCALL_OPEN), "D"(&descriptor), "S"(&resource));
    while(true) {
        asm volatile ("int $0x80" : : "a"(SYSCALL_READ), "D"(resource), "S"(buffer), "d"(sizeof(buffer)));
    }
}

void kernel_main(multiboot_info_t* info) {
    error_t err = NO_ERROR;

    // early vmm initialization (maps the first 4gb)
    vmm_early_init();

    // initialize the terminal so we can log stuff
    term_init(info);
    term_print("[kernel_main] successfully loaded the kernel (bootloader='%s', cmdline='%s')\n", (char *)(uintptr_t)info->boot_loader_name, (char *)(uintptr_t)info->cmdline);

    // test cpuid
    int vendor;
    char name[13] = {0};
    get_cpu_vendor(&vendor, name);
    term_print("[kernel_main] cpu vendor='%s'\n", name);

    // initialize isr (allows to catch exceptions)
    // we will finish initializing interrupts later
    isr_init();
    idt_init();

    // initialize memory stuff
    pmm_init(info);
    vmm_init(info);

    // kernel will be mapped to 0xffffffffffffffff - (MB(512) - GB(2) (ASLR of 1.5GB)
    // heap starts at 0xffffffff00000000 - (MB(512) - GB(2)) (ASLR of 1.5GB)
    // user mode heap will start at MB(512) - GB(2) (ASLR of 1.5GB)

    mm_context_init(&kernel_memory_manager, 0xFFFFFFFF00000000);

    // finish initializing the idt (now includes irq and syscall interrupt)
    irq_init();
    syscall_init();
    idt_init();

    CHECK_AND_RETHROW(resource_manager_init());
    CHECK_AND_RETHROW(zero_provider_init());

    // initlize the scheduler
    CHECK_AND_RETHROW(scheduler_init());

    // create some test processes
    /*process_t* pa = process_create(NULL, false);
    pa->threads[0].cpu_state.rbp = GB(4);
    pa->threads[0].cpu_state.rsp = GB(4);
    pa->threads[0].cpu_state.rip = GB(1);
    pa->threads[0].start = (thread_start_f) GB(1);
    for(uint64_t i = 0; i <= ALIGN_UP((uint64_t)thread_a_end - (uint64_t)thread_a, KB(4)); i += KB(4)) {
        vmm_allocate(pa->address_space, (void *) GB(1) + i, PAGE_ATTR_EXECUTE | PAGE_ATTR_USER | PAGE_ATTR_WRITE);
    }
    vmm_allocate(pa->address_space, (void *) GB(4) - KB(4), PAGE_ATTR_USER | PAGE_ATTR_WRITE);
    vmm_set(pa->address_space);
    memset((void *) GB(1), 0xCC, ALIGN_UP((uint64_t)thread_a_end - (uint64_t)thread_a, KB(4)));
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
    memset((void *) GB(1), 0xCC, ALIGN_UP((uint64_t)thread_b_end - (uint64_t)thread_b, KB(4)));
    memcpy((void *) GB(1), thread_b, (uint64_t)thread_b_end - (uint64_t)thread_b);
    vmm_set(kernel_address_space);*/

    process_t* pk = process_create(thread_kernel, true);
    char* kstack = 0;
    kstack = mm_allocate(&kernel_memory_manager, KB(1));
    pk->threads[0].cpu_state.rbp = (uint64_t)kstack + KB(1);
    pk->threads[0].cpu_state.rsp = (uint64_t)kstack + KB(1);

    // kick start the system!
    term_write("[kernel_main] Enabling interrupts\n");
    sti();

    while(true) {
        hlt();
    }

cleanup:
    KERNEL_PANIC();
}
