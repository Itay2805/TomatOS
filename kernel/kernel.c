#include "kernel.h"

#include <providers/zero/zero_provider.h>
#include <providers/stdio/stdio_provider.h>
#include <providers/echfs/echfs_provider.h>
#include <providers/ata/ata_provider.h>
#include <providers/ps2/ps2_provider.h>
#include <providers/elf/elf_provider.h>
#include <providers/echfs/echfs.h>

#include <common/string.h>
#include <common/klib.h>
#include <common/logging.h>
#include <common/common.h>

#include <interrupts/idt.h>
#include <interrupts/irq.h>
#include <interrupts/isr.h>

#include <process/scheduler.h>
#include <process/syscall.h>
#include <process/process.h>

#include <memory/gdt.h>
#include <memory/pmm.h>
#include <memory/vmm.h>

#include <drivers/pci.h>

#include <cpu/cpuid.h>
#include <cpu/msr.h>
#include <shell/shell.h>
#include <drivers/rtc.h>
#include <providers/virtio/virtio_provider.h>

#include "graphics/term.h"

extern void* boot_pdpe;
mm_context_t kernel_memory_manager;

spinlock_t freq_lock = 0;

static void provider_init(error_t(*init_func)()) {
    error_t err = NO_ERROR;

    CHECK_AND_RETHROW(init_func());

cleanup:
    if(IS_ERROR(err)) {
        KERNEL_STACK_TRACE(err);
        ERROR_FREE(err);
    }
}

void kernel_main(multiboot_info_t* info) {
    error_t err = NO_ERROR;

    // early vmm initialization (maps the first 4gb)
    vmm_early_init();

    // initialize the terminal so we can log stuff
    term_init(info);
    LOG_NOTICE("successfully loaded the kernel");
    LOG_INFO("bootloader='%s'", (char *)(uintptr_t)info->boot_loader_name);
    LOG_INFO("cmdline='%s'", (char *)(uintptr_t)info->cmdline);

    // test cpuid
    int vendor;
    char name[13] = {0};
    get_cpu_vendor(&vendor, name);
    LOG_INFO("cpu vendor='%s'", name);

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

    LOG_INFO("Calculating CPU freq...");
    uint64_t start = rtc_unixtime();
    int64_t start_tick = rdtsc();
    while(rtc_unixtime() - start < 1);
    int64_t end_tick = rdtsc();
    LOG_INFO("Running at `%d` steps per second", (int)(end_tick - start_tick));

    /// ONLY USE ERRORS FROM HERE
    CHECK_AND_RETHROW(pci_init());

    // initialize processes
    CHECK_AND_RETHROW(process_init());

    // initialize resource related stuff
    provider_init(resource_manager_init);
    provider_init(zero_provider_init);
    provider_init(virtio_provider_init);
    provider_init(ps2_provider_init);
    provider_init(stdio_provider_init);
    provider_init(ata_provider_init);
    provider_init(echfs_provider_init);
    provider_init(elf_provider_init);

    // initlize the scheduler
    CHECK_AND_RETHROW(scheduler_init());

    // start the embedded shell
    // TODO: Mount from the commandline
    start_shell("echfs://[ata://primary:0/]/");

    // kick start the system!
    LOG_NOTICE("Enabling interrupts");
    sti();

    while(true) {
        hlt();
    }

cleanup:
    KERNEL_PANIC(err);
}
