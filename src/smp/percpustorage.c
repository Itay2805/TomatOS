#include <stb/stb_ds.h>
#include <interrupts/apic/lapic.h>
#include <acpi/tables/madt.h>
#include "percpustorage.h"

per_cpu_storage_entry_t* per_cpu_storage;

error_t per_cpu_storage_init() {
    error_t err = NO_ERROR;

    for(int i = 0; i < arrlen(madt_lapics); i++) {
        madt_lapic_t* lapic = madt_lapics[i];
        if(!lapic->processor_enabled) continue;

        per_cpu_storage_t st = {
            .processor_id = lapic->processor_id,
            .kernel_stack = (uintptr_t) kmalloc(MB(2)),
            .nmi_stack = (uintptr_t) kmalloc(KB(512))
        };

        hmput(per_cpu_storage, lapic->id, st);
    }

cleanup:
    return err;
}

per_cpu_storage_t* get_per_cpu_storage() {
    return &hmget(per_cpu_storage, lapic_get_id());
}
