#include <memory/vmm.h>

#include <stddef.h>
#include <string.h>

#include "rsdt.h"
#include "rsdp.h"

rsdt_t* rsdt;
xsdt_t* xsdt;

void rsdt_init() {
    sdt_hdr_t* hdr;

    if(rsdp->revision == 0) {
        rsdt = (rsdt_t *)PHYSICAL_TO_DIRECT((uintptr_t)rsdp->rsdt_addr);
        hdr = &rsdt->header;
        debug_log("[+] \tRSDT - %p", rsdp->rsdt_addr);
    }else {
        xsdt = (xsdt_t *) PHYSICAL_TO_DIRECT(rsdp2->xsdt_addr);
        hdr = &xsdt->header;

        if(rsdp->revision != 2) {
            debug_log("[!] \tUnknown ACPI revision, assuming XSDT");
        }

        debug_log("[+] \tXSDT - %p\n", rsdp2->xsdt_addr);
    }

    ASSERT(acpi_validate_checksum(hdr, hdr->length));
    ASSERT(rsdt || xsdt);
}

sdt_hdr_t* rsdt_search(char* signature, int index) {
    if(rsdt != NULL) {
        // rsdt searching
        int actual_index = 0;
        for(int i = 0; i < ((rsdt->header.length - sizeof(sdt_hdr_t)) / 4); i++) {
            sdt_hdr_t* hdr = (sdt_hdr_t *)PHYSICAL_TO_DIRECT((uintptr_t)rsdt->sdts[i]);
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
        for(int i = 0; i < ((xsdt->header.length - sizeof(sdt_hdr_t)) / 8); i++) {
            sdt_hdr_t* hdr = (sdt_hdr_t *) PHYSICAL_TO_DIRECT(xsdt->sdts[i]);
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