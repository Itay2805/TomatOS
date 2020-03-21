#ifndef __ACPI_TABLES_MCFG_H__
#define __ACPI_TABLES_MCFG_H__

#include <stdint.h>
#include "table.h"

typedef struct acpi_mcfg {
    acpi_header_t header;
    uint64_t _reserved1;
    uint64_t base_address;
    uint16_t pci_segment_group_number;
    uint8_t start_bus_number;
    uint8_t end_bus_number;
    uint32_t _reserved2;
} acpi_mcfg_t;

#endif //__ACPI_TABLES_MCFG_H__
