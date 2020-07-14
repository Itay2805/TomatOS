#include <util/except.h>
#include <mem/vmm.h>
#include "rsdp.h"

acpi_rsdp_t* g_acpi_rsdp = NULL;
acpi_xsdp_t* g_acpi_xsdp = NULL;

void rsdp_init(uintptr_t rsdp_ptr) {
    ASSERT(rsdp_ptr != 0);

    // set the default table
    g_acpi_rsdp = (acpi_rsdp_t*)rsdp_ptr;

    // if we have version two then set the version two variable as well
    if (g_acpi_rsdp->revision >= 2) {
        g_acpi_xsdp = (acpi_xsdp_t*)g_acpi_rsdp;
    }

    if (g_acpi_xsdp) {
        TRACE("\tRSDP 0x%016lx %08x (v%02d %6.6s)", rsdp_ptr, g_acpi_xsdp->length, g_acpi_rsdp->revision, g_acpi_rsdp->oem);
    } else {
        TRACE("\tRSDP 0x%016lx -------- (v%02d %6.6s)", rsdp_ptr, g_acpi_rsdp->revision, g_acpi_rsdp->oem);
    }
}
