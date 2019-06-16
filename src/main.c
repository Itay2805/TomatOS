#include <drivers/vmdev/vmdev.h>
#include <drivers/apic/ioapic.h>
#include <drivers/pic8259/pic.h>
#include <drivers/term/term.h>
#include <drivers/acpi/acpi.h>
#include <drivers/apic/apic.h>
#include <drivers/pci/pci.h>

#include <process/scheduler.h>
#include <process/process.h>

#include <interrupts/interrupts.h>
#include <interrupts/timer.h>
#include <interrupts/idt.h>
#include <interrupts/irq.h>

#include <logger/logger.h>

#include <memory/gdt.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <memory/mm.h>

#include <boot/boot.h>
#include <cpu/fpu.h>
#include <common.h>
#include <error.h>


static void* test_1(void* arg) {
    (void)arg;
    while(true) {
    }
    return NULL;
}

static void* test_2(void* arg) {
    (void)arg;
    while(true)  {

    }
    return NULL;
}

error_t keyboard_handler(registers_t* regs) {
    log_info("KEYBOARD!");
    return NO_ERROR;
}

void kernel_main(boot_info_t* info) {
    error_t err = NO_ERROR;

    /*********************************************************
     * Early initialization
     *
     * basically setting a nice enviroment to work in
     *********************************************************/

    // vm logger can be initialized very early on
    vmdev_register_logger();

    // initialize the idt and gdt
    idt_init();
    gdt_init();

    /*********************************************************
     * Early memory initialization
     *
     * can start using errors
     *********************************************************/
    CHECK_AND_RETHROW(pmm_early_init(info));
    CHECK_AND_RETHROW(vmm_init(info));

    // convert to direct mapping
    info = PHYSICAL_ADDRESS(info);
    info->rsdp_ptr = (uint64_t) PHYSICAL_ADDRESS(info->rsdp_ptr);
    info->framebuffer.addr = (uint64_t) PHYSICAL_ADDRESS(info->framebuffer.addr);
    info->mmap.entries = PHYSICAL_ADDRESS(info->mmap.entries);

    /*********************************************************
     * Initialization of essentials
     *
     * we require everything in here to work
     *********************************************************/
    CHECK_AND_RETHROW(pmm_init());
    CHECK_AND_RETHROW(mm_init());
    term_init(info);
    CHECK_AND_RETHROW(acpi_tables_init(info));
    CATCH(pci_init());
    CHECK_AND_RETHROW(pic8259_disable());
    CHECK_AND_RETHROW(apic_init());
    CHECK_AND_RETHROW(timer_init());
    CHECK_AND_RETHROW(acpi_init());
    //CHECK_AND_RETHROW(fpu_init());

    // finally initialize the threading and scheduler
    CHECK_AND_RETHROW(thread_init());
    CHECK_AND_RETHROW(scheduler_init());

    /*********************************************************
     * Initialization completed
     *********************************************************/
    log_notice("initialization finished");

    process_t* process = NULL;
    thread_t* thread_1 = NULL;
    thread_t* thread_2 = NULL;
    CHECK_AND_RETHROW(process_create(&process, NULL, 0, NULL));
    CHECK_AND_RETHROW(thread_create(process, test_1, NULL, &thread_1));
    CHECK_AND_RETHROW(thread_create(process, test_2, NULL, &thread_2));

    // TODO: start the scheduler
    _sti();

    while(true) {
        _hlt();
    }

cleanup:
    log_critical("Error during kernel initialization :(");
    while(true);
}