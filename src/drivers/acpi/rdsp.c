#include "rsdp.h"

#include "acpi.h"

#include <memory/vmm.h>

#include <string.h>
#include <common.h>
#include <ctype.h>

#define RSDP_SEARCH_START   PHYSICAL_ADDRESS(0x000E0000)
#define RSDP_SEARCH_END     PHYSICAL_ADDRESS(0x000FFFFF)

rsdp_t* rsdp;
rsdp2_t* rsdp2;

static bool validate(char* ptr) {
    rsdp2_t* tmprsdp = (rsdp2_t*)ptr;
    int size;

    // verify signature
    if(memcmp(tmprsdp->rsdp.signature, "RSD PTR ", 8) != 0) return false;
    if(tmprsdp->rsdp.revision == 0) {
        // ACPI 1.0
        size = sizeof(rsdp_t);
    }else {
        // ACPI 2.0+
        // unknown revision, assume
        if(tmprsdp->rsdp.revision != 2) {
            log_warn("Unknown RSDP revision, Assuming compatible (Got %d, Expected 0/2)", tmprsdp->rsdp.revision);
        }
        size = tmprsdp->length;
    }

    return acpi_validate_checksum(ptr, (size_t) size);
}

error_t rsdp_init() {
    error_t err = NO_ERROR;

    log_debug("\tSearching for RSDP");
    for(char* cur = (char *) RSDP_SEARCH_START; cur < (char*)(RSDP_SEARCH_END - sizeof(rsdp_t)); cur++) {
        // we will map if it is not mapped already
        if(!vmm_is_mapped(kernel_address_space, (uintptr_t) cur)) {
            CHECK_AND_RETHROW(vmm_map(kernel_address_space, cur, cur - PHYSICAL_BASE, 0));
        }
        if(validate(cur)) {
            rsdp = (rsdp_t *) cur;
            if(rsdp->revision == 2) {
                rsdp2 = (rsdp2_t *) rsdp;
            }
            break;
        }
    }

    CHECK_ERROR_TRACE(rsdp, ERROR_NOT_FOUND, "RSDP table not found!");

    log_info("\tRSDP found (0x%016p)", (uintptr_t)rsdp - PHYSICAL_BASE);
    log_debug("\t\tRevision: %d", rsdp->revision);
    log_debug("\t\tOEM ID: %s", rsdp->oemid);

cleanup:
    return err;
}
