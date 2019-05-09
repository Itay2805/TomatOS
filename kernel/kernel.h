#ifndef TOMATKERNEL_KERNEL_H
#define TOMATKERNEL_KERNEL_H

#include <memory/mm.h>

#include "common/multiboot.h"

/**
 * The kernel entry point
 *
 * Note: This function assumes we are already in long mode 64bit and have the first
 *       1GB identity mapped
 */
void kernel_main(multiboot_info_t* info);

/**
 * This is the memory manager of the kernel
 */
extern mm_context_t kernel_memory_manager;

/**
 * This is the memory manager for kernel processes
 */
extern mm_context_t kernel_process_memory_manager;

/**
 * Will get the current memory manager, while in interrupt context we are going to use
 * the kernel_memory_manager, otherwise we will use the kernel_process_memory_manager.
 *
 * @remark
 * You must free the pointer with the memory manager you allocated with as these are
 * two completely different allocations
 */
mm_context_t* get_current_memory_manager();

#endif //TOMATKERNEL_KERNEL_H
