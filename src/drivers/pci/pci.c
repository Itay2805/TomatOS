#include <acpi/tables/mcfg.h>
#include <memory/vmm.h>
#include <processes/process.h>
#include "pci.h"


void* pci_get_config_space(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t function) {
    // iterate the mcfg
    for(size_t i = 0; i < ((acpi_mcfg->header.length - sizeof(acpi_mcfg_t)) / sizeof(mcfg_entry_t)); i++) {
        mcfg_entry_t* entry = &acpi_mcfg->entries[i];

        // check if entry is in the range
        if(entry->segment == seg && entry->start_pci_bus <= bus && entry->end_pci_bus >= bus) {
            // get the base address
            uintptr_t phys = entry->base + (((bus - entry->start_pci_bus) << 20u) | (slot << 15u) | function << 12u);

            // map it if not mapped
            if(!vmm_is_mapped(&kernel_process->vmm_handle, PHYSICAL_TO_DIRECT(phys), PAGE_SIZE)) {
                vmm_map(&kernel_process->vmm_handle, phys, PHYSICAL_TO_DIRECT(phys), PAGE_SIZE, PAGE_SUPERVISOR_READWRITE, DEFAULT_CACHE);
            }

            // return the buffer
            return PHYSICAL_TO_DIRECT((void*)phys);
        }
    }

    // return null if not found
    return NULL;
}
