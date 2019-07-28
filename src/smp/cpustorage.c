#include <stb/stb_ds.h>
#include <acpi/tables/madt.h>
#include <common/common.h>
#include <common/cpu/msr.h>
#include "cpustorage.h"

extern uintptr_t pcpu_storage_size;
#define PCPU_STORAGE_SIZE ADDROF(pcpu_storage_size)

char** all_pcpu_storage;
PCPU size_t cpu_index;
PCPU uintptr_t cpu_kernel_stack;

error_t per_cpu_storage_init() {

    // amount of per cpu storage
    size_t active_lapis = 0;
    for(size_t i = 0; i < arrlen(madt_lapics); i++) {
        if(madt_lapics[i]->processor_enabled) {
            active_lapis++;
        }
    }
    arrsetlen(all_pcpu_storage, active_lapis);

    // allocate it all
    for(size_t i = 0; i < arrlen(all_pcpu_storage); i++) {
        all_pcpu_storage[i] = kmalloc(PCPU_STORAGE_SIZE);
    }

    return NO_ERROR;
}

error_t set_cpu_storage(size_t index) {
    log_info("\tset per cpu storage");
    _wrmsr(IA32_GS_BASE, (uint64_t) &all_pcpu_storage[index]);
    cpu_index = index;
    cpu_kernel_stack = (uintptr_t)kmalloc(MB(2));
    return NO_ERROR;
}

size_t get_core_count() {
    return arrlen(all_pcpu_storage);
}

