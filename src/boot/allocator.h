#ifndef TOMATKERNEL_BOOT_ALLOCATOR_H
#define TOMATKERNEL_BOOT_ALLOCATOR_H

#include <stddef.h>

/**
 * The boot allocator allows the boot stuff allocate memory for the boot structure
 *
 * It is going to use either 0x00000500-0x00007BFF or 0x00007E00-0x0007FFFF depending
 * on where the boot_info is located, of course that assumes it is only going to use
 * one space, but I think it should be good enough at least for the boot.
 *
 * @param boot_info [IN] The start of the boot info
 */
void boot_allocator_init(void* boot_info);

/**
 * Will allocate some memory
 *
 * @param size  [IN] The size
 */
void* boot_allocate(size_t size);

#endif //TOMATKERNEL_BOOT_ALLOCATOR_H
