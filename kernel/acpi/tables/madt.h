#ifndef __TOMATOS_KERNEL_ACPI_TABLES_MADT_H__
#define __TOMATOS_KERNEL_ACPI_TABLES_MADT_H__

#include <acpispec/tables.h>
#include <util/defs.h>

typedef struct acpi_madt {
    acpi_header_t header;
    uint32_t lapic_address;
    uint32_t pcat_compat : 1;
    uint32_t _reserved0 : 31;
} PACKED acpi_madt_t;

#define MADT_LAPIC          0x00
#define MADT_IOAPIC         0x01
#define MADT_ISO            0x02
#define MADT_NMI_SOURCE     0x03
#define MADT_LAPIC_NMI      0x04

typedef struct madt_lapic {
    uint8_t acpi_processor_uid;
    uint8_t apic_id;
    uint32_t enabled : 1;
    uint32_t online_capable : 1;
} PACKED madt_lapic_t;

typedef struct madt_ioapic {
    uint8_t ioapic_id;
    uint8_t _reserved;
    uint32_t ioapic_address;
    uint32_t gsi_base;
} PACKED madt_ioapic_t;

typedef struct madt_iso {
    uint8_t bus;
    uint8_t source;
    uint32_t gsi;
    uint16_t flags;
} PACKED madt_iso_t;

typedef struct madt_lapic_nmi {
    uint8_t acpi_processor_uid;
    uint16_t flags;
    uint8_t lint;
} PACKED madt_lapic_nmi_t;

typedef struct madt_entry {
    uint8_t type;
    uint8_t length;
    union {
        madt_lapic_t lapic;
        madt_ioapic_t ioapic;
        madt_iso_t iso;
        madt_lapic_nmi_t lapic_nmi;
    };
} PACKED madt_entry_t;

#endif //__TOMATOS_KERNEL_ACPI_TABLES_MADT_H__
