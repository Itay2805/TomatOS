#ifndef TOMATKERNEL_MADT_H
#define TOMATKERNEL_MADT_H

#include "rsdt.h"

typedef struct madt_entry {
    uint8_t type;
    uint8_t length;
    union {
        struct {
            uint8_t acpi_processor_id;
            uint8_t apic_id;
            uint32_t processor_enabled : 1;
        } __attribute__((packed)) processor_lapic;
        struct {
            uint8_t ioapic_id;
            uint8_t reserved;
            uint32_t ioapic_addr;
            uint32_t global_system_interrupt_base;
        } __attribute__((packed)) ioapic;
        struct {
            uint8_t bus_source;
            uint8_t irq_source;
            uint32_t global_system_interrupt;
            uint16_t flags;
        } __attribute__((packed)) interrupt_source_override;
        struct {
            uint8_t acpi_processor_id;
            uint16_t flags;
            uint8_t lint;
        } __attribute__((packed)) non_maskable_interrupts;
        struct {
            uint16_t reserved;
            uint64_t lapic_addr;
        } __attribute__((packed)) lapic_address_override;
    };
} __attribute__((packed)) madt_entry_t;

typedef struct madt {
    sdt_hdr_t header;

    uint32_t lapic_addr;
    uint32_t flags;
} __attribute__((packed)) madt_t;

extern madt_t* madt;

/**
 * Find the MADT
 */
error_t madt_init();

#endif //TOMATKERNEL_MADT_H
