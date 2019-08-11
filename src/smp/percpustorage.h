#ifndef TOMATOS_PERCPUSTORAGE_H
#define TOMATOS_PERCPUSTORAGE_H

#include <stdint.h>
#include <stddef.h>
#include <error.h>

typedef struct per_cpu_storage {
    // the APIC processor and lapic id
    uint32_t lapic_id;
    uint32_t processor_id;

    // The stack to use normally in the kernel
    uintptr_t kernel_stack;

    // The stack to use during an NMI
    uintptr_t nmi_stack;

    // the per cpu index, starts from 0
    size_t index;
} per_cpu_storage_t;

/**
 * The map of the lapic id to the per cpu storage
 */
extern per_cpu_storage_t* per_cpu_storage;

/**
 * Allocate all the per cpu storage
 */
error_t per_cpu_storage_init();

/**
 * Get the storage of the current cpu
 * @return
 */
per_cpu_storage_t* get_per_cpu_storage();

/**
 * Get the index of the current cpu
 */
size_t get_cpu_index();

#endif //TOMATOS_PERCPUSTORAGE_H
