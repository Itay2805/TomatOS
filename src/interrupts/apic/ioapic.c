#include <libc/stddef.h>
#include <memory/vmm.h>
#include <util/defs.h>
#include "ioapic.h"

void ioapic_write(madt_ioapic_t* ioapic, uint32_t reg, uint32_t value) {
    POKE32(PHYSICAL_TO_DIRECT((uintptr_t)ioapic->mmio_base)) = reg;
    POKE32(PHYSICAL_TO_DIRECT((uintptr_t)ioapic->mmio_base + 0x10)) = value;
}

uint32_t ioapic_read(madt_ioapic_t* ioapic, uint32_t reg) {
    POKE32(PHYSICAL_TO_DIRECT((uintptr_t)ioapic->mmio_base)) = reg;
    return POKE32(PHYSICAL_TO_DIRECT((uintptr_t)ioapic->mmio_base + 0x10));
}

void ioapic_get_range(madt_ioapic_t* ioapic, uint32_t* gsi_start, uint32_t* gsi_end) {
    *gsi_start = ioapic->gsi_base;
    uint32_t ver = ioapic_read(ioapic, IOAPIC_REG_VERSION);
    *gsi_end = ioapic->gsi_base + ((ver >> 16) & 0xFF);
}

madt_ioapic_t* ioapic_get_from_gsi(uint32_t gsi) {
    for(int i = 0; ; i++) {
        madt_ioapic_t* ioapic = madt_get_next(MADT_TYPE_IOAPIC, i);
        if(ioapic == NULL) break;

        uint32_t gsi_start, gsi_end;
        ioapic_get_range(ioapic, &gsi_start, &gsi_end);
        if(gsi >= gsi_start && gsi <= gsi_end) {
            return ioapic;
        }
    }

    return NULL;
}

error_t ioapic_redirect(uint32_t irq, uint8_t vector, int flags) {
    error_t err = NO_ERROR;
    uint32_t gsi = irq;

    // make sure the vector is valid
    CHECK((uint32_t)vector < 255);

    // check for any overrides
    for(int i = 0; ; i++) {
        madt_iso_t* iso = madt_get_next(MADT_TYPE_ISO, i);
        if(iso == NULL) break;

        if(iso->irq_source == irq) {
            gsi = iso->gsi;
            flags = iso->flags;
            break;
        }
    }

    // get the correct ioapic
    madt_ioapic_t* ioapic = ioapic_get_from_gsi(gsi);
    CHECK_ERROR_DEBUG(ioapic, ERROR_NOT_FOUND, "Given IRQ can not be handled by any of the I/O APICs (%d)", gsi);

    // set the parameters
    ioapic_redirection_entry_t redir = {
            .vector = vector,
            .destination_mode = 0,
            .destination_id = 0xFF,
            .polarity  = (uint64_t) ((flags & MADT_FLAG_ACTIVE_LOW) != 0),
            .trigger_mode = (uint64_t) ((flags & MADT_FLAG_LEVEL_TRIGGERED) != 0),
            .delievery_mode = IOAPIC_DELIVERY_MODE_LOWEST_PRIORITY,
            .mask = false,
    };

    // adjust the base
    gsi -= ioapic->gsi_base;
    ioapic_write(ioapic, IOAPIC_REG_REDIRECTION_TABLE_BASE + gsi * 2 + 0, redir.raw_low);
    ioapic_write(ioapic, IOAPIC_REG_REDIRECTION_TABLE_BASE + gsi * 2 + 1, redir.raw_high);

cleanup:
    return err;
}
