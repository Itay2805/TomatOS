#include <stb/stb_ds.h>
#include <acpi/tables/madt.h>
#include <common/common.h>
#include <common/cpu/msr.h>
#include "cpustorage.h"

per_cpu_storage_t* per_cpu_storage;

error_t per_cpu_storage_init() {

    // amount of per cpu storage
    size_t active_lapis = 0;
    for(size_t i = 0; i < arrlen(madt_lapics); i++) {
        if(madt_lapics[i]->processor_enabled) {
            active_lapis++;
        }
    }
    arrsetlen(per_cpu_storage, active_lapis);

    // set each of them
    for(size_t i = 0; i < arrlen(per_cpu_storage); i++) {
        per_cpu_storage[i].index = i;
        per_cpu_storage[i].kernel_stack = (uintptr_t) kmalloc(MB(2));
    }

    return NO_ERROR;
}

error_t set_cpu_storage(size_t index) {
    log_info("\tset per cpu storage");
    _wrmsr(IA32_GS_BASE, (uint64_t) &per_cpu_storage[index]);
    return NO_ERROR;
}

size_t cpu_get_index() {
    return read_gs_64(offsetof(per_cpu_storage_t, index));
}

uintptr_t cpu_get_kernel_stack() {
    return read_gs_64(offsetof(per_cpu_storage_t, kernel_stack));
}


