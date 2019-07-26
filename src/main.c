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
#include <smp/cpustorage.h>
#include <drivers/hpet/hpet.h>
#include <common/locks/spinlock.h>
#include <acpi/tables/madt.h>
#include <stb/stb_ds.h>
#include <common/locks/event.h>
#include <smp/smp.h>
#include <drivers/portio.h>
#include <lai/core.h>
#include <lai/helpers/sci.h>

/**
 * This is the main core initialization sequence
 * @param info
 */
void kernel_main(uint32_t magic, tboot_info_t* info) {
    error_t err = NO_ERROR;

    // register the early loggers
    vmdev_register_logger();
    //term_early_init(info);

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

    //term_init();
    //term_clear();

    // start doing the late early initialization
    CHECK_AND_RETHROW(acpi_tables_init(info)); // full ACPI will be set later
    CHECK_AND_RETHROW(interrupts_init());

    // start getting the basic drivers
    CHECK_AND_RETHROW(hpet_init());
    CHECK_AND_RETHROW(acpi_init());
    CHECK_AND_RETHROW(pci_init());

    // we are now ready to startup SMP
    CHECK_AND_RETHROW(smp_init());

    // TODO: Create initial kernel thread
    // TODO: Kick start processes
    // TODO:    - do PCI iteration and driver loading
    // TODO:    - start ACPI namespace
    // TODO:    - start driver loading
    // TODO:    - load the init process
    // TODO:    - ???
    // TODO:    - profit!

    _sti();

    while(true) {
        _hlt();
    }

cleanup:
    log_critical("Error during kernel initialization :(");
    _cli(); while(true) _hlt();
}