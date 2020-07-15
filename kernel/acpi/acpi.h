#ifndef __ACPI_ACPI_H__
#define __ACPI_ACPI_H__

#include <stdbool.h>

/**
 * Memory regions that acpi has access to
 */
typedef struct memmap_entry {
    uint64_t base;
    uint64_t end;
    bool acpi_access;
} memmap_entry_t;

/**
 * This is regions that acpi can access
 */
extern memmap_entry_t* g_memory_map;

/**
 * Initialize all the acpi tables
 */
void init_acpi_tables(uintptr_t rsdp_ptr);

void acpi_ustall(uint64_t nanoseconds);
void acpi_stall(uint64_t microseconds);

#endif //__ACPI_ACPI_H__
