#ifndef TOMATKERNEL_ACPI_H
#define TOMATKERNEL_ACPI_H

#include <error.h>

/**
 * Initialize ACPI by searching for the needed tables
 * @return
 */
error_t acpi_init();

#endif //TOMATKERNEL_ACPI_H
