#ifndef __ACPI_TABLES_TABLE_H__
#define __ACPI_TABLES_TABLE_H__

#include <stdint.h>

typedef struct acpi_header_t {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem[6];
    char oem_table[8];
    uint32_t oem_revision;
    char creator_id[4];
    uint32_t creator_revision;
}__attribute__((packed)) acpi_header_t;

// TODO: checksum calculation on all tables

void trace_table(acpi_header_t* header);

#endif //__ACPI_TABLES_TABLE_H__
