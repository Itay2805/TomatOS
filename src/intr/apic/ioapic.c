#include <util/except.h>

#include <stb_ds.h>
#include <util/def.h>
#include <mm/vmm.h>
#include <acpi/tables/madt.h>
#include <proc/process.h>

#include "apic_spec.h"
#include "ioapic.h"
#include "lapic.h"

typedef struct ioapic {
    uintptr_t base;
    size_t irq_start;
    size_t irq_end;
} ioapic_t;

static ioapic_t* ioapics = NULL;
static madt_iso_t* isos = NULL;

static uint32_t ioapic_read(ioapic_t* apic, size_t index) {
    ASSERT(index < 0x100);
    POKE8(apic->base + IOAPIC_INDEX_OFFSET) = index;
    return POKE32(apic->base + IOAPIC_DATA_OFFSET);
}

static void ioapic_write(ioapic_t* apic, size_t index, uint32_t value) {
    ASSERT(index < 0x100);
    POKE8(apic->base + IOAPIC_INDEX_OFFSET) = index;
    POKE32(apic->base + IOAPIC_DATA_OFFSET) = value;
}

void ioapic_add(uintptr_t mmio_base, size_t gsi_base) {
    // setup the entry
    ioapic_t apic = {
        .base = PHYSICAL_TO_DIRECT(mmio_base),
        .irq_start = gsi_base
    };

    // make sure the area is mapped
    vmm_map(&kernel_process.vmm_handle, mmio_base, PHYSICAL_TO_DIRECT(mmio_base), 0x100 * 4,
            PAGE_SUPERVISOR_READWRITE, DEFAULT_CACHE);

    // get the range
    IO_APIC_VERSION_REGISTER version = { .raw = ioapic_read(&apic, IO_APIC_VERSION_REGISTER_INDEX) };
    apic.irq_end = gsi_base + version.maximum_redirection_entry;

    // submit it
    arrpush(ioapics, apic);
}

void ioapic_add_iso(madt_iso_t* iso) {
    arrpush(isos, *iso);
}

static ioapic_t* get_ioapic(size_t irq) {
    for (int i = 0; i < arrlen(ioapics); i++) {
        if (ioapics[i].irq_start <= irq && irq <= ioapics[i].irq_end) {
            return &ioapics[i];
        }
    }
    ASSERT(!"Could not find I/O APIC with an irq in range");
}

void ioapic_enable_interrupt(size_t irq, bool enable) {
    ioapic_t* apic = get_ioapic(irq);

    for (int i = 0; i < arrlen(isos); i++) {
        if (isos[i].irq_source == irq) {
            irq = isos[i].gsi;
        }
    }

    // read the entry
    IO_APIC_REDIRECTION_TABLE_ENTRY entry = {
        .raw_split.low = ioapic_read(apic, IO_APIC_REDIRECTION_TABLE_ENTRY_INDEX + irq * 2),
        .raw_split.high = ioapic_read(apic, IO_APIC_REDIRECTION_TABLE_ENTRY_INDEX + irq * 2 + 1)
    };

    // modify it
    entry.mask = enable ? 0 : 1;

    // write it
    ioapic_write(apic, IO_APIC_REDIRECTION_TABLE_ENTRY_INDEX + irq * 2, entry.raw_split.low);
    ioapic_write(apic, IO_APIC_REDIRECTION_TABLE_ENTRY_INDEX + irq * 2 + 1, entry.raw_split.high);
}

void ioapic_configure_interrupt(size_t irq, size_t vector, ioapic_delivery_mode_t delivery_mode, bool level_triggered, bool assertion_level) {
    ioapic_t* apic = get_ioapic(irq);

    for (int i = 0; i < arrlen(isos); i++) {
        if (isos[i].irq_source == irq) {
            irq = isos[i].gsi;
            level_triggered = isos[i].flags & MADT_FLAG_LEVEL_TRIGGERED;
            assertion_level = isos[i].flags & MADT_FLAG_ACTIVE_LOW ? 0 : 1;
        }
    }

    ASSERT(vector <= 0xFF);

    // setup the entry
    IO_APIC_REDIRECTION_TABLE_ENTRY entry = {
        .vector = vector,
        .delivery_mode = delivery_mode,
        .destination_mode = 0,
        .polarity = assertion_level ? 0 : 1,
        .trigger_mode = level_triggered ? 1 : 0,
        .mask = 1,
        .destination_id = get_apic_id()
    };

    // write it
    ioapic_write(apic, IO_APIC_REDIRECTION_TABLE_ENTRY_INDEX + irq * 2, entry.raw_split.low);
    ioapic_write(apic, IO_APIC_REDIRECTION_TABLE_ENTRY_INDEX + irq * 2 + 1, entry.raw_split.high);
}
