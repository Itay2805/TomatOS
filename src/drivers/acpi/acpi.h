#ifndef TOMATKERNEL_ACPI_H
#define TOMATKERNEL_ACPI_H

#include <error.h>

/**
 * Will search and parse all the needed ACPI tables
 */
error_t acpi_tables_init();

/**
 * This will initialize everything related to the ACPI namespace
 *
 * @remark
 * Must be ran after the acpi tables have been found
 *
 * @remark
 * Must be ran after the I/O APIC has been initialized
 */
error_t acpi_init();

/**
 * Check if the checksum of an acpi table is correct
 */
bool acpi_validate_checksum(const void* addr, size_t size);

#endif //TOMATKERNEL_ACPI_H
