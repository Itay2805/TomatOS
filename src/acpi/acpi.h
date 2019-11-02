#ifndef TOMATKERNEL_ACPI_H
#define TOMATKERNEL_ACPI_H

#include <tboot.h>

/**
 * will search for all the acpi tables
 */
void acpi_tables_init(tboot_info_t* info);

#endif //TOMATKERNEL_ACPI_H
