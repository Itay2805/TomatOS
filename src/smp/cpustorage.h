#ifndef TOMATKERNEL_CPUSTORAGE_H
#define TOMATKERNEL_CPUSTORAGE_H

#include <libc/stddef.h>
#include <common/error.h>

////////////////////////////////////////////////////////////
// The storage stuff
////////////////////////////////////////////////////////////

#define PCPU __attribute__((section(".pcpu"), address_space(256)))

/**
 * Array of per cpu storage
 */
extern char** all_pcpu_storage;

/**
 * Lapics per cpu index
 */
extern uint32_t* cpu_lapics;

////////////////////////////////////////////////////////////
// Common per cpu variables
////////////////////////////////////////////////////////////

extern PCPU size_t cpu_index;
extern PCPU uintptr_t cpu_kernel_stack;

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

/**
 * Will get the count of cores we have
 */
size_t get_core_count();

#endif //TOMATKERNEL_CPUSTORAGE_H
