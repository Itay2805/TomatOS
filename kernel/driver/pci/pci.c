#include <lai/host.h>
#include <mem/vmm.h>
#include "pci.h"
#include "pci_spec.h"

static acpi_mcfg_entry_t* g_mcfg_entries = NULL;
static size_t g_mcfg_entry_count = 0;

err_t init_pci() {
    err_t err = NO_ERROR;

    acpi_mcfg_t* mcfg = laihost_scan("MCFG", 0);
    CHECK_ERROR_TRACE(mcfg != NULL, ERROR_NOT_FOUND, "We only support memory mapped pci!");
    g_mcfg_entry_count = (mcfg->header.length - offsetof(acpi_mcfg_t, entries)) / sizeof(acpi_mcfg_entry_t);
    g_mcfg_entries = mcfg->entries;

cleanup:
    return err;
}

err_t get_config_for_pci(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, void** config) {
    err_t err = NO_ERROR;

    CHECK(config != 0);
    CHECK_ERROR(g_mcfg_entries != NULL, ERROR_NOT_READY);

    for (int i = 0; i < g_mcfg_entry_count; i++) {
        if (g_mcfg_entries[i].pci_segment == seg && g_mcfg_entries[i].start_bus <= bus && bus <= g_mcfg_entries[i].end_bus) {
            *config = PHYSICAL_TO_DIRECT(g_mcfg_entries[i].base_address + ((bus - g_mcfg_entries[i].start_bus) << 20 | slot << 15 | fun << 12));
            goto cleanup;
        }
    }

    CHECK_FAIL_ERROR(ERROR_NOT_FOUND);

cleanup:
    return err;
}
