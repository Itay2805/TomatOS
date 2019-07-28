#include "fadt.h"
#include "acpi/acpi.h"
#include "rsdp.h"

#include <memory/vmm.h>
#include <common.h>

fadt_t* fadt;
sdt_hdr_t* dsdt;

static error_t dsdt_init() {
    error_t err = NO_ERROR;

    // get the correct dsdt
    if(rsdp) {
        dsdt = (sdt_hdr_t*)CONVERT_TO_DIRECT((uintptr_t)fadt->dsdt);
    }else if(rsdp2) {
        dsdt = (sdt_hdr_t*)CONVERT_TO_DIRECT(fadt->acpi2.x_dsdt);
    }else {
        CHECK_FAIL_ERROR_TRACE(ERROR_NOT_SUPPORTED, "Could not get either rsdp or rsdp2");
    }

    // map it
    CHECK_AND_RETHROW(vmm_map_direct((uintptr_t)dsdt, KB(4)));
    CHECK_AND_RETHROW(vmm_map_direct((uintptr_t)dsdt, dsdt->length));

    CHECK_ERROR_TRACE(acpi_validate_checksum(dsdt, dsdt->length), ERROR_NOT_FOUND, "DSDT checksum incorrect");
    log_info("\tDSDT Found (0x%016p)", (uintptr_t)dsdt - DIRECT_MAPPING_BASE);
    log_info("\t\tRevision: %d", dsdt->revision);
    log_info("\t\tOEM ID: %.6s", dsdt->oemid);
    log_info("\t\tOEM TABLE ID: %.8s", dsdt->oem_table_id);

cleanup:
    return err;
}

error_t fadt_init() {
    error_t err = NO_ERROR;

    fadt = (fadt_t *) rsdt_search("FACP", 0);
    CHECK_ERROR_TRACE(fadt, ERROR_NOT_FOUND, "FADT (FACP) Not found");

    CHECK_ERROR_TRACE(acpi_validate_checksum(fadt, fadt->header.length), ERROR_NOT_FOUND, "FADT checksum incorrect");
    log_info("\tFADT (FACP) Found (0x%016p)", (uintptr_t)fadt - DIRECT_MAPPING_BASE);
    log_info("\t\tRevision: %d", fadt->header.revision);
    log_info("\t\tOEM ID: %.6s", fadt->header.oemid);
    log_info("\t\tOEM TABLE ID: %.8s", fadt->header.oem_table_id);



    CHECK_AND_RETHROW(dsdt_init());


cleanup:
    return err;
}
