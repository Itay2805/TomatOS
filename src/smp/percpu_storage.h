#ifndef TOMATKERNEL_PERCPU_STORAGE_H
#define TOMATKERNEL_PERCPU_STORAGE_H

#include <libc/stddef.h>
#include <memory/gdt.h>
#include <processes/thread.h>

typedef struct per_cpu_storage {
    // scheduler related
    uintptr_t syscall_stack; /* must be first! this is because of how the syscall trampoline works */
    thread_t* running_thread;
    thread_t* idle_thread;

    // apic
    uintptr_t lapic_id;
    uintptr_t processor_id;

    // kernel stacks
    uintptr_t interrupt_stack;
    uintptr_t exception_stack;
    uintptr_t nmi_stack;
    uintptr_t page_fault_stack;

    // misc
    tss64_t tss;
} per_cpu_storage_t;

/**
 * Allocate the per cpu storage structures
 */
void percpu_storage_init();

/**
 * Get the cpu storage of the current cpu
 */
per_cpu_storage_t* get_percpu_storage();

/**
 * Get the per cpu storage of another processor
 *
 * @param lapic_id  [IN] The lapic id of the processor
 */
per_cpu_storage_t* get_percpu_storage_of(int lapic_id);

#endif //TOMATKERNEL_PERCPU_STORAGE_H
