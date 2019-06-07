#include <boot/multiboot.h>

#include <drivers/vmdev/vmdev.h>
#include <drivers/acpi/acpi.h>

#include <interrupts/interrupts.h>
#include <interrupts/idt.h>

#include <logger/logger.h>

#include <memory/gdt.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <memory/mm.h>

#include <common.h>
#include <error.h>

void kernel_main(multiboot_info_t* info) {
    error_t err = NO_ERROR;

    /*********************************************************
     * Early initialization
     *********************************************************/

    // we start by creating a logger, allows us to actually log stuff
    vmdev_register_logger();

    // initialize the idt and gdt
    idt_init();
    gdt_init();

    /*********************************************************
     * Early memory initialization
     * can start using errors
     *********************************************************/
    // TODO: Maybe put in the pre init arrays
    CHECK_AND_RETHROW(pmm_early_init(info));
    CHECK_AND_RETHROW(vmm_init(info));

    /*********************************************************
     * Initialization of everything else
     *********************************************************/
    // TODO: run pre-init arrays

    // now finish initialization of the pmm and initialize mm
    CHECK_AND_RETHROW(pmm_init());
    CHECK_AND_RETHROW(mm_init());

    // everything else can be initialized now
    CHECK_AND_RETHROW(acpi_init());

    log_info("initialization finished");

    /*********************************************************
     * Initialization completed
     *********************************************************/

    // TODO: start the scheduler
    // _sti();

    while(true) {
        _hlt();
    }

cleanup:
    log_critical("Error during kernel initialization :(");
    while(true);
}