#include <acpi/tables/hpet.h>
#include <lai/core.h>
#include <lai/helpers/sci.h>
#include <interrupts/apic/ioapic.h>
#include <interrupts/interrupts.h>
#include "acpi.h"

#include "acpi/tables/rsdp.h"
#include "acpi/tables/rsdt.h"
#include "acpi/tables/mcfg.h"
#include "acpi/tables/madt.h"
#include "acpi/tables/fadt.h"

error_t acpi_tables_init(tboot_info_t* info) {
    error_t err = NO_ERROR;

    log_info("Initializing ACPI tables");
    CHECK_AND_RETHROW(rsdp_init(info));
    CHECK_AND_RETHROW(rsdt_init());
    CHECK_AND_RETHROW(fadt_init());
    CHECK_AND_RETHROW(madt_init());
    CHECK_AND_RETHROW(fadt_init());

    // these tables are optional
    CHECK_AND_RETHROW(hpet_table_init());
    CHECK_AND_RETHROW(mcfg_init());

cleanup:
    return err;
}

static error_t sci_handler(registers_t* registers) {
    error_t err = NO_ERROR;

    // get the event
    uint16_t sci_event = lai_get_sci_event();
    log_info("Got SCI event %d", sci_event);

cleanup:
    return err;
}


error_t acpi_init() {
    error_t err = NO_ERROR;

    log_info("Initializing LAI");

    // create the lai namespace
    lai_create_namespace();

    // enable acpi (apic mode)
    log_info("\tEnabling ACPI mode");
    CHECK(lai_enable_acpi(1) == 0);

    // register an sci handler
    uint8_t vec = interrupt_allocate();
    CHECK_AND_RETHROW(ioapic_redirect(fadt->sci_irq, vec, 0));
    CHECK_AND_RETHROW(interrupt_register(vec, sci_handler));
    log_info("\t\tAdded SCI handler (#%d -> #%d)", fadt->sci_irq, vec);

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
