#ifndef TOMATKERNEL_ACPI_H
#define TOMATKERNEL_ACPI_H

#include <error.h>
#include <tboot/tboot.h>

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
 * Will search and parse all the needed ACPI tables
 */
error_t acpi_tables_init(tboot_info_t* info);

/**
 * Check if the checksum of an acpi table is correct
 */
bool acpi_validate_checksum(const void* addr, size_t size);

#endif //TOMATKERNEL_ACPI_H
