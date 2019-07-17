#include "rsdp.h"

#include <memory/vmm.h>

#include <string.h>
#include <common.h>
#include <ctype.h>

rsdp_t* rsdp;
rsdp2_t* rsdp2;

error_t rsdp_init(tboot_info_t* info) {
    error_t err = NO_ERROR;

    // check we actually have this
    CHECK_ERROR_TRACE(info->rsdp, ERROR_NOT_FOUND, "RSDP table not found!");

    // set the table
    rsdp = (rsdp_t *) info->rsdp;
    if(rsdp->revision == 2) {
        rsdp2 = (rsdp2_t *) rsdp;
    }

    log_info("\tRSDP found (0x%016p)", (uintptr_t)rsdp - DIRECT_MAPPING_BASE);
    log_debug("\t\tRevision: %d", rsdp->revision);
    log_debug("\t\tOEM ID: %.6s", rsdp->oemid);

cleanup:
    return err;
}
