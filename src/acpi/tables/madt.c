#include <stddef.h>
#include <util/debug.h>
#include <memory/vmm.h>
#include "madt.h"

acpi_madt_t* acpi_madt = NULL;

void madt_init() {
    acpi_madt = (acpi_madt_t*) rsdt_search("APIC", 0);
    ASSERT(acpi_madt != NULL);
    debug_log("[+] \tMADT - %p, entries:\n", DIRECT_TO_PHYSICAL(acpi_madt));

    madt_entry_t* cur = &acpi_madt->entries[0];
    for(uintptr_t addr = (uintptr_t) acpi_madt->entries; addr < (uintptr_t)acpi_madt->entries + (acpi_madt->header.length - sizeof(acpi_madt_t)); addr += cur->length, cur = (madt_entry_t *) addr) {
        switch(cur->type) {
            case MADT_TYPE_LAPIC: {
                madt_lapic_t* lapic = &cur->lapic;
                debug_log("[*] \t\tLAPIC: #%d, PID: #%d (%s)\n", lapic->id, lapic->processor_id, lapic->processor_enabled ? "enabled" : "disabled");
            } break;
            case MADT_TYPE_IOAPIC: {
                madt_ioapic_t* ioapic = &cur->ioapic;
                debug_log("[*] \t\tI/O APIC #%d (0x%016p)\n", ioapic->id, ioapic->mmio_base);
            } break;
            case MADT_TYPE_ISO: {
                madt_iso_t* iso = &cur->iso;
                debug_log("[*] \t\tISO %d->%d:%d\n", iso->gsi, iso->bus_source, iso->irq_source);
            } break;
            case MADT_TYPE_NMI: {
                madt_nmi_t* nmi = &cur->nmi;
                debug_log("[*] \t\tNMI on ");
                if(nmi->processor_id == 0xFF) {
                    debug_log("all processors");
                }else {
                    debug_log("processor #%d", nmi->processor_id);
                }
                debug_log(", LINT: %d\n", nmi->lint);
            } break;
            default:break;
        }
    }
}

void* madt_get_next(int type, int index) {
    int seen = 0;

    madt_entry_t* cur = &acpi_madt->entries[0];
    for(uintptr_t addr = (uintptr_t) acpi_madt->entries; addr < (uintptr_t)acpi_madt->entries + (acpi_madt->header.length - sizeof(acpi_madt_t)); addr += cur->length, cur = (madt_entry_t *) addr) {
        if(cur->type == type) {
            if(seen == index) {
                return (void *) ((uintptr_t)cur + offsetof(madt_entry_t, lapic));
            }
            seen++;
        }
    }

    return NULL;
}
