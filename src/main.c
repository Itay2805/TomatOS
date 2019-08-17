#include <common/error.h>
#include <tboot/tboot.h>
#include <interrupts/interrupts.h>
#include <memory/pmm.h>
#include <memory/mm.h>
#include <memory/gdt.h>
#include <interrupts/idt.h>
#include <acpi/acpi.h>
#include <logger/vmdev/vmdev.h>
#include <logger/term/term.h>
#include <pci/pci.h>
#include <helpers/hpet/hpet.h>
#include <common/locks/spinlock.h>
#include <acpi/tables/madt.h>
#include <stb/stb_ds.h>
#include <helpers/portio.h>
#include <lai/core.h>
#include <lai/helpers/sci.h>
#include <processes/process.h>
#include <processes/thread.h>
#include <processes/scheduler.h>
#include <smp/smp.h>
#include <objects/drivers/ahci/ahci.h>
#include <resources/drivers/framebuffer/framebuffer.h>
#include <objects/screen.h>

static void kernel_thread(tboot_info_t* info) {
    error_t err = NO_ERROR;
    log_info("In kernel thread!");

    // do driver initialization
    CATCH(framebuffer_init(info));

    object_t* screen;
    CHECK_AND_RETHROW(object_get_primary(TRAIT_SCREEN, &screen));
    screen_functions_t* functions = (screen_functions_t*)screen->traits[TRAIT_SCREEN].functions;
    CHECK_AND_RETHROW(functions->clear(screen));

    CATCH(ahci_init());

cleanup:
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