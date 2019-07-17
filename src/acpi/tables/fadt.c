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
        dsdt = (sdt_hdr_t*)(uintptr_t)CONVERT_TO_DIRECT(fadt->dsdt);
    }else if(rsdp2) {
        dsdt = (sdt_hdr_t*)CONVERT_TO_DIRECT(fadt->acpi2.x_dsdt);
    }else {
        CHECK_FAIL_ERROR_TRACE(ERROR_NOT_SUPPORTED, "Could not get either rsdp or rsdp2");
    }

    // first of all make sure the header is mapped
    if(!vmm_is_mapped(kernel_address_space, (uintptr_t) dsdt)) {
        CHECK_AND_RETHROW(vmm_map(kernel_address_space, dsdt, (void *) dsdt - DIRECT_MAPPING_BASE, PAGE_ATTR_WRITE));
    }

    // now we can map the rest (skipping the first page)
    for(uintptr_t addr = ALIGN_DOWN(dsdt, KB(4)) + KB(4); addr < ALIGN_UP((uintptr_t)dsdt + dsdt->length * KB(4), KB(4)); addr += KB(4)) {
        if(!vmm_is_mapped(kernel_address_space, addr)) {
            CHECK_AND_RETHROW(vmm_map(kernel_address_space, (void*)addr, (void *) addr - DIRECT_MAPPING_BASE, PAGE_ATTR_WRITE));
        }
    }

    CHECK_ERROR_TRACE(acpi_validate_checksum(dsdt, dsdt->length), ERROR_NOT_FOUND, "DSDT checksum incorrect");
    log_info("\tDSDT Found (0x%016p)", (uintptr_t)dsdt - DIRECT_MAPPING_BASE);
    log_debug("\t\tRevision: %d", dsdt->revision);
    log_debug("\t\tOEM ID: %6s", dsdt->oemid);
    log_debug("\t\tOEM TABLE ID: %8s", dsdt->oem_table_id);

cleanup:
    return err;
}

error_t fadt_init() {
    error_t err = NO_ERROR;

    fadt = (fadt_t *) rsdt_search("FACP", 0);
    CHECK_ERROR_TRACE(fadt, ERROR_NOT_FOUND, "FADT (FACP) Not found");
    CHECK_ERROR_TRACE(acpi_validate_checksum(fadt, fadt->header.length), ERROR_NOT_FOUND, "FADT checksum incorrect");

    log_info("\tFADT (FACP) Found (0x%016p)", (uintptr_t)fadt - DIRECT_MAPPING_BASE);
    log_debug("\t\tRevision: %d", fadt->header.revision);
    log_debug("\t\tOEM ID: %6s", fadt->header.oemid);
    log_debug("\t\tOEM TABLE ID: %8s", fadt->header.oem_table_id);

    CHECK_AND_RETHROW(dsdt_init());


cleanup:
    return err;
}