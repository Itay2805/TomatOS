#include "lapic.h"

#include <interrupts/interrupts.h>
#include <acpi/tables/madt.h>
#include <memory/vmm.h>
#include <stb/stb_ds.h>
#include <cpu/atomic.h>
#include "ioapic.h"

error_t ioapic_init() {
    error_t err = NO_ERROR;

    log_info("\tI/O APIC initialization");

    // make sure all the ioapics are mapped
    for(madt_ioapic_t** it = madt_ioapics; it < madt_ioapics + arrlen(madt_ioapics); it++) {
        madt_ioapic_t* ioapic = *it;
        log_info("\t\tMapping #%d IOAPIC (0x%p)", ioapic->id, ioapic->mmio_base);
        CHECK_AND_RETHROW(vmm_map_direct(ioapic->mmio_base, KB(4), (page_attrs_t){ .write = true, .global = true }));
    }

cleanup:
    return err;
}

void ioapic_write(madt_ioapic_t* ioapic, uint32_t reg, uint32_t value) {
    VOL_POKE32(CONVERT_TO_DIRECT((uintptr_t)ioapic->mmio_base)) = reg;
    VOL_POKE32(CONVERT_TO_DIRECT((uintptr_t)ioapic->mmio_base + 0x10)) = value;
}

uint32_t ioapic_read(madt_ioapic_t* ioapic, uint32_t reg) {
    VOL_POKE32(CONVERT_TO_DIRECT((uintptr_t)ioapic->mmio_base)) = reg;
    return VOL_POKE32(CONVERT_TO_DIRECT((uintptr_t)ioapic->mmio_base + 0x10));
}

void ioapic_get_range(madt_ioapic_t* ioapic, uint32_t* gsi_start, uint32_t* gsi_end) {
    *gsi_start = ioapic->gsi_base;
    uint32_t ver = ioapic_read(ioapic, IOAPIC_REG_VERSION);
    *gsi_end = ioapic->gsi_base + ((ver >> 16) & 0xFF);
}

madt_ioapic_t* ioapic_get_from_gsi(uint32_t gsi) {
    for(madt_ioapic_t** it = madt_ioapics; it < madt_ioapics + arrlen(madt_ioapics); it++) {
        madt_ioapic_t* ioapic = *it;
        uint32_t gsi_start, gsi_end;
        ioapic_get_range(ioapic, &gsi_start, &gsi_end);
        if(gsi >= gsi_start && gsi <= gsi_end) {
            return ioapic;
        }
    }

    return NULL;
}

// TODO: get the destination as parameter
error_t ioapic_redirect(uint32_t irq, uint8_t vector, int flags) {
    error_t err = NO_ERROR;
    uint32_t gsi = irq;

    // make sure the vector is valid
    CHECK((uint32_t)vector < 255);

    // check for any overrides
    for(madt_iso_t** it = madt_isos; it < madt_isos + arrlen(madt_isos); it++) {
        madt_iso_t* iso = *it;
        if(iso->irq_source == irq) {
            gsi = iso->gsi;
            flags = iso->flags;
            break;
        }
    }

    // get the correct ioapic
    madt_ioapic_t* ioapic = ioapic_get_from_gsi(gsi);
    CHECK_ERROR_TRACE(ioapic, ERROR_NOT_SUPPORTED, "Given IRQ can not be handled by any of the I/O APICs (%d)", gsi);

    // set the parameters
    ioapic_redirection_entry_t redir = {
        .vector = vector,
        .destination_mode = 0,
        .destination_id = lapic_get_id(),  // all lapics
        .polarity  = (uint64_t) ((flags & MADT_FLAG_ACTIVE_LOW) != 0),
        .trigger_mode = (uint64_t) ((flags & MADT_FLAG_LEVEL_TRIGGERED) != 0),
        .delievery_mode = IOAPIC_DELIVERY_MODE_FIXED,
        .mask = false,
    };

    // adjust the base
    gsi -= ioapic->gsi_base;
    ioapic_write(ioapic, IOAPIC_REG_REDIRECTION_TABLE_BASE + gsi * 2 + 0, redir.raw_low);
    ioapic_write(ioapic, IOAPIC_REG_REDIRECTION_TABLE_BASE + gsi * 2 + 1, redir.raw_high);

cleanup:
    return err;
}