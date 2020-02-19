#include <util/except.h>
#include <mm/vmm.h>

#include <string.h>

#include "rsdt.h"
#include "rsdp.h"

acpi_rsdt_t* acpi_rsdt = NULL;
acpi_xsdt_t* acpi_xsdt = NULL;

void rsdt_init() {
    acpi_header_t* hdr;

    if (acpi_rsdp->revision == 0) {
        acpi_rsdt = (acpi_rsdt_t*)PHYSICAL_TO_DIRECT((uintptr_t)acpi_rsdp->rsdt);
        hdr = &acpi_rsdt->header;

        trace_table(&acpi_rsdt->header);
    } else {
        acpi_xsdt = (acpi_xsdt_t*) PHYSICAL_TO_DIRECT(acpi_xsdp->xsdt);
        hdr = &acpi_xsdt->header;

        WARN(acpi_rsdp->revision == 2, "Unknown ACPI revision, assuming XSDT");

        trace_table(&acpi_xsdt->header);
    }

    // make sure we got one
    ASSERT(acpi_rsdt || acpi_xsdt);
}



acpi_header_t* rsdt_search(const char* signature, int index) {
    if (acpi_rsdt != NULL) {
        // rsdt searching
        int actual_index = 0;
        for (unsigned long i = 0; i < ((acpi_rsdt->header.length - sizeof(acpi_header_t)) / 4); i++) {
            acpi_header_t* hdr = (acpi_header_t *)PHYSICAL_TO_DIRECT((uintptr_t)acpi_rsdt->tables[i]);
            if (memcmp(hdr->signature, signature, 4) == 0) {
                if (actual_index == index) {
                    return hdr;
                } else {
                    actual_index++;
                }
            }
        }
    } else {
        int actual_index = 0;
        for (unsigned long i = 0; i < ((acpi_xsdt->header.length - sizeof(acpi_header_t)) / 8); i++) {
            acpi_header_t* hdr = (acpi_header_t*) PHYSICAL_TO_DIRECT(acpi_xsdt->tables[i]);
            if (memcmp(hdr->signature, signature, 4) == 0) {
                if (actual_index == index) {
                    return hdr;
                } else {
                    actual_index++;
                }
            }
        }
    }

    // not found
    return NULL;
}
