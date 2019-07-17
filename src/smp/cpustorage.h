#ifndef TOMATKERNEL_CPUSTORAGE_H
#define TOMATKERNEL_CPUSTORAGE_H

#include <libc/stddef.h>
#include <common/error.h>

////////////////////////////////////////////////////////////
// The storage stuff
////////////////////////////////////////////////////////////

typedef struct per_cpu_storage {
    size_t index;
    uintptr_t kernel_stack;
} per_cpu_storage_t;

/**
 * Array of per cpu storage
 */
extern per_cpu_storage_t* per_cpu_storage;

/**
 * This will allocate an initialize the per cpu storage
 */
error_t per_cpu_storage_init();

/**
 * Will set the gs of the current
 *
 * @param index [IN] The cpu index of the current cpu
 */
error_t set_cpu_storage(size_t index);

////////////////////////////////////////////////////////////
// Other utilities
////////////////////////////////////////////////////////////

#define _swapgs() asm volatile("swapgs")

__attribute__((always_inline))
static inline uint64_t read_gs_64(uint64_t offset) {
    uint32_t ret;
    asm volatile ("mov  %%gs:(%1), %0" : "=r"(ret) : "r"(offset));
    return ret;
}

////////////////////////////////////////////////////////////
// Utility functions to get the stuff
////////////////////////////////////////////////////////////

size_t cpu_get_index();
uintptr_t cpu_get_kernel_stack();

#endif //TOMATKERNEL_CPUSTORAGE_H
