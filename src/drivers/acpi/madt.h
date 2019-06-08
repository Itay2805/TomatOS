#ifndef TOMATKERNEL_MADT_H
#define TOMATKERNEL_MADT_H

#include "rsdt.h"

#define MADT_TYPE_LAPIC             0
#define MADT_TYPE_IOAPIC            1
#define MADT_TYPE_ISO               2
#define MADT_TYPE_NMI               4

#define MADT_FLAG_ACTIVE_LOW        2
#define MADT_FLAG_LEVEL_TRIGGERED   8

typedef struct madt_lapic {
    uint8_t acpi_processor_id;
    uint8_t apic_id;
    uint32_t processor_enabled : 1;
} __attribute__((packed)) madt_lapic_t;

typedef struct madt_ioapic {
    uint8_t ioapic_id;
    uint8_t reserved;
    uint32_t ioapic_addr;
    uint32_t global_system_interrupt_base;
} __attribute__((packed)) madt_ioapic_t;

// interrupt source override
typedef struct madt_iso {
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t global_system_interrupt;
    uint16_t flags;
} __attribute__((packed)) madt_iso_t;

// none mastkable interrupt
typedef struct madt_nmi {
    uint8_t acpi_processor_id;
    uint16_t flags;
    uint8_t lint;
} __attribute__((packed)) madt_nmi_t;

typedef struct madt_entry {
    uint8_t type;
    uint8_t length;
    union {
        madt_lapic_t lapic;
        madt_ioapic_t ioapic;
        madt_iso_t iso;
        madt_nmi_t nmi;
    };
} __attribute__((packed)) madt_entry_t;

typedef struct madt {
    sdt_hdr_t header;

    uint32_t lapic_addr;
    uint32_t flags;
    madt_entry_t entries[0];
} __attribute__((packed)) madt_t;

extern madt_t* madt;
extern madt_lapic_t** madt_lapics;
extern madt_ioapic_t** madt_ioapics;
extern madt_iso_t** madt_isos;
extern madt_nmi_t** madt_nmis;

/**
 * Find the MADT
 */
error_t madt_init();

#endif //TOMATKERNEL_MADT_H
