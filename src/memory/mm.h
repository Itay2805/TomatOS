#ifndef TOMATKERNEL_MM_H
#define TOMATKERNEL_MM_H

#include <error.h>

// TODO: Add locks to all of these
#define MM_BASE                0xFFFFFFF800000000
#define KERNEL_MAPPING_BASE    0xFFFFFFFFC0000000

error_t mm_init();

/**
 * Allocate a new buffer
 *
 * @remark
 * This uses an alignment of 8
 *
 * @param size          [IN]    The size of the buffer to allocate
 * @param out_ptr       [OUT]   The out pointer to the allocate buffer
 */
error_t mm_allocate(size_t size, void** out_ptr);

/**
 * Allocate a buffer aligned to certain address
 * @param size          [IN]    The size of the new buffer
 * @param alignment     [IN]    The alignment of the base address for the new buffer
 * @param out_ptr       [OUT]   The out pointer to the allocate buffer
 */
error_t mm_allocate_aligned(size_t size, size_t alignment, void** out_ptr);

/**
 * Free the given buffer
 *
 * @param ptr   [IN] The pointer of the buffer to free
 */
error_t mm_free(void* ptr);

/**
 * Reallocate a buffer
 *
 * @param ptr       [IN/OUT]    The buffer to reallocate, the new buffer pointer will be also put here
 * @param size      [IN]        The new size
 */
error_t mm_reallocate(void** ptr, size_t size);

#endif //TOMATKERNEL_MM_H
