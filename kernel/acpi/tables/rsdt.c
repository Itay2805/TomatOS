#include <util/string.h>
#include <util/except.h>
#include <mem/vmm.h>

#include "rsdt.h"
#include "rsdp.h"

acpi_rsdt_t* g_acpi_rsdt = NULL;
acpi_xsdt_t* g_acpi_xsdt = NULL;

void rsdt_init() {
    if (g_acpi_rsdp->revision == 0) {
        g_acpi_rsdt = (acpi_rsdt_t*)PHYSICAL_TO_DIRECT((uintptr_t)g_acpi_rsdp->rsdt);

        trace_table(&g_acpi_rsdt->header);
    } else {
        g_acpi_xsdt = (acpi_xsdt_t*) PHYSICAL_TO_DIRECT(g_acpi_xsdp->xsdt);

        WARN(g_acpi_rsdp->revision == 2, "Unknown ACPI revision, assuming XSDT");

        trace_table(&g_acpi_xsdt->header);
    }

    // make sure we got one
    ASSERT(g_acpi_rsdt || g_acpi_xsdt);

    // trace out all the tables
    if (g_acpi_rsdt != NULL) {
        for (unsigned long i = 0; i < ((g_acpi_rsdt->header.length - sizeof(acpi_header_t)) / 4); i++) {
            acpi_header_t* hdr = (acpi_header_t *)PHYSICAL_TO_DIRECT((uintptr_t)g_acpi_rsdt->tables[i]);
            trace_table(hdr);
        }
    } else {
        for (unsigned long i = 0; i < ((g_acpi_xsdt->header.length - sizeof(acpi_header_t)) / 8); i++) {
            acpi_header_t* hdr = (acpi_header_t*) PHYSICAL_TO_DIRECT(g_acpi_xsdt->tables[i]);
            trace_table(hdr);
        }
    }
}



acpi_header_t* rsdt_search(const char* signature, int index) {
    if (g_acpi_rsdt != NULL) {
        // rsdt searching
        int actual_index = 0;
        for (unsigned long i = 0; i < ((g_acpi_rsdt->header.length - sizeof(acpi_header_t)) / 4); i++) {
            acpi_header_t* hdr = (acpi_header_t *)PHYSICAL_TO_DIRECT((uintptr_t)g_acpi_rsdt->tables[i]);
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
        for (unsigned long i = 0; i < ((g_acpi_xsdt->header.length - sizeof(acpi_header_t)) / 8); i++) {
            acpi_header_t* hdr = (acpi_header_t*) PHYSICAL_TO_DIRECT(g_acpi_xsdt->tables[i]);
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
