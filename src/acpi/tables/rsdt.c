#include "rsdt.h"
#include "rsdp.h"
#include "acpi/acpi.h"

#include <logger/logger.h>
#include <memory/vmm.h>

#include <stddef.h>
#include <string.h>

// TODO: Instead of having both have a single uniform list
rsdt_t* rsdt;
xsdt_t* xsdt;

error_t rsdt_init() {
    error_t err = NO_ERROR;
    sdt_hdr_t* hdr;

    if(rsdp->revision == 0) {
        rsdt = (rsdt_t *)CONVERT_TO_DIRECT((uintptr_t)rsdp->rsdt_addr);
        hdr = &rsdt->header;
        if(!vmm_is_mapped(kernel_address_space, (uintptr_t) rsdt)) {
            CHECK_AND_RETHROW(vmm_map(kernel_address_space, rsdt, (void*)(uintptr_t)rsdp->rsdt_addr, 0));
        }

        log_info("\tRSDT found (0x%016p)", (uintptr_t)rsdt - DIRECT_MAPPING_BASE);
    }else {
        xsdt = (xsdt_t *) CONVERT_TO_DIRECT(rsdp2->xsdt_addr);
        hdr = &xsdt->header;
        if(!vmm_is_mapped(kernel_address_space, (uintptr_t) xsdt)) {
            CHECK_AND_RETHROW(vmm_map(kernel_address_space, xsdt, (void*)(uintptr_t)rsdp2->xsdt_addr, 0));
        }


        if(rsdp->revision != 2) {
            log_warn("\tUnknown ACPI revision, assuming XSDT");
        }

        log_info("\tXSDT found (0x%016p)", (uintptr_t)xsdt - DIRECT_MAPPING_BASE);
    }

    CHECK_TRACE(acpi_validate_checksum(hdr, hdr->length), "Invalid SDT checksum!");
    log_info("\t\tRevision: %d", hdr->revision);
    log_info("\t\tOEM ID: %6.s", hdr->oemid);
    log_info("\t\tOEM TABLE ID: %.8s", hdr->oem_table_id);


    // just for debugging
    log_info("\t\tentries:");
    if(rsdt != NULL) {
        // rsdt iteration
        for(uint32_t* sdt = rsdt->sdts; sdt < (rsdt->sdts + ((rsdt->header.length - sizeof(sdt_hdr_t)) / 4)); sdt++) {
            hdr = (sdt_hdr_t *)CONVERT_TO_DIRECT((uintptr_t)*sdt);
            if(!vmm_is_mapped(kernel_address_space, (uintptr_t) hdr)) {
                CHECK_AND_RETHROW(vmm_map(kernel_address_space, hdr, (void*)(uintptr_t)*sdt, 0));
            }
            log_info("\t\t\t%.4s", hdr->signature);
        }
    }else {
        // xsdt iteration
        for(uint64_t* sdt = xsdt->sdts; sdt < (xsdt->sdts + ((xsdt->header.length - sizeof(sdt_hdr_t)) / 8)); sdt++) {
            hdr = (sdt_hdr_t *) CONVERT_TO_DIRECT(*sdt);
            if(!vmm_is_mapped(kernel_address_space, (uintptr_t) hdr)) {
                CHECK_AND_RETHROW(vmm_map(kernel_address_space, hdr, (void*)(uintptr_t)*sdt, 0));
            }
            log_info("\t\t\t%.4s", hdr->signature);
        }
    }


    CHECK_ERROR_TRACE(rsdt || xsdt, ERROR_NOT_FOUND, "Could not find xsdt/rsdt!");
cleanup:
    return err;
}

sdt_hdr_t* xsdt_search(char* signature, int index) {
    int actual_index = 0;
    for(int i = 0; i < ((xsdt->header.length - sizeof(sdt_hdr_t)) / 8); i++) {
        sdt_hdr_t* hdr = (sdt_hdr_t *) CONVERT_TO_DIRECT(xsdt->sdts[i]);
        if(memcmp(hdr->signature, signature, 4) == 0) {
            if(actual_index == index) {
                return hdr;
            }else {
                actual_index++;
            }
        }
    }

    return NULL;
}

sdt_hdr_t* rsdt_search(char* signature, int index) {
    if(rsdt != NULL) {
        // rsdt searching
        int actual_index = 0;
        for(int i = 0; i < ((rsdt->header.length - sizeof(sdt_hdr_t)) / 4); i++) {
            sdt_hdr_t* hdr = (sdt_hdr_t *)CONVERT_TO_DIRECT((uintptr_t)rsdt->sdts[i]);
            if(memcmp(hdr->signature, signature, 4) == 0) {
                if(actual_index == index) {
                    return hdr;
                }else {
                    actual_index++;
                }
            }
        }
    }else {
        // xsdt searching
        return xsdt_search(signature, index);
    }

    // not found
    return NULL;
}
