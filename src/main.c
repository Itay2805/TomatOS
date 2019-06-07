#include <boot/multiboot.h>

#include <drivers/e9hack/e9hack.h>

#include <interrupts/idt.h>

#include <logger/logger.h>

#include <memory/gdt.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <memory/mm.h>

#include <common.h>
#include <error.h>
#include <interrupts/interrupts.h>
#include <drivers/acpi/acpi.h>

void kernel_main(multiboot_info_t* info) {
    error_t err = NO_ERROR;

    // we start by creating a logger, allows us to actually log stuff
    e9hack_register_logger();

    // initialize the idt first, so we can catch errors
    idt_init();

    // now have a proper gdt
    gdt_init();

    // early initialization of the pmm and vmm comes next
    // TODO: Maybe put in the pre init arrays
    CHECK_AND_RETHROW(pmm_early_init(info));
    CHECK_AND_RETHROW(vmm_init(info));

    log_info("finished early initialization");

    // TODO: run pre-init arrays

    // now finish initialization of the pmm and initialize mm
    CHECK_AND_RETHROW(pmm_init());
    CHECK_AND_RETHROW(mm_init());

    CHECK_AND_RETHROW(acpi_init());

    log_info("initialization finished");

    // TODO: start the scheduler
    // _sti();

    while(true) {
        _hlt();
    }

cleanup:
    log_critical("Error during kernel initialization :(");
}