#include <drivers/acpi/madt.h>
#include <memory/vmm.h>
#include <interrupts/interrupts.h>
#include <buf.h>
#include "ioapic.h"
#include "lapic.h"

static char* mmio_base;
static uint32_t* ioregsel;
static uint32_t* iowin;
static madt_ioapic_t* ioapic;

static error_t redirect_isos() {
    // handle special properties
    for(madt_iso_t** it = madt_isos; it < buf_end(madt_isos); it++) {
        madt_iso_t* iso = *it;

        ioapic_redirection_entry_t redir = {
            .vector = iso->irq_source + INTERRUPT_IRQ_BASE,
            .destination = lapic_get_id(),
            .low_active = (uint64_t) ((iso->flags & MADT_FLAG_ACTIVE_LOW) != 0),
            .edge_sensitive = (uint64_t) ((iso->flags & MADT_FLAG_LEVEL_TRIGGERED) != 0),
        };

        lapic_write(IOAPIC_REG_REDIRECTION_TABLE_BASE + iso->gsi * 2 + 0, redir.raw_low);
        lapic_write(IOAPIC_REG_REDIRECTION_TABLE_BASE + iso->gsi * 2 + 1, redir.raw_high);
    }

    return NO_ERROR;
}

error_t ioapic_init() {
    error_t err = NO_ERROR;

    log_info("\tI/O APIC initialization #0");

    // TODO: Handling multiple IOAPICs?
    ioapic = madt_ioapics[0];

    // setup all of the mmio stuff
    mmio_base = PHYSICAL_ADDRESS(ioapic->mmio_base);
    ioregsel = PHYSICAL_ADDRESS(ioapic->mmio_base);
    iowin = PHYSICAL_ADDRESS(ioapic->mmio_base + 0x10);
    if(!vmm_is_mapped(kernel_address_space, (uintptr_t) PHYSICAL_ADDRESS(ioapic->mmio_base))) {
        vmm_map(kernel_address_space, PHYSICAL_ADDRESS(ioapic->mmio_base), (void *)(uintptr_t)ioapic->mmio_base, PAGE_ATTR_WRITE);
    }

    log_debug("\t\tRedirecting ISOs (Local APIC #%d)", lapic_get_id());
    CHECK_AND_RETHROW(redirect_isos());

cleanup:
    return err;
}

void ioapic_write(uint32_t reg, uint32_t value) {
    *ioregsel = reg;
    *iowin = value;
}

uint32_t ioapic_read(uint32_t reg) {
    *ioregsel = reg;
    return *iowin;
}