#ifndef TOMATKERNEL_MADT_H
#define TOMATKERNEL_MADT_H

#include <stdint.h>
#include "rsdt.h"

#define MADT_TYPE_LAPIC             0
#define MADT_TYPE_IOAPIC            1
#define MADT_TYPE_ISO               2
#define MADT_TYPE_NMI               4

#define MADT_FLAG_ACTIVE_LOW        2
#define MADT_FLAG_LEVEL_TRIGGERED   8

#define MADT_FLAGS_PIC8259          (1 << 0)

typedef struct madt_lapic {
    uint8_t processor_id;
    uint8_t id;
    uint32_t processor_enabled : 1;
} __attribute__((packed)) madt_lapic_t;

typedef struct madt_ioapic {
    uint8_t id;
    uint8_t reserved;
    uint32_t mmio_base;
    uint32_t gsi_base;
} __attribute__((packed)) madt_ioapic_t;

// interrupt source override
typedef struct madt_iso {
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t gsi;
    uint16_t flags;
} __attribute__((packed)) madt_iso_t;

// none mastkable interrupt
typedef struct madt_nmi {
    uint8_t processor_id;
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

typedef struct acpi_madt {
    acpi_header_t header;

    uint32_t lapic_addr;
    uint32_t flags;
    madt_entry_t entries[0];
} __attribute__((packed)) acpi_madt_t;

extern acpi_madt_t* acpi_madt;

void madt_init();

/**
 * Get the next entry of the given type
 *
 * @param type  [IN] The type
 * @param index [IN] The index
 *
 * @return the entry, NULL if not found
 */
void* madt_get_next(int type, int index);

#endif //TOMATKERNEL_MADT_H
