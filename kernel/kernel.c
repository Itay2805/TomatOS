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
#include <providers/term/term_provider.h>
#include <providers/ata/ata_provider.h>
#include <providers/echfs/echfs_provider.h>
#include <common/klib.h>
#include <providers/echfs/echfs.h>

#include "graphics/term.h"

extern void* boot_pdpe;
mm_context_t kernel_memory_manager;

static void thread_kernel(void* arg) {
    resource_t stdout;
    resource_descriptor_t stdout_desc = {
            .scheme = "term"
    };

    open(&stdout_desc, &stdout);
    write(stdout, "In kernel thread\n", sizeof("In kernel thread\n"), NULL);

    resource_t echfs_root;
    resource_descriptor_t ata_desc = {
            .scheme = "ata",
            .domain = "primary",
            .port = 0
    };
    resource_descriptor_t echfs_desc = {
            .scheme = "echfs",
            .path = "",
            .sub = &ata_desc
    };
    if(open(&echfs_desc, &echfs_root)) {
        echfs_directory_entry_t entry;
        write(stdout, "===========\n", sizeof("===========\n"), NULL);
        while(invoke(echfs_root, 1, &entry)) {
            if(entry.type == ECHFS_OBJECT_TYPE_FILE) {
                write(stdout, "F> ", 3, NULL);
            }else {
                write(stdout, "D> ", 3, NULL);
            }
            write(stdout, entry.name, 218, NULL);
            write(stdout, "\n", 1, NULL);
        }
        write(stdout, "===========\n", sizeof("===========\n"), NULL);
    }else {
        write(stdout, "Failed to open echfs://[ata://primary:0/]/\n", sizeof("Failed to open echfs://[ata://primary:0/]/\n"), NULL);
    }

    while(true);
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

    /// ONLY USE ERRORS FROM HERE

    // initialize processes
    CHECK_AND_RETHROW(process_init());

    // initialize resource related stuff
    CHECK_AND_RETHROW(resource_manager_init());
    CHECK_AND_RETHROW(zero_provider_init());
    CHECK_AND_RETHROW(term_provider_init());
    CHECK_AND_RETHROW(ata_provider_init());
    CHECK_AND_RETHROW(echfs_provider_init());

    // initlize the scheduler
    CHECK_AND_RETHROW(scheduler_init());

    // create some test processes
    process_t* pk = process_create(thread_kernel, true);
    char* kstack = mm_allocate(&kernel_memory_manager, KB(4));
    pk->threads[0].cpu_state.rbp = (uint64_t)kstack + KB(4);
    pk->threads[0].cpu_state.rsp = (uint64_t)kstack + KB(4);

    // kick start the system!
    term_write("[kernel_main] Enabling interrupts\n");
    sti();

    while(true) {
        hlt();
    }

cleanup:
    KERNEL_PANIC(err);
}
