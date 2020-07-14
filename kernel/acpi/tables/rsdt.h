#ifndef __ACPI_TABLES_RSDT_H__
#define __ACPI_TABLES_RSDT_H__

#include "table.h"

#include <lai/core.h>

extern acpi_rsdt_t* g_acpi_rsdt;
extern acpi_xsdt_t* g_acpi_xsdt;

void rsdt_init();

acpi_header_t* rsdt_search(const char* signature, int index);

#endif //__ACPI_TABLES_RSDT_H__
