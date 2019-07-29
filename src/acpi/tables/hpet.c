#include <memory/vmm.h>
#include <acpi/acpi.h>
#include "hpet.h"

hpet_table_t* hpet_table;

error_t hpet_table_init() {
    error_t err = NO_ERROR;

    hpet_table = (hpet_table_t *) rsdt_search("HPET", 0);
    CHECK_ERROR_TRACE(hpet_table != 0, ERROR_NOT_FOUND, "Could not find HPET acpi table!");
    log_info("\tHPET Found (0x%016p):", (uintptr_t)hpet_table - DIRECT_MAPPING_BASE);
    log_info("\t\tRevision: %d", hpet_table->header.revision);
    log_info("\t\tOEM ID: %.6s", hpet_table->header.oemid);
    log_info("\t\tOEM TABLE ID: %.8s", hpet_table->header.oem_table_id);

    CHECK_TRACE(acpi_validate_checksum(hpet_table, hpet_table->header.length), "Got invalid checksum on HPET table");

    cleanup:
    return err;
}
