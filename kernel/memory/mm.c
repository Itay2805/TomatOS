#include "mm.h"

#include <common/global_except.h>
#include <common/string.h>
#include <common/common.h>

#include <graphics/term.h>

#include "vmm.h"

#define MM_MAGIC (void*)0xCAFEBABE

////////////////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////////////////

/**
 * Will expand the heap
 *
 * @param context   [IN/OUT] The context
 * @param min       [IN]     The minimum size we need to expand to
 *
 * @remark
 * This assumes the parameters is valid, so it is up to the caller to verify that!
 */
static global_error_t expand(mm_context_t* context, size_t min) {
    // allocate all the required pages
    int attrs = PAGE_ATTR_WRITE;
    if(vmm_get() != kernel_address_space) {
        attrs |= PAGE_ATTR_USER;
    }
    min = ALIGN_UP(min, 4096u);
    size_t page_count = min / 4096u;
    char* start = (void *) ((uintptr_t)context->last + sizeof(mm_block_t) + context->last->size);
    for(size_t i = 0; i < page_count; i++) {
        vmm_allocate(vmm_get(), start + i * KB(4), attrs);
    }

    // update sizes
    context->total_size += min;

    if(context->last->allocated) {
        // the last block is already allocated, we need
        // to create a new block
        context->used_size += sizeof(mm_block_t);

        // set the new block
        mm_block_t* new_block = (mm_block_t *) ((uintptr_t)context->last + sizeof(mm_block_t) + context->last->size);
        new_block->magic = MM_MAGIC;
        new_block->allocated = false;
        new_block->alignment = 0;
        new_block->size = min - sizeof(mm_block_t);
        new_block->next = context->first;
        new_block->prev = context->last;

        // link into the chain
        context->last->next = new_block;
        context->first->prev = new_block;

        // update the last block
        context->last = new_block;
    }else {
        // last is not allocated, join with it
        context->last->size += min;
    }
    return NO_ERROR;
}


/**
 * Will iterate the blocks and attempt to join free blocks that are next to each other
 *
 * @param context [IN/OUT]  The context
 *
 * @remark
 * This assumes the parameters is valid, so it is up to the caller to verify that!
 */
static global_error_t join(mm_context_t* context) {
    mm_block_t* current = context->first;
    while (current->next != NULL) {
        mm_block_t* next = current->next;

        if(next == context->first) {
            // this cycles back to the start
            break;
        }

        if(!current->allocated && !next->allocated) {
            // join the blocks
            current->size += next->size + sizeof(mm_block_t);
            current->next = next->next;
            next->next->prev = current;

            // invalidate the block
            next->magic = 0;
            next->size = 0;
            next->prev = 0;
            next->next = 0;
            next->allocated = false;

            context->used_size -= sizeof(mm_block_t);
        }else {
            current = next;
        }
    }
    return NO_ERROR;
}

/**
 * Gets the required padding
 *
 * TODO: We can do this with just the block since it has the alignment
 */
static ptrdiff_t get_padding(mm_block_t* block, size_t alignment) {
    return ALIGN_UP((uintptr_t)&block->data, alignment) - (uintptr_t)&block->data;
}

/**
 * Will return the size with the padding needed for this block
 */
static ptrdiff_t get_size_with_padding(mm_block_t* block, size_t size, size_t alignment) {
    return size + get_padding(block, alignment);
}

/**
 * Will check if can fit the variable while taking into account the size and alignment of memory
 */
static bool check_size_and_alignment(mm_block_t* block, size_t size, size_t alignment) {
    return (bool) (get_size_with_padding(block, size, alignment) <= block->size);
}

/**
 * Will check if we have enough space in this block to split it to two blocks
 */
static bool can_split(mm_block_t* block, size_t size, size_t alignment) {
    ptrdiff_t size_left = block->size - sizeof(mm_block_t) - get_size_with_padding(block, size, alignment) - sizeof(size_t) * 3;
    return (bool) ((size_left) > 0);
}

/**
 * Will attempt to get the block from the ptr
 *
 * @param ptr       [IN]    The pointer to get the block for
 * @param block     [OUT]   The pointer to the block
 */
