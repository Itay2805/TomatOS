#include "madt.h"

#include <drivers/acpi/acpi.h>
#include <memory/vmm.h>
#include <buf.h>

madt_t* madt;
madt_lapic_t** madt_lapics;
madt_ioapic_t** madt_ioapics;
madt_iso_t** madt_isos;
madt_nmi_t** madt_nmis;

error_t madt_init() {
    error_t err = NO_ERROR;

    madt = (madt_t*) rsdt_search("APIC", 0);
    CHECK_ERROR_TRACE(madt, ERROR_NOT_FOUND, "MADT (APIC) Not found");
    CHECK_ERROR_TRACE(acpi_validate_checksum(madt, madt->header.length), ERROR_NOT_FOUND, "MADT (APIC) checksum incorrect");

    log_info("\tMADT (APIC) Found (0x%016p)", (uintptr_t)madt - PHYSICAL_BASE);
    log_debug("\t\tRevision: %d", madt->header.revision);
    log_debug("\t\tOEM ID: %c%c%c%c%c%c"
            , madt->header.oemid[0]
            , madt->header.oemid[1]
            , madt->header.oemid[2]
            , madt->header.oemid[3]
            , madt->header.oemid[4]
            , madt->header.oemid[5]);
    log_debug("\t\tOEM TABLE ID: %c%c%c%c%c%c%c%c"
            , madt->header.oem_table_id[0]
            , madt->header.oem_table_id[1]
            , madt->header.oem_table_id[2]
            , madt->header.oem_table_id[3]
            , madt->header.oem_table_id[4]
            , madt->header.oem_table_id[5]
            , madt->header.oem_table_id[6]
            , madt->header.oem_table_id[7]);

    log_debug("\t\tentries:");
    madt_entry_t* cur = &madt->entries[0];
    for(uintptr_t addr = (uintptr_t) madt->entries; addr < (uintptr_t)madt->entries + (madt->header.length - sizeof(madt_t)); addr += cur->length, cur = (madt_entry_t *) addr) {
        switch(cur->type) {
            case MADT_TYPE_LAPIC: {
                madt_lapic_t* lapic = &cur->lapic;
                log_debug("\t\t\tLocal APIC #%d (0x%016p)", lapic->id, (uintptr_t)lapic - PHYSICAL_BASE);
                log_debug("\t\t\t\tProcessor ID: %d (%s)", lapic->processor_id, lapic->processor_enabled ? "enabled" : "disabled");
                buf_push(madt_lapics, lapic);
            } break;
            case MADT_TYPE_IOAPIC: {
                madt_ioapic_t* ioapic = &cur->ioapic;
                log_debug("\t\t\tI/O APIC #%d (0x%016p)", ioapic->id, (uintptr_t)ioapic - PHYSICAL_BASE);
                log_debug("\t\t\t\tMMIO: 0x%016x", ioapic->mmio_base);
                log_debug("\t\t\t\tGSI base: 0x%08x", ioapic->gsi_base);
                buf_push(madt_ioapics, ioapic);
            } break;
            case MADT_TYPE_ISO: {
                madt_iso_t* iso = &cur->iso;
                log_debug("\t\t\tISO #%d (0x%016p) %d->%d:%d", buf_len(madt_isos), (uintptr_t)iso - PHYSICAL_BASE, iso->gsi, iso->bus_source, iso->irq_source);
                // TODO: Flags
                buf_push(madt_isos, iso);
            } break;
            case MADT_TYPE_NMI: {
                madt_nmi_t* nmi = &cur->nmi;
                log_debug("\t\t\tNMI #%d (0x%016p)", buf_len(madt_nmis), (uintptr_t)nmi - PHYSICAL_BASE);
                if(nmi->processor_id == 0xFF) {
                    log_debug("\t\t\t\tProcessor: all");
                }else {
                    log_debug("\t\t\t\tProcessor: %d", nmi->processor_id);
                }
                log_debug("\t\t\t\tLINT: %d", nmi->lint);
                // TODO: Flags
                buf_push(madt_nmis, nmi);
            } break;
            default:break;
        }
    }

cleanup:
    return err;
}
