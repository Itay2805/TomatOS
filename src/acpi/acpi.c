#include "acpi.h"

#include "acpi/tables/rsdp.h"
#include "acpi/tables/rsdt.h"
#include "acpi/tables/mcfg.h"
#include "acpi/tables/madt.h"
#include "acpi/tables/fadt.h"

error_t acpi_tables_init(tboot_info_t* info) {
    error_t err = NO_ERROR;

    log_notice("Initializing ACPI tables");
    CHECK_AND_RETHROW(rsdp_init(info));
    CHECK_AND_RETHROW(rsdt_init());
    CHECK_AND_RETHROW(fadt_init());
    CHECK_AND_RETHROW(madt_init());
    CHECK_AND_RETHROW(fadt_init());

    mcfg_init();

cleanup:
    return err;
}
bool acpi_validate_checksum(const void* addr, size_t size) {
    uint8_t checksum = 0;
    const char* buf = addr;
    for(int i = 0; i < size; i++) {
        checksum += buf[i];
    }
    return checksum == 0;
}