static global_error_t get_block_from_ptr(void* ptr, mm_block_t** block) {
    global_error_t err = NO_ERROR;
    mm_block_t* assumed_block = ptr - sizeof(mm_block_t);

    int padding = 0;
    while(assumed_block->magic != MM_MAGIC) {
        assumed_block = (mm_block_t *) ((uintptr_t)assumed_block - 1);
        padding++;
    }

    CHECK_GLOBAL_ERROR(padding == get_padding(assumed_block, assumed_block->alignment), ERROR_INVALID_POINTER);

    *block = assumed_block;

cleanup:
    return err;
}

/**
 * Will search for a free block, expanding the buffer if needed
 *
 * @param tried_join    [IN]        Have we tried to join?
 * @param tried_expand  [IN]        Have we tried to expand?
 *
 * @remark
 * This assumes the parameters is valid, so it is up to the caller to verify that!
 */
static global_error_t update_free(mm_context_t* context, bool tried_join, bool tried_expand) {
    global_error_t err = NO_ERROR;
    mm_block_t* current = context->free;
    bool first = true;

    while(current->allocated) {
        CHECK_GLOBAL_ERROR(first || current != context->free, ERROR_OUT_OF_MEMORY);
        current = current->next;
        first = false;
    }

    context->free = current;

cleanup:
    if(IS_GLOBAL_ERROR(err) == ERROR_OUT_OF_MEMORY) {
        if(!tried_join) {
            CHECK_GLOBAL_AND_RETHROW_LABEL(join(context), cleanup_failed);
            CHECK_GLOBAL_AND_RETHROW_LABEL(update_free(context, true, false), cleanup_failed);
        }else if(!tried_expand) {
            CHECK_GLOBAL_AND_RETHROW_LABEL(expand(context, sizeof(size_t) * 3), cleanup_failed);
            CHECK_GLOBAL_AND_RETHROW_LABEL(update_free(context, true, true), cleanup_failed);
        }
    }
cleanup_failed:
    return err;
}

/**
 * Handles the actual allocation, will also try to join/expand while allocating when an empty block
 * sufficient in size is not found
 *
 * @param context       [IN/OUT]    The context
 * @param size          [IN]        The size of the allocation
 * @param alignment     [IN]        The alignment of the allocation
 * @param ptr           [OUT]       The final pointer if allocated
 * @param tried_join    [IN]        Have we tried to join?
 * @param tried_expand  [IN]        Have we tried to expand?
 *
 * @remark
 * This assumes the parameters is valid, so it is up to the caller to verify that!
 */
static global_error_t allocate_internal(mm_context_t* context, size_t size, size_t alignment, void** ptr, bool tried_join, bool tried_expand) {
    global_error_t err = NO_ERROR;
    mm_block_t* current = context->free;
    bool first = true;
    void* allocated = NULL;

    while(current != NULL) {
        CHECK_GLOBAL_ERROR(first || current != context->free, ERROR_OUT_OF_MEMORY);

        if(!current->allocated && check_size_and_alignment(current, size, alignment)) {
            if(can_split(current, size, alignment)) {
                size_t newsize = (size_t) get_size_with_padding(current, size, alignment);
                mm_block_t* next = current->next;

                // set up the new block
                mm_block_t* new_next = (mm_block_t *) (current->data + newsize);
                new_next->next = next;
                new_next->prev = current;
                new_next->magic = MM_MAGIC;
                new_next->allocated = false;
                new_next->size = current->size - sizeof(mm_block_t) - newsize;

                // insert to the linked list
                current->next = new_next;
                current->size = newsize;
                next->prev = new_next;

                // update the size
                context->used_size += sizeof(mm_block_t);

                // update the last block
                if(current == context->last) {
                    context->last = new_next;
                }
            }

            // update the free block pointer
            CHECK_GLOBAL_AND_RETHROW(update_free(context, tried_join, tried_expand));

            // set as allocated
            current->alignment = alignment;
            current->allocated = true;
            context->used_size += current->size;
            allocated = current->data + get_padding(current, alignment);
            break;
        }

        first = false;
        current = current->next;
    }

    *ptr = allocated;

cleanup:
    if(IS_GLOBAL_ERROR(err) == ERROR_OUT_OF_MEMORY) {
        if(!tried_join) {
            CHECK_GLOBAL_AND_RETHROW_LABEL(join(context), cleanup_failed);
            CHECK_GLOBAL_AND_RETHROW_LABEL(allocate_internal(context, size, alignment, ptr, true, false), cleanup_failed);
        }else if(!tried_expand) {
            CHECK_GLOBAL_AND_RETHROW_LABEL(expand(context, sizeof(size_t) * 3), cleanup_failed);
            CHECK_GLOBAL_AND_RETHROW_LABEL(allocate_internal(context, size, alignment, ptr, true, true), cleanup_failed);
        }
    }
cleanup_failed:
    return err;
}

