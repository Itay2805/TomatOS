#include <util/except.h>
#include <mm/vmm.h>
#include "rsdp.h"

acpi_rsdp_t* acpi_rsdp = NULL;
acpi_xsdp_t* acpi_xsdp = NULL;

void rsdp_init(uintptr_t rsdp_ptr) {
    ASSERT(rsdp_ptr != 0);

    // set the default table
    acpi_rsdp = (acpi_rsdp_t*)PHYSICAL_TO_DIRECT(rsdp_ptr);

    // if we have version two then set the version two variable as well
    if (acpi_rsdp->revision >= 2) {
        acpi_xsdp = (acpi_xsdp_t*)acpi_rsdp;
    }

    if (acpi_xsdp) {
        TRACE("\tRSDP 0x%016lx %08x (v%02d %6.6s)", rsdp_ptr, acpi_xsdp->length, acpi_rsdp->revision, acpi_rsdp->oem);
    } else {
        TRACE("\tRSDP 0x%016lx -------- (v%02d %6.6s)", rsdp_ptr, acpi_rsdp->revision, acpi_rsdp->oem);
    }
}
