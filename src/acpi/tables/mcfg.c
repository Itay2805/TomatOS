#include "mcfg.h"

#include <memory/vmm.h>
#include <acpi/acpi.h>

mcfg_t* mcfg = NULL;

void mcfg_init() {
    mcfg = (mcfg_t *) rsdt_search("MCFG", 0);
    if(mcfg) {
        log_info("\tMCFG Found (0x%016p):", (uintptr_t)mcfg - DIRECT_MAPPING_BASE);
        log_debug("\t\tRevision: %d", mcfg->header.revision);
        log_debug("\t\tOEM ID: %.6s", mcfg->header.oemid);
        log_debug("\t\tOEM TABLE ID: %.8s", mcfg->header.oem_table_id);
    }else {
        log_warn("\tMCFG not found");
    }
}
