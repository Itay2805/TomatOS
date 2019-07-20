#include <memory/vmm.h>
#include <acpi/acpi.h>
#include "hpet.h"

hpet_table_t* hpet;

error_t hpet_table_init() {
    error_t err = NO_ERROR;

    hpet = (hpet_table_t *) rsdt_search("HPET", 0);
    CHECK_ERROR_TRACE(hpet != 0, ERROR_NOT_FOUND, "Could not find HPET acpi table!");
    log_info("\tHPET Found (0x%016p):", (uintptr_t)hpet - DIRECT_MAPPING_BASE);
    log_debug("\t\tRevision: %d", hpet->header.revision);
    log_debug("\t\tOEM ID: %.6s", hpet->header.oemid);
    log_debug("\t\tOEM TABLE ID: %.8s", hpet->header.oem_table_id);

    CHECK_TRACE(acpi_validate_checksum(hpet, hpet->header.length), "Got invalid checksum on HPET table");

cleanup:
    return err;
}
