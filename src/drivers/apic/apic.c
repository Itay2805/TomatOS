#include "apic.h"

#include <drivers/acpi/madt.h>
#include <memory/vmm.h>

#include "lapic.h"

error_t apic_init() {
    error_t err = NO_ERROR;

    // make sure we have everything we need
    log_info("Initializing APIC");
    CHECK_ERROR_TRACE(madt, ERROR_NOT_FOUND, "APIC table not found!");
    CHECK_ERROR_TRACE(madt_lapics, ERROR_NOT_FOUND, "No Local APICs found");
    CHECK_ERROR_TRACE(madt_ioapics, ERROR_NOT_FOUND, "No I/O APICs found");

    CHECK_AND_RETHROW(lapic_init());

cleanup:
    return err;
}
