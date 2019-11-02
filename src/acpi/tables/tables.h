#ifndef TOMATKERNEL_TABLES_H
#define TOMATKERNEL_TABLES_H

#include <util/error.h>
#include <tboot.h>

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define ACPI_ADDRESS_SPACE_MMIO     0
#define ACPI_ADDRESS_SPACE_PORTIO   1

typedef struct acpi_generic_address {
    uint8_t address_space_id;
    uint8_t bit_width;
    uint8_t bit_offset;
    uint8_t _reserved;
    uint64_t address;
} __attribute__((packed)) acpi_generic_address_t;

/**
 * Check if the checksum of an acpi table is correct
 */
bool acpi_validate_checksum(const void* addr, size_t size);

#endif //TOMATKERNEL_TABLES_H
