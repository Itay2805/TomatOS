#include <boot/multiboot.h>

#include <logger/logger.h>

#include <memory/gdt.h>

#include <common.h>
#include <error.h>
#include <drivers/e9hack/e9hack.h>
#include <memory/pmm.h>
#include <memory/vmm.h>

void kernel_main(multiboot_info_t* info) {
    error_t err = NO_ERROR;
    e9hack_register_logger();

    gdt_init();
    CHECK_AND_RETHROW(pmm_early_init(info));
    CHECK_AND_RETHROW(vmm_init(info));

    // TODO: run pre-init arrays

    log_info("finished early initialization");

    // TODO: run init arrays

    CHECK_AND_RETHROW(pmm_init());

    log_info("initialization finished");

    uint8_t i = 0;
    while(true) {
        // POKE8(0xFFFF800000000000ll + 0xB8000 + 1) = i++;
    }

cleanup:
    log_critical("Error during kernel initialization :(");
}