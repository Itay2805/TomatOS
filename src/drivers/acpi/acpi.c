#include "acpi.h"

#include "rsdp.h"

error_t acpi_init() {
    error_t err = NO_ERROR;

    log_info("Initializing ACPI");
    CHECK_AND_RETHROW(rsdp_init());

cleanup:
    return err;
}
