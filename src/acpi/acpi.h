#ifndef TOMATKERNEL_ACPI_H
#define TOMATKERNEL_ACPI_H

#include <error.h>
#include <tboot/tboot.h>

/**
 * Will search and parse all the needed ACPI tables
 */
error_t acpi_tables_init(tboot_info_t* info);

/**
 * Check if the checksum of an acpi table is correct
 */
bool acpi_validate_checksum(const void* addr, size_t size);

#endif //TOMATKERNEL_ACPI_H
