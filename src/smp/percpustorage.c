#include <stb/stb_ds.h>
#include <interrupts/apic/lapic.h>
#include <acpi/tables/madt.h>
#include "percpustorage.h"

// TODO: Use an array instead of a hashmap
per_cpu_storage_t* per_cpu_storage;

error_t per_cpu_storage_init() {
    error_t err = NO_ERROR;

    // get highest lapic number
    int num = 0;
    for(int i = 0; i < arrlen(madt_lapics); i++) {
        madt_lapic_t* lapic = madt_lapics[i];
        if(!lapic->processor_enabled) continue;
        if(num < lapic->id) {
            num = lapic->id;
        }
    }
    arrsetlen(per_cpu_storage, num + 1);

    // now fill all the per process information
    for(int i = 0; i < arrlen(madt_lapics); i++) {
        madt_lapic_t* lapic = madt_lapics[i];
        if(!lapic->processor_enabled) continue;

        per_cpu_storage_t st = {
            .processor_id = lapic->processor_id,
            .index = arrlen(per_cpu_storage),

            // allocate stacks
            .kernel_stack = (uintptr_t) vmalloc(MB(2)),
            .nmi_stack = (uintptr_t) vmalloc(KB(8)),
            .exception_stack = (uintptr_t) vmalloc(KB(8)),
            .page_fault_stack = (uintptr_t) vmalloc(KB(8)),
        };

        per_cpu_storage[lapic->id] = st;
    }

//cleanup:
    return err;
}

per_cpu_storage_t* get_per_cpu_storage() {
    return &per_cpu_storage[lapic_get_id()];
}

size_t get_cpu_index() {
    return get_per_cpu_storage()->index;
}
