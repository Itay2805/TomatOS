#ifndef __ACPI_TABLES_RSDP_H__
#define __ACPI_TABLES_RSDP_H__

#include <stdint.h>
#include <util/except.h>
#include <mem/vmm.h>

#include <lai/core.h>

extern acpi_rsdp_t* g_acpi_rsdp;
extern acpi_xsdp_t* g_acpi_xsdp;

void rsdp_init(uintptr_t rsdp_ptr);

#endif //__ACPI_TABLES_RSDP_H__
