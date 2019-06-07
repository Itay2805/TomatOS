#include "rsdp.h"

#include <memory/vmm.h>
#include <string.h>
#include <common.h>

#define RSDP_SEARCH_START   PHYSICAL_ADDRESS(0x000E0000)
#define RSDP_SEARCH_END     PHYSICAL_ADDRESS(0x000FFFFF)

rsdp_t* rsdp;
rsdp2_t* rsdp2;

static bool validate(char* ptr) {
    uint8_t checksum = 0;
    rsdp2_t* rsdp2 = (rsdp2_t*)ptr;
    int size;

    // verify signature
    if(memcmp(rsdp2->rsdp.signature, "RSD PTR ", 8) != 0) return false;
    if(rsdp2->rsdp.revision == 0) {
        // ACPI 1.0
        size = sizeof(rsdp_t);
    }else {
        // ACPI 2.0+
        // unknown revision, assume
        if(rsdp2->rsdp.revision != 2) {
            log_warn("Unknown RSDP revision, Assuming compatible (Got %d, Expected 0/2)", rsdp2->rsdp.revision);
        }
        size = rsdp2->length;
    }

    for(int  i = 0; i < size; i++) {
        checksum += ptr[i];
    }

    return checksum == 0;
}

error_t rsdp_init() {
    error_t err = NO_ERROR;

    log_info("\tSearching for RSDP");
    for(char* cur = (char *) RSDP_SEARCH_START; cur < (char*)(RSDP_SEARCH_END - sizeof(rsdp_t)); cur++) {
        if(vmm_is_mapped(kernel_address_space, (uintptr_t) cur)) {
            if(validate(cur)) {
                rsdp = (rsdp_t *) cur;
                if(rsdp->revision == 2) {
                    rsdp2 = (rsdp2_t *) rsdp;
                }
            }
        }else {
            cur = ALIGN_DOWN_PTR(cur, KB(4)) + KB(4);
        }
    }

    CHECK_ERROR_TRACE(rsdp, ERROR_NOT_FOUND, "\tRSDP table not found!");

    log_info("\tRSDP found (0x%016p):", (uintptr_t)rsdp - PHYSICAL_BASE);
    log_info("\t\tRevision: %d", rsdp->revision);
    log_info("\t\tOEM ID: %s", rsdp->oemid);

cleanup:
    return err;
}
