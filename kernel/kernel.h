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

#endif //TOMATKERNEL_KERNEL_H
