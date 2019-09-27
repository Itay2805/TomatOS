#ifndef TOMATKERNEL_ERCPUSTORAGE_H
#define TOMATKERNEL_ERCPUSTORAGE_H

#include <stdint.h>
#include <stddef.h>
#include <error.h>

typedef struct per_cpu_storage {
    // stacks used by a syscall
    // NOTE: These must be first
    uintptr_t syscall_stack;
    uintptr_t user_stack;

    // IST stacks
    uintptr_t kernel_stack;
    uintptr_t nmi_stack;
    uintptr_t exception_stack;
    uintptr_t page_fault_stack;

    // the APIC processor and lapic id
    uint32_t lapic_id;
    uint32_t processor_id;

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

/**
 * Get the cpu cpunt
 */
size_t get_cpu_count();

#endif //TOMATKERNEL_ERCPUSTORAGE_H
