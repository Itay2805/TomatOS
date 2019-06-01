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

void kernel_main(multiboot_info_t* info) {
    error_t err = NO_ERROR;
    e9hack_register_logger();

    idt_init();
    gdt_init();

    CHECK_AND_RETHROW(pmm_early_init(info));
    CHECK_AND_RETHROW(vmm_init(info));

    log_info("finished early initialization");

    // TODO: run pre-init arrays

    CHECK_AND_RETHROW(pmm_init());
    CHECK_AND_RETHROW(mm_init());

    // TODO: run init arrays

    log_info("initialization finished");

    while(true) {
    }

cleanup:
    log_critical("Error during kernel initialization :(");
}