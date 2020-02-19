#include <acpi/tables/rsdp.h>
#include <acpi/tables/rsdt.h>

#include <tboot.h>
#include <util/trace.h>
#include <acpi/tables/madt.h>

#include "acpi.h"

void acpi_tables_init(tboot_info_t* info) {
    TRACE("ACPI Table initialization");
    rsdp_init(info);
    rsdt_init();
    madt_init();
}
