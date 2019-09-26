#include <tboot/tboot.h>
#include <common/error.h>
#include <interrupts/interrupts.h>
#include <drivers/rtl8169/rtl8169.h>
#include <logger/term/term.h>
#include <logger/vmdev/vmdev.h>
#include <interrupts/idt.h>
#include <memory/pmm.h>
#include <memory/mm.h>
#include <acpi/acpi.h>
#include <pci/pci.h>
#include <smp/smp.h>
#include <helpers/hpet/hpet.h>
#include <processes/scheduler.h>
#include <processes/process.h>
#include <drivers/ahci/ahci.h>

static void kernel_thread(tboot_info_t* info) {
    error_t err = NO_ERROR;
    log_info("In kernel thread!");

    _cli();

    /////////////////////////////
    // do driver initialization
    /////////////////////////////
    log_info("Doing driver initialization");

    // Storage device initialization
    CATCH(ahci_init());

    // TODO: Partition initialization

    // TODO: Filesystem initialization

    // TODO: USB Initialization

    // TODO: Mouse & Keyboard initialization

    // TODO: Loopback + Network stack initialization

    // Network drivers initialization
    CATCH(rtl8169_init());

    /////////////////////////////
    // Kick start everything
    /////////////////////////////

    _sti();

cleanup:
    // TODO: kill thread
    while(true) _hlt();
}

/**
 * This is the main core initialization sequence
 * @param info
 */
void kernel_main(uint32_t magic, tboot_info_t* info) {
    error_t err = NO_ERROR;

    // register the early loggers
    vmdev_register_logger();
    term_early_init(info);

    // setup the basic
    idt_init();

    log_info("Well hello there :)");

    // check the params
    CHECK_TRACE(magic == TBOOT_MAGIC, "tboot magic is invalid!");
    CHECK_TRACE(info, "tboot info is null!");

    // full memory initialization
    CHECK_AND_RETHROW(pmm_early_init(info));
    CHECK_AND_RETHROW(vmm_init(info));

    info = CONVERT_TO_DIRECT(info);
    info->framebuffer.addr = CONVERT_TO_DIRECT(info->framebuffer.addr);
    info->mmap.entries = CONVERT_TO_DIRECT(info->mmap.entries);
    info->rsdp = CONVERT_TO_DIRECT(info->rsdp);

    // finish memory initialization
    CHECK_AND_RETHROW(pmm_init());
    CHECK_AND_RETHROW(mm_init());

    term_init();
    term_clear();

    // start doing the late early initialization
    CHECK_AND_RETHROW(acpi_tables_init(info)); // full ACPI will be set later
    CHECK_AND_RETHROW(interrupts_init());

    // do the smp
    // don't forget to init the hpet first for the stall
    CHECK_AND_RETHROW(hpet_init());
    CHECK_AND_RETHROW(smp_init());

    // start getting the basic helpers
    CHECK_AND_RETHROW(acpi_init());
    CHECK_AND_RETHROW(pci_init());

    // Create the kernel process and thread
    log_info("Finished early kernel initialization!");

    // create the main kernel thread
    thread_t* thread;
    CHECK_AND_RETHROW(create_process(kernel_address_space, &kernel_process));

    CHECK_AND_RETHROW(create_thread(kernel_process, &thread));
    thread->context.cpu.rip = (uint64_t) kernel_thread;
    thread->context.cpu.rdi = (uint64_t) info;
    thread->status = THREAD_STATUS_NORMAL;
    CHECK_AND_RETHROW(scheduler_queue_thread(thread));

    CHECK_AND_RETHROW(scheduler_init());
    CHECK_AND_RETHROW(scheduler_kickstart());

    _sti();

    while(true) {
        _hlt();
    }

cleanup:
    log_critical("Error during early kernel initialization :(");
    _cli(); while(true) _hlt();
}