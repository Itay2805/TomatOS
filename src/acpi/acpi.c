#include <acpi/tables/rsdp.h>
#include <acpi/tables/rsdt.h>

#include <stivale.h>
#include <util/trace.h>
#include <acpi/tables/madt.h>

#include "acpi.h"

void acpi_tables_init(uintptr_t rsdp_ptr) {
    TRACE("ACPI Table initialization");
    rsdp_init(rsdp_ptr);
    rsdt_init();
    madt_init();
}
