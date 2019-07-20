#ifndef TOMATKERNEL_ACPI_TABLE_HPET_H
#define TOMATKERNEL_ACPI_TABLE_HPET_H

#include "rsdt.h"
#include <acpi/acpi.h>

typedef struct hpet_table {
    sdt_hdr_t header;

    uint8_t hardware_id;
    uint8_t timer_count : 5;
    uint8_t counter_size : 1;
    uint8_t _reserved : 1;
    uint8_t legacy_replacement_capable : 1;
    uint16_t pci_vendor;

    acpi_generic_address_t addr;
    uint8_t hpet_id;
    uint16_t clock_tick;
    uint8_t page_protection;
} __attribute__((packed)) hpet_table_t;

extern hpet_table_t* hpet_table;

/**
 * Will find and map the hpet table
 */
error_t hpet_table_init();

#endif //TOMATKERNEL_ACPI_TABLE_HPET_H
