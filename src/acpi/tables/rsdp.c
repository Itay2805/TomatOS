#include <memory/vmm.h>
#include "rsdp.h"

acpi_rsdp1_t* acpi_rsdp;
acpi_rsdp2_t* acpi_rsdp2;

void rsdp_init(tboot_info_t* info) {
    ASSERT(info->rsdp != NULL);

    acpi_rsdp = (acpi_rsdp1_t*)PHYSICAL_TO_DIRECT(info->rsdp);
    if(acpi_rsdp->revision >= 2) {
        acpi_rsdp2 = (acpi_rsdp2_t*)acpi_rsdp;
    }

    debug_log("[+] \tRSDP - %p (Rev. %d)\n", info->rsdp, acpi_rsdp->revision);
}
