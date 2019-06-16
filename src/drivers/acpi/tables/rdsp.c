#include "rsdp.h"

#include <memory/vmm.h>

#include <string.h>
#include <common.h>
#include <ctype.h>

rsdp_t* rsdp;
rsdp2_t* rsdp2;

error_t rsdp_init(boot_info_t* info) {
    error_t err = NO_ERROR;

    // check we actually have this
    CHECK_ERROR_TRACE(info->rsdp_ptr, ERROR_NOT_FOUND, "RSDP table not found!");

    // set the table
    rsdp = (rsdp_t *) info->rsdp_ptr;
    if(rsdp->revision == 2) {
        rsdp2 = (rsdp2_t *) rsdp;
    }

    log_info("\tRSDP found (0x%016p)", (uintptr_t)rsdp - PHYSICAL_BASE);
    log_debug("\t\tRevision: %d", rsdp->revision);
    log_debug("\t\tOEM ID: %c%c%c%c%c%c", rsdp->oemid[0], rsdp->oemid[1], rsdp->oemid[2], rsdp->oemid[3], rsdp->oemid[4], rsdp->oemid[5]);

cleanup:
    return err;
}
