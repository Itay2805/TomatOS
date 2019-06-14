#include "mcfg.h"

#include <memory/vmm.h>

mcfg_t* mcfg = NULL;

void mcfg_init() {
    mcfg = (mcfg_t *) rsdt_search("MCFG", 0);
    if(mcfg) {
        log_info("\tMCFG Found (0x%016p):", (uintptr_t)mcfg - PHYSICAL_BASE);
        log_debug("\t\tRevision: %d", mcfg->header.revision);
        log_debug("\t\tOEM ID: %c%c%c%c%c%c"
                , mcfg->header.oemid[0]
                , mcfg->header.oemid[1]
                , mcfg->header.oemid[2]
                , mcfg->header.oemid[3]
                , mcfg->header.oemid[4]
                , mcfg->header.oemid[5]);
        log_debug("\t\tOEM TABLE ID: %c%c%c%c%c%c%c%c"
                , mcfg->header.oem_table_id[0]
                , mcfg->header.oem_table_id[1]
                , mcfg->header.oem_table_id[2]
                , mcfg->header.oem_table_id[3]
                , mcfg->header.oem_table_id[4]
                , mcfg->header.oem_table_id[5]
                , mcfg->header.oem_table_id[6]
                , mcfg->header.oem_table_id[7]);
    }else {
        log_warn("\tMCFG not found");
    }
}
