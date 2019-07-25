#include "mcfg.h"

#include <memory/vmm.h>
#include <acpi/acpi.h>

mcfg_t* mcfg = NULL;

error_t mcfg_init() {
    error_t err = NO_ERROR;

    mcfg = (mcfg_t *) rsdt_search("MCFG", 0);
    CHECK_ERROR_TRACE(mcfg != NULL, ERROR_NOT_FOUND, "MCFG not found");

    log_info("\tMCFG Found (0x%016p):", (uintptr_t)mcfg - DIRECT_MAPPING_BASE);
    log_info("\t\tRevision: %d", mcfg->header.revision);
    log_info("\t\tOEM ID: %.6s", mcfg->header.oemid);
    log_info("\t\tOEM TABLE ID: %.8s", mcfg->header.oem_table_id);

cleanup:
    return err;
}
