#include <acpi/tables/rsdp.h>
#include <acpi/tables/rsdt.h>
#include <acpi/tables/madt.h>
#include "acpi.h"

void acpi_tables_init(tboot_info_t* info) {
    debug_log("[*] Getting all ACPI tables\n");

    rsdp_init(info);
    rsdt_init();
    madt_init();
}
