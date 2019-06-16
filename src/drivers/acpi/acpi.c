#include <memory/vmm.h>
#include <lai/core.h>
#include <drivers/apic/ioapic.h>
#include <interrupts/irq.h>
#include <stdlib.h>
#include "acpi.h"

#include "tables/rsdp.h"
#include "tables/rsdt.h"
#include "tables/mcfg.h"
#include "tables/madt.h"
#include "tables/fadt.h"

error_t acpi_tables_init(boot_info_t* info) {
    error_t err = NO_ERROR;

    log_notice("Initializing ACPI tables");
    CHECK_AND_RETHROW(rsdp_init(info));
    CHECK_AND_RETHROW(rsdt_init());
    CHECK_AND_RETHROW(fadt_init());
    CHECK_AND_RETHROW(madt_init());
    CHECK_AND_RETHROW(fadt_init());

    mcfg_init();

cleanup:
    return err;
}

static error_t sci_handler(registers_t* regs) {
    uint16_t sci_event = lai_get_sci_event();
    log_info("Got SCI event %d", sci_event);
    return NO_ERROR;
}

error_t acpi_init() {
    error_t err = NO_ERROR;

    log_info("Starting ACPI namespace");
    lai_create_namespace();

    log_info("Setting SCI handler");
    irq_set_handler(10, sci_handler);
    CHECK_AND_RETHROW(ioapic_redirect(fadt->sci_irq, 10));

//    lai_enable_tracing(true);
    lai_enable_acpi(true);

cleanup:
    return err;
}

bool acpi_validate_checksum(const void* addr, size_t size) {
    uint8_t checksum = 0;
    const char* buf = addr;
    for(int i = 0; i < size; i++) {
        checksum += buf[i];
    }
    return checksum == 0;
}
