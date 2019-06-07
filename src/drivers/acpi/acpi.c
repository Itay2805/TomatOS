#include <memory/vmm.h>
#include "acpi.h"

#include "rsdp.h"
#include "rsdt.h"
#include "mcfg.h"

error_t acpi_init() {
    error_t err = NO_ERROR;

    log_info("Initializing ACPI");
    CHECK_AND_RETHROW(rsdp_init());
    CHECK_AND_RETHROW(rsdt_init());

    mcfg_init();

    // just for debugging
    if(rsdt != NULL) {
        log_debug("\titerating RSDT:");

        // rsdt searching
        for(uint32_t* sdt = rsdt->sdts; sdt < (rsdt->sdts + ((rsdt->header.length - sizeof(sdt_hdr_t)) / 4)); sdt++) {
            sdt_hdr_t* hdr = (sdt_hdr_t *) PHYSICAL_ADDRESS(*sdt);
            log_debug("\t\t%c%c%c%c"
            , hdr->signature[0]
            , hdr->signature[1]
            , hdr->signature[2]
            , hdr->signature[3]);
        }
    }else {
        log_debug("\titerating XSDT:");
        for(uint64_t* sdt = xsdt->sdts; sdt < (xsdt->sdts + ((xsdt->header.length - sizeof(sdt_hdr_t)) / 8)); sdt++) {
            sdt_hdr_t* hdr = (sdt_hdr_t *) PHYSICAL_ADDRESS(*sdt);
            log_debug("\t\t%c%c%c%c"
            , hdr->signature[0]
            , hdr->signature[1]
            , hdr->signature[2]
            , hdr->signature[3]);
        }
    }

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
