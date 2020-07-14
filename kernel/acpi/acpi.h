#ifndef __ACPI_ACPI_H__
#define __ACPI_ACPI_H__

#include <stdbool.h>

/**
 * Memory regions that acpi has access to
 */
typedef struct acpi_mem_region {
    uint64_t base;
    uint64_t end;
} acpi_mem_region_t;

/**
 * This is regions that acpi can access
 */
extern acpi_mem_region_t* g_acpi_regions;

/**
 * Initialize all the acpi tables
 */
void init_acpi_tables(uintptr_t rsdp_ptr);

void acpi_ustall(uint64_t nanoseconds);
void acpi_stall(uint64_t microseconds);

#endif //__ACPI_ACPI_H__
