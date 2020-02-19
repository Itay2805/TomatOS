#ifndef __ACPI_TABLES_RSDT_H__
#define __ACPI_TABLES_RSDT_H__

#include "table.h"

typedef struct acpi_rsdt_t
{
    acpi_header_t header;
    uint32_t tables[];
}__attribute__((packed)) acpi_rsdt_t;

typedef struct acpi_xsdt_t
{
    acpi_header_t header;
    uint64_t tables[];
}__attribute__((packed)) acpi_xsdt_t;

extern acpi_rsdt_t* acpi_rsdt;
extern acpi_xsdt_t* acpi_xsdt;

void rsdt_init();

acpi_header_t* rsdt_search(const char* signature, int index);

#endif //__ACPI_TABLES_RSDT_H__
