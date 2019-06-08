#include "madt.h"

#include <memory/vmm.h>

#include <buf.h>

madt_t* madt;
madt_lapic_t** madt_lapics;
madt_ioapic_t** madt_ioapics;
madt_iso_t** madt_isos;
madt_nmi_t** madt_nmis;

error_t madt_init() {
    error_t err = NO_ERROR;

    madt = (madt_t*) rsdt_search("APIC");
    CHECK_ERROR_TRACE(madt, ERROR_NOT_FOUND, "MADT Not found");

    log_info("\tMADT Found (0x%016p)", (uintptr_t)madt - PHYSICAL_BASE);
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
            case MADT_LAPIC: {
                madt_lapic_t* lapic = &cur->lapic;
                log_debug("\t\t\tLocal APIC #%d (0x%016p)", buf_len(madt_lapics), lapic);
                buf_push(madt_lapics, lapic);
            } break;
            case MADT_IOAPIC: {
                madt_ioapic_t* ioapic = &cur->ioapic;
                log_debug("\t\t\tI/O APIC #%d (0x%016p)", buf_len(madt_ioapics), ioapic);
                buf_push(madt_ioapics, ioapic);
            } break;
            case MADT_ISO: {
                madt_iso_t* iso = &cur->iso;
                log_debug("\t\t\tISO #%d (0x%016p)", buf_len(madt_isos), iso);
                buf_push(madt_isos, iso);
            } break;
            case MADT_NMI: {
                madt_nmi_t* nmi = &cur->nmi;
                log_debug("\t\t\tNMI #%d (0x%016p)", buf_len(madt_nmis), nmi);
                buf_push(madt_nmis, nmi);
            } break;
            default:break;
        }
    }

cleanup:
    return err;
}
