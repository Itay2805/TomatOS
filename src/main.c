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
    gdt_init();
    idt_init();

    // full memory initialization
    CHECK_AND_RETHROW(pmm_early_init(info));
    CHECK_AND_RETHROW(vmm_init(info));

    // anything which needs to convert the addresses to the direct mapping
    term_init();

    // finish memory initialization
    CHECK_AND_RETHROW(pmm_init());
    CHECK_AND_RETHROW(mm_init());

    // convert the boot info the the direct mapping
    info->framebuffer.addr = CONVERT_TO_DIRECT(info->framebuffer.addr);

    // start doing the late early initialization
    CHECK_AND_RETHROW(acpi_tables_init(info)); // full ACPI will be set later
    CHECK_AND_RETHROW(interrupts_init());

    // start getting the basic drivers
    CHECK_AND_RETHROW(pci_init());
    // TODO: HPET init

    // TODO: SMP

    // TODO: Objects Manager init

    // TODO: Create initial kernel thread
    // TODO: Kick start processes
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
    while(true);
}