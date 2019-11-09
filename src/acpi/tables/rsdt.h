#ifndef TOMATKERNEL_RSDT_H
#define TOMATKERNEL_RSDT_H

#include <acpispec/tables.h>

#include <stdint.h>

typedef struct acpi_xsdt {
    acpi_header_t header;
    uint64_t tables[0];
} __attribute__((packed)) acpi_xsdt_t;

extern acpi_rsdt_t* acpi_rsdt;
extern acpi_xsdt_t* acpi_xsdt;

/**
 * Will initialize the rsdt
 */
void rsdt_init();

/**
 * Will search for an sdt inside the rsdt
 *
 * @remark
 * Will search either rsdt or xsdt as needed
 *
 * @param signature [IN] The signature of the table
 * @param index     [IN] The number of the table (for this specific signature)
 */
acpi_header_t* rsdt_search(const char* signature, int index);

#endif //TOMATKERNEL_RSDT_H
