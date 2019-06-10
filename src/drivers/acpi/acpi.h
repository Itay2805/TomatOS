#ifndef TOMATKERNEL_ACPI_H
#define TOMATKERNEL_ACPI_H

#include <error.h>

/**
 * Initialize ACPI by searching for the needed tables
 * @return
 */
error_t acpi_init();

/**
 * Initialize the SCI handling
 *
 * @remark
 * Must be done after the I/O APIC is initialized
 */
error_t acpi_sci_init();

/**
 * Check if the checksum of an acpi table is correct
 */
bool acpi_validate_checksum(const void* addr, size_t size);

#endif //TOMATKERNEL_ACPI_H
