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

/**
 * Initialize the acpi timer
 */
err_t init_acpi();

#endif //__ACPI_ACPI_H__
