#include <memory/vmm.h>

#include <stddef.h>
#include <string.h>

#include "rsdt.h"
#include "rsdp.h"

acpi_rsdt_t* acpi_rsdt;
acpi_xsdt_t* acpi_xsdt;

void rsdt_init() {
    acpi_header_t* hdr;

    if(acpi_rsdp->revision == 0) {
        acpi_rsdt = (acpi_rsdt_t*)PHYSICAL_TO_DIRECT((uintptr_t)acpi_rsdp->rsdt);
        hdr = &acpi_rsdt->header;
        debug_log("[+] \tRSDT - %p", acpi_rsdp->rsdt);
    }else {
        acpi_xsdt = (acpi_xsdt_t*) PHYSICAL_TO_DIRECT(acpi_rsdp2->xsdt);
        hdr = &acpi_xsdt->header;

        if(acpi_rsdp->revision != 2) {
            debug_log("[!] \tUnknown ACPI revision, assuming XSDT");
        }

        debug_log("[+] \tXSDT - %p\n", acpi_rsdp2->xsdt);
    }

    ASSERT(acpi_validate_checksum(hdr, hdr->length));
    ASSERT(acpi_rsdt || acpi_xsdt);
}

acpi_header_t* rsdt_search(const char* signature, int index) {
    if(acpi_rsdt != NULL) {
        // rsdt searching
        int actual_index = 0;
        for(unsigned long i = 0; i < ((acpi_rsdt->header.length - sizeof(acpi_header_t)) / 4); i++) {
            acpi_header_t* hdr = (acpi_header_t *)PHYSICAL_TO_DIRECT((uintptr_t)acpi_rsdt->tables[i]);
            if(memcmp(hdr->signature, signature, 4) == 0) {
                if(actual_index == index) {
                    return hdr;
                }else {
                    actual_index++;
                }
            }
        }
    }else {
        int actual_index = 0;
        for(unsigned long i = 0; i < ((acpi_xsdt->header.length - sizeof(acpi_header_t)) / 8); i++) {
            acpi_header_t* hdr = (acpi_header_t*) PHYSICAL_TO_DIRECT(acpi_xsdt->tables[i]);
            if(memcmp(hdr->signature, signature, 4) == 0) {
                if(actual_index == index) {
                    return hdr;
                }else {
                    actual_index++;
                }
            }
        }
    }

    // not found
    return NULL;
}