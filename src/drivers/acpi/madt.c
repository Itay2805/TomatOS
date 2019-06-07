#include "madt.h"

#include <memory/vmm.h>

madt_t* madt;

error_t madt_init() {
    error_t err = NO_ERROR;

    madt = (madt_t*) rsdt_search("APIC");
    CHECK_ERROR_TRACE(madt, ERROR_NOT_FOUND, "MADT Not found");

    log_info("\tMADT Found (0x%016p):", (uintptr_t)madt - PHYSICAL_BASE);
    log_info("\t\tRevision: %d", madt->header.revision);
    log_info("\t\tOEM ID: %c%c%c%c%c%c"
            , madt->header.oemid[0]
            , madt->header.oemid[1]
            , madt->header.oemid[2]
            , madt->header.oemid[3]
            , madt->header.oemid[4]
            , madt->header.oemid[5]);
    log_info("\t\tOEM TABLE ID: %c%c%c%c%c%c%c%c"
            , madt->header.oem_table_id[0]
            , madt->header.oem_table_id[1]
            , madt->header.oem_table_id[2]
            , madt->header.oem_table_id[3]
            , madt->header.oem_table_id[4]
            , madt->header.oem_table_id[5]
            , madt->header.oem_table_id[6]
            , madt->header.oem_table_id[7]);

cleanup:
    return err;
}
