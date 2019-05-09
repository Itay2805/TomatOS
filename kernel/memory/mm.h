#ifndef COMMON_MM_H
#define COMMON_MM_H

////////////////////////////////////////////////////////////////////////////
// Memory Manager
////////////////////////////////////////////////////////////////////////////
//
// This is the dynamic memory manager (also called heap).
//
// It is mostly used by the kernel to allocate dynamic data for objects (like
// threads, processes, events and so on)
//
// Note: Since the mm works in virtual memory, meaning that before you try
//       and allocate something in another address space you must switch to
//       that address space first!
//
////////////////////////////////////////////////////////////////////////////

#include <common/stdint.h>
#include <common/stdbool.h>
#include <locks/spinlock.h>

/**
 * Am allocated memory block
 */
typedef struct mm_block {
    void* magic;
    ptrdiff_t size;
    bool allocated;
    struct mm_block* next;
    struct mm_block* prev;
    size_t alignment;
    char data[0];
} mm_block_t;

/**
 * The memory manager context
 */
typedef struct mm_context {
    mm_block_t* first;
    mm_block_t* last;
    mm_block_t* free;
    size_t total_size;
    size_t used_size;
} mm_context_t;

/**
 * Initialize a new context and have the data start at the given address
 *
 * @param context       [IN/OUT]    The context of the memory manager
 * @param virtual_start [IN]        The address to start the memory manager from (virtual)
 *
 * @remark
 * Right now this will not stop growing up
 * TODO: Should probably add a size limit
 */
void mm_context_init(mm_context_t* context, uintptr_t virtual_start);

/**
 * Allocate memory of the given size
 *
 * @param context   [IN/OUT]    The context of the memory manager
 * @param size      [IN]        The amount of bytes to allocate
 *
 * @remark
 * Internally this will simply call mm_allocate_aligned with an alignment of 8bytes
 *
 * @return
 *  Pointer for the allocated buffer
 *
 * @see
 * mm_allocate_aligned
 *
 */
void* mm_allocate(mm_context_t* context, size_t size);

/**
 * Allocate memory of the given size, but have it aligned to the alignment
 *
 * @param context   [IN/OUT]    The context of the memory manager
 * @param size      [IN]        The amount of bytes to allocate
 * @param alignment [IN]        The alignment, in bytes, of the starting address
 *
 * @return
 * Pointer for the allocated pointer
 */
void* mm_allocate_aligned(mm_context_t* context, size_t size, size_t alignment);

/**
 * Free an allocated buffer
 *
 * @param context   [IN/OUT]    The context of the memory manager
 * @param ptr       [IN]        The pointer to free
 */
void mm_free(mm_context_t* context, void* ptr);

/**
 * Reallocate a given buffer, will allocate a new buffer if ptr is null
 *
 * @param context   [IN/OUT]    The context of the memory manager
 * @param ptr       [IN]        The pointer to reallocate
 * @param size      [IN]        The new size of the buffer
 *
 * @remark
 * If the pointer was NULL will simply allocate a buffer
 *
 * @return
 * The new allocated buffer
 */
void* mm_reallocate(mm_context_t* context, void* ptr, size_t size);

#endif
