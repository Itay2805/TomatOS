#include <boot/multiboot.h>

#include <drivers/vmdev/vmdev.h>
#include <drivers/pic8259/pic.h>
#include <drivers/acpi/acpi.h>
#include <drivers/apic/apic.h>
#include <drivers/pci/pci.h>

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
     *
     * basically setting a nice enviroment to work in
     *********************************************************/

    // we start by creating a logger, allows us to actually log stuff
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

    /*********************************************************
     * Initialization of essentials
     *
     * we require everything in here to work
     *********************************************************/
    CHECK_AND_RETHROW(pmm_init());
    CHECK_AND_RETHROW(mm_init());
    CHECK_AND_RETHROW(acpi_init());
    CHECK_AND_RETHROW(pic8259_disable());
    CHECK_AND_RETHROW(apic_init());

    /*********************************************************
     * Driver initialization
     *
     * anything in here might not be loaded depending
     * on the computer configuration
     *********************************************************/
    pci_init();

    /*********************************************************
     * Initialization completed
     *********************************************************/
    log_info("initialization finished");

    // TODO: start the scheduler
    _sti();

    while(true) {
        _hlt();
    }

cleanup:
    log_critical("Error during kernel initialization :(");
    while(true);
}