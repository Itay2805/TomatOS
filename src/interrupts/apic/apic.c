#include "apic.h"

#include <interrupts/pic8259/pic.h>
#include <acpi/tables/madt.h>
#include <memory/vmm.h>
#include <cpu/msr.h>
#include <stb/stb_ds.h>

#include "lapic.h"
#include "ioapic.h"

static uint32_t lapic_apic_map[255] = {};

error_t apic_init() {
    error_t err = NO_ERROR;

    // make sure we have everything we need
    log_notice("Initializing APIC");
    CHECK_ERROR_TRACE(madt, ERROR_NOT_FOUND, "APIC table not found!");
    CHECK_ERROR_TRACE(madt_lapics, ERROR_NOT_FOUND, "No Local APICs found");
    CHECK_ERROR_TRACE(madt_ioapics, ERROR_NOT_FOUND, "No I/O APICs found");

    if(pic8259_available()) {
        log_info("Found PIC8259, disabling");
        pic8259_disable();
    }

    log_debug("\tEnabling APIC globally");
    _wrmsr(IA32_APIC_BASE, _rdmsr(IA32_APIC_BASE) | (1 << 11));

    // create the map between lapic->procid
    for(madt_lapic_t** it = madt_lapics; it < madt_lapics + arrlen(madt_lapics); it++) {
        madt_lapic_t* lapic = *it;
        if(!lapic->processor_enabled) continue;
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