/**
 * Actual implementation of free
 *
 * @param block [IN] The block to free
 *
 * @remark
 * Will assume the block is valid
 */
static global_error_t internal_free(mm_context_t* context, mm_block_t* block) {
    block->alignment = 0;
    block->allocated = false;
    context->used_size -= block->size;

    return NO_ERROR;
}

////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////


void mm_context_init(mm_context_t* context, uintptr_t virtual_start) {
    spinlock_lock(&context->lock);

    int attrs = PAGE_ATTR_WRITE;
    if(vmm_get() != kernel_address_space) {
        attrs |= PAGE_ATTR_USER;
    }

    vmm_allocate(vmm_get(), (void *) virtual_start, attrs);
    context->first = (mm_block_t*)virtual_start;
    context->last = context->first;
    context->free = context->first;
    context->total_size = 4096;
    context->used_size = sizeof(mm_block_t);

    context->first->size = 4096 - sizeof(mm_block_t);
    context->first->magic = MM_MAGIC;
    context->first->prev = context->first;
    context->first->next = context->first;
    context->first->allocated = false;

    spinlock_unlock(&context->lock);
}

void* mm_allocate(mm_context_t* context, size_t size) {
    return mm_allocate_aligned(context, size, 8);
}

void* mm_allocate_aligned(mm_context_t* context, size_t size, size_t alignment) {
    global_error_t err = NO_ERROR;
    void* ptr = NULL;

    spinlock_lock(&context->lock);

    CHECK_GLOBAL_AND_RETHROW(allocate_internal(context, size, alignment, (void**)&ptr, false, false));

cleanup:
    if(IS_GLOBAL_ERROR(err) != NO_ERROR) {
        KERNEL_GLOBAL_PANIC();
    }

    spinlock_unlock(&context->lock);

    return ptr;
}

void mm_free(mm_context_t* context, void* ptr) {
    global_error_t err = NO_ERROR;
    mm_block_t* block = NULL;

    spinlock_lock(&context->lock);

    CHECK_GLOBAL_ERROR(context, ERROR_INVALID_ARGUMENT);

    CHECK_GLOBAL_ERROR((void*)(context->last + context->last->size) > ptr && (void*)context->first < ptr, ERROR_INVALID_POINTER);

    CHECK_GLOBAL_AND_RETHROW(get_block_from_ptr(ptr, &block));
    CHECK_GLOBAL_ERROR(block->allocated, ERROR_INVALID_POINTER);

    CHECK_GLOBAL_AND_RETHROW(internal_free(context, block));

cleanup:
    // we ignore invalid pointer in the free
    if(IS_GLOBAL_ERROR(err) != NO_ERROR) {
        KERNEL_GLOBAL_PANIC();
    }

    spinlock_unlock(&context->lock);
}

void* mm_reallocate(mm_context_t* context, void* ptr, size_t size) {
    global_error_t err = NO_ERROR;
    mm_block_t* block = NULL;
    void* new = NULL;
    size_t s = 0;

    spinlock_lock(&context->lock);

    CHECK_GLOBAL_ERROR(context, ERROR_INVALID_ARGUMENT);
    CHECK_GLOBAL_ERROR(size > 0, ERROR_INVALID_ARGUMENT);

    if(ptr == NULL) {
        new = mm_allocate(context, size);
    }else {
        CHECK_GLOBAL_AND_RETHROW(get_block_from_ptr(ptr, &block));
        CHECK_GLOBAL_ERROR(block->allocated, ERROR_ALREADY_FREED);

        if(block->size > (ptrdiff_t)size) {
            s = (size_t) (block->size - get_padding(block, block->alignment));
            CHECK_GLOBAL_AND_RETHROW(allocate_internal(context, size, block->alignment, (void**)new, false, false));
        }else {
            new = ptr;
        }
    }

    if(ptr != new) {
        // copy the old memory and free the new one
        memcpy(new, ptr, s);
        memset(new + s, 0, size - s);

        // free the block
        CHECK_GLOBAL_AND_RETHROW(internal_free(context, block));
    }

cleanup:
    if(IS_GLOBAL_ERROR(err) != NO_ERROR) {
        KERNEL_GLOBAL_PANIC();
    }

    spinlock_unlock(&context->lock);

    return ptr;
}
