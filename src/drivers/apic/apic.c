#include "apic.h"

#include <drivers/acpi/madt.h>
#include <memory/vmm.h>
#include <cpu/msr.h>
#include <buf.h>

#include "lapic.h"
#include "ioapic.h"

uint32_t lapic_apic_map[255] = {};

error_t apic_init() {
    error_t err = NO_ERROR;

    // make sure we have everything we need
    log_info("Initializing APIC");
    CHECK_ERROR_TRACE(madt, ERROR_NOT_FOUND, "APIC table not found!");
    CHECK_ERROR_TRACE(madt_lapics, ERROR_NOT_FOUND, "No Local APICs found");
    CHECK_ERROR_TRACE(madt_ioapics, ERROR_NOT_FOUND, "No I/O APICs found");

    log_debug("\tEnabling APIC globally");
    _wrmsr(IA32_APIC_BASE, _rdmsr(IA32_APIC_BASE) | (1 << 11));

    // create the map between lapic->procid
    for(madt_lapic_t** it = madt_lapics; it < buf_end(madt_lapics); it++) {
        madt_lapic_t* lapic = *it;
        CHECK(lapic->id < 255);
        lapic_apic_map[lapic->id] = lapic->processor_id;
    }

    CHECK_AND_RETHROW(lapic_init());
    CHECK_AND_RETHROW(ioapic_init());

cleanup:
    return err;
}

uint64_t apic_get_processor_id() {
    return lapic_apic_map[lapic_get_id()];
}
