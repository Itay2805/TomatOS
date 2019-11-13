#include <acpi/tables/rsdp.h>
#include <acpi/tables/rsdt.h>
#include <acpi/tables/madt.h>
#include <acpi/tables/fadt.h>
#include <acpi/tables/mcfg.h>

#include <lai/helpers/sci.h>
#include <lai/core.h>
#include <interrupts/interrupts.h>
#include <interrupts/apic/ioapic.h>

#include "acpi.h"

void acpi_tables_init(tboot_info_t* info) {
    debug_log("[*] Getting all ACPI tables\n");

    rsdp_init(info);
    rsdt_init();
    fadt_init();
    mcfg_init();
    madt_init();

    lai_set_acpi_revision(acpi_rsdp->revision);
}

static error_t sci_handler(interrupt_context_t* ctx, void* user_param) {
    return NO_ERROR;
}

static interrupt_handler_t handler = {
    .vector = -1,
    .callback = sci_handler,
    .name = "ACPI SCI Handler",
    .user_param = NULL,
};

void acpi_init() {
    debug_log("[*] Initializing ACPI\n");
    lai_create_namespace();
    ASSERT(!lai_enable_acpi(1));
    interrupt_register(&handler);
    ASSERT(!IS_ERROR(ioapic_redirect(acpi_fadt->sci_irq, handler.vector, 0)));
}
