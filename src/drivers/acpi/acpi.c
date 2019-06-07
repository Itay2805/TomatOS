#include "acpi.h"

#include "rsdp.h"
#include "rsdt.h"

error_t acpi_init() {
    error_t err = NO_ERROR;

    log_info("Initializing ACPI");
    CHECK_AND_RETHROW(rsdp_init());
    CHECK_AND_RETHROW(rsdt_init());

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
