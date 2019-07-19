#include <memory/vmm.h>
#include "hpet.h"

hpet_t* hpet;

error_t hpet_search() {
    error_t err = NO_ERROR;

    hpet = (hpet_t*)rsdt_search("HPET", 0);
    if(hpet) {
        log_info("\tHPET Found (0x%016p):", (uintptr_t)hpet - DIRECT_MAPPING_BASE);
        log_debug("\t\tRevision: %d", hpet->header.revision);
        log_debug("\t\tOEM ID: %.6s", hpet->header.oemid);
        log_debug("\t\tOEM TABLE ID: %.8s", hpet->header.oem_table_id);
    }else {
        CHECK_FAIL_ERROR(ERROR_NOT_FOUND);
    }

cleanup:
    if(err != NO_ERROR) {
        if(err == ERROR_NOT_FOUND) {
            log_warn("\tCould not find HPET table");
            return NO_ERROR;
        }
    }
    return err;
}
