#include <memory/vmm.h>
#include "fadt.h"
#include "rsdp.h"

acpi_fadt_t* acpi_fadt;
acpi_aml_t* acpi_dsdt;

static void dsdt_init() {
    if(acpi_rsdp2 != NULL) {
        acpi_dsdt = (acpi_aml_t*)PHYSICAL_TO_DIRECT(acpi_fadt->x_dsdt);
    }else if(acpi_rsdp != NULL) {
        acpi_dsdt = (acpi_aml_t*)PHYSICAL_TO_DIRECT((uintptr_t)acpi_fadt->dsdt);
    }else {
        ASSERT(false);
    }

    debug_log("[+] \tDSDT - %p\n", DIRECT_TO_PHYSICAL(acpi_dsdt));
}

void fadt_init() {
    acpi_fadt = (acpi_fadt_t*)rsdt_search("FACP", 0);
    ASSERT(acpi_fadt != NULL);
    ASSERT(acpi_validate_checksum(acpi_fadt, acpi_fadt->header.length));
    debug_log("[+] \tFADT - %p (Rev. %d.%d)\n", DIRECT_TO_PHYSICAL(acpi_fadt), acpi_fadt->header.revision, acpi_fadt->minor_version);

    dsdt_init();
}
