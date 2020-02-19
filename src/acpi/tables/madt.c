#include <util/except.h>
#include <intr/apic/ioapic.h>
#include "madt.h"
#include "rsdt.h"

acpi_madt_t* acpi_madt = NULL;

void madt_init() {
    acpi_madt = (acpi_madt_t*)rsdt_search("APIC", 0);
    ASSERT(acpi_madt != NULL);
    trace_table(&acpi_madt->header);

    madt_entry_t* cur = &acpi_madt->entries[0];
    for(uintptr_t addr = (uintptr_t) acpi_madt->entries; addr < (uintptr_t)acpi_madt->entries + (acpi_madt->header.length - sizeof(acpi_madt_t)); addr += cur->length, cur = (madt_entry_t *) addr) {
        switch(cur->type) {
            case MADT_TYPE_LAPIC: {
                madt_lapic_t* lapic = &cur->lapic;
                TRACE("\t\tLAPIC: #%d, PID: #%d (%s)", lapic->id, lapic->processor_id, lapic->processor_enabled ? "enabled" : "disabled");
            } break;
            case MADT_TYPE_IOAPIC: {
                madt_ioapic_t* ioapic = &cur->ioapic;
                TRACE("\t\tI/O APIC #%d (0x%016x)", ioapic->id, ioapic->mmio_base);
                ioapic_add(ioapic->mmio_base, ioapic->gsi_base);
            } break;
            case MADT_TYPE_ISO: {
                madt_iso_t* iso = &cur->iso;
                TRACE("\t\tISO %d:%d->%d", iso->bus_source, iso->irq_source, iso->gsi);
                ioapic_add_iso(iso);
            } break;
            case MADT_TYPE_NMI: {
                madt_nmi_t* nmi = &cur->nmi;
                trace("[*] \t\tNMI on ");
                if(nmi->processor_id == 0xFF) {
                    trace("all processors");
                }else {
                    trace("processor #%d", nmi->processor_id);
                }
                trace(", LINT: %d\n", nmi->lint);
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
