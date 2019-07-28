#include <common.h>
#include <string.h>
#include <locks/spinlock.h>
#include "mm.h"
#include "vmm.h"

////////////////////////////////////////////////////////////////////////////
// The block
////////////////////////////////////////////////////////////////////////////

#define MM_MAGIC (void*)0xCAFEBABE
#define MM_BLOCK_SIZE offsetof(mm_block_t, data)

typedef struct mm_block {
    void* magic_front;

    size_t size;
    bool allocated;
    struct mm_block* prev;
    struct mm_block* next;
    size_t alignment;

    void* magic_back;

    char data[0];
} mm_block_t;


////////////////////////////////////////////////////////////////////////////
// The context of the memory manager
////////////////////////////////////////////////////////////////////////////

// TODO: Maybe add a stack for storing free blocks instead of only storing one free block
static mm_block_t* first_block;
static mm_block_t* last_block;
static mm_block_t* free_block;
static size_t total_size;
static size_t used_size;
static spinlock_t mm_lock;

////////////////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////////////////

/**
 * Will expand the heap
 *
 * @param min       [IN]     The minimum size we need to expand to
 *
 * @remark
 * This assumes the parameters is valid, so it is up to the caller to verify that!
 */
static error_t expand(size_t min) {
    // allocate all the required pages
    if(last_block->allocated) {
        min += MM_BLOCK_SIZE;
    }

    min = ALIGN_UP(min, 4096u);
    size_t page_count = min / 4096u;
    char* start = (void *) ((uintptr_t)last_block + MM_BLOCK_SIZE + last_block->size);
    for(size_t i = 0; i < page_count; i++) {
        vmm_allocate(vmm_get(), start + i * KB(4), PAGE_ATTR_WRITE);
    }

    // update sizes
    total_size += min;

    if(last_block->allocated) {
        // the last block is already allocated, we need
        // to create a new block
        used_size += MM_BLOCK_SIZE;

        // set the new block
        mm_block_t* new_block = (mm_block_t *) ((uintptr_t)last_block + MM_BLOCK_SIZE + last_block->size);
        new_block->magic_front = MM_MAGIC;
        new_block->magic_back = MM_MAGIC;
        new_block->allocated = false;
        new_block->alignment = 0;
        new_block->size = min - MM_BLOCK_SIZE;
        new_block->next = first_block;
        new_block->prev = last_block;

        // link into the chain
        last_block->next = new_block;
        first_block->prev = new_block;

        // update the last block
        last_block = new_block;
    }else {
        // last is not allocated, join with it
        last_block->size += min;
    }
    return NO_ERROR;
}

/**
 * Will iterate the blocks and attempt to join free blocks that are next to each other
 *
 * @remark
 * This assumes the parameters is valid, so it is up to the caller to verify that!
 */
static error_t join() {
    mm_block_t* current = first_block;
    while (current->next != NULL) {
        mm_block_t* next = current->next;

        if(next == first_block) {
            // this cycles back to the start
            break;
        }

        if(!current->allocated && !next->allocated) {
            // join the blocks
            current->size += next->size + MM_BLOCK_SIZE;
            current->next = next->next;
            next->next->prev = current;

            // invalidate the block
            next->magic_front = 0;
            next->magic_back = 0;
            next->size = 0;
            next->prev = 0;
            next->next = 0;
            next->allocated = false;

            // update context pointers if needed
            if(next == free_block) {
                free_block = current;
            }

            if(next == last_block) {
                last_block = current;
                first_block->prev = last_block;
            }

            used_size -= MM_BLOCK_SIZE;
        }else {
            current = next;
        }
    }
    return NO_ERROR;
}

/**
 * Gets the required padding
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
    ptrdiff_t size_left = block->size - MM_BLOCK_SIZE - get_size_with_padding(block, size, alignment) - sizeof(size_t) * 3;
    return (bool) ((size_left) > 0);
}

/**
 * Will attempt to get the block from the ptr
 *
 * @param ptr       [IN]    The pointer to get the block for
 * @param block     [OUT]   The pointer to the block
 */
static error_t get_block_from_ptr(void* ptr, mm_block_t** block) {
    error_t err = NO_ERROR;
    mm_block_t* assumed_block = ptr - offsetof(mm_block_t, data);

    CHECK_ERROR(vmm_is_mapped(kernel_address_space, (uintptr_t) ptr), ERROR_NOT_MAPPED);

    int padding = 0;
    while(assumed_block->magic_front != MM_MAGIC) {
        assumed_block = (mm_block_t *) ((uintptr_t)assumed_block - 1);
        padding++;
        CHECK_ERROR(vmm_is_mapped(kernel_address_space, (uintptr_t) assumed_block), ERROR_NOT_MAPPED);
    }

    CHECK_ERROR(padding == get_padding(assumed_block, assumed_block->alignment), ERROR_INVALID_POINTER);

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
static error_t update_free(bool tried_join, bool tried_expand) {
    error_t err = NO_ERROR;
    mm_block_t* current = free_block;
    bool first = true;

    while(current->allocated) {
        CHECK_ERROR_QUITE(first || current != free_block, ERROR_OUT_OF_MEMORY);
        current = current->next;
        first = false;
    }

    free_block = current;

cleanup:
    if(err == ERROR_OUT_OF_MEMORY) {
        if(!tried_join) {
            CHECK_AND_RETHROW_LABEL_QUITE(join(), cleanup_failed);
            CHECK_AND_RETHROW_LABEL_QUITE(update_free(true, false), cleanup_failed);
        }else if(!tried_expand) {
            CHECK_AND_RETHROW_LABEL_QUITE(expand(sizeof(size_t) * 3), cleanup_failed);
            CHECK_AND_RETHROW_LABEL_QUITE(update_free(true, true), cleanup_failed);
        }
    }
cleanup_failed:
    return err;
}

/**
 * Attempts to verify the integrity of the memory manager
 */
static error_t verify_integrity() {
    error_t err = NO_ERROR;
    mm_block_t* current = first_block;

    do {
        // some basic checks
        CHECK(current != NULL);
        CHECK(current->magic_front == MM_MAGIC);
        CHECK(current->magic_back == MM_MAGIC);
        CHECK(current->size != 0);
        CHECK(current->size < total_size);

        current = current->next;

        // fully iterated
        if (current == first_block) {
            break;
        }
    } while(current != first_block);

cleanup:
    return err;
}

/**
 * Handles the actual allocation, will also try to join/expand while allocating when an empty block
 * sufficient in size is not found
 *
 * @param size          [IN]        The size of the allocation
 * @param alignment     [IN]        The alignment of the allocation
 * @param ptr           [OUT]       The final pointer if allocated
 * @param tried_join    [IN]        Have we tried to join?
 * @param tried_expand  [IN]        Have we tried to expand?
 *
 * @remark
 * This assumes the parameters is valid, so it is up to the caller to verify that!
 */
static error_t allocate_internal(size_t size, size_t alignment, void** ptr, bool tried_join, bool tried_expand) {
    error_t err = NO_ERROR;
    mm_block_t* current = free_block;
    bool first = true;
    void* allocated = NULL;

    if(current == NULL) {
        CHECK_AND_RETHROW(update_free(tried_join, tried_expand));
        current = free_block;
    }

    CHECK(current != NULL);

    while(current != NULL) {
        CHECK_ERROR_QUITE(first || current != free_block, ERROR_OUT_OF_MEMORY);

        if(!current->allocated && check_size_and_alignment(current, size, alignment)) {
            if(can_split(current, size, alignment)) {
                size_t newsize = (size_t) get_size_with_padding(current, size, alignment);
                mm_block_t* next = current->next;

                // set up the new block
                mm_block_t* new_next = (mm_block_t *) (current->data + newsize);
                new_next->next = next;
                new_next->prev = current;
                new_next->magic_front = MM_MAGIC;
                new_next->magic_back = MM_MAGIC;
                new_next->allocated = false;
                new_next->size = current->size - MM_BLOCK_SIZE - newsize;
                new_next->alignment = 0;

                // insert to the linked list
                current->next = new_next;
                current->size = newsize;
                next->prev = new_next;

                // update the size
                used_size += MM_BLOCK_SIZE;

                // update the last block
                if(current == last_block) {
                    last_block = new_next;
                }
            }

            // set as allocated
            current->alignment = alignment;
            current->allocated = true;
            used_size += current->size;
            memset(current->data, 0, (size_t) current->size);
            allocated = current->data + get_padding(current, alignment);

            // update the free block pointer
            CHECK_AND_RETHROW(update_free(tried_join, tried_expand));

            break;
        }

        first = false;
        current = current->next;
    }

    CHECK_ERROR_QUITE(allocated != NULL, ERROR_OUT_OF_MEMORY);
    *ptr = allocated;

cleanup:
    // attempt and recover by trying and join/allocate more memory
    if(err == ERROR_OUT_OF_MEMORY) {
        if(!tried_join) {
            CHECK_AND_RETHROW_LABEL_QUITE(join(), cleanup_failed);
            CHECK_AND_RETHROW_LABEL_QUITE(allocate_internal(size, alignment, ptr, true, false), cleanup_failed);
        }else if(!tried_expand) {
            size_t min;
            if(size > alignment) {
                min = size + alignment * 2;
            }else {
                min = alignment * 3;
            }
            CHECK_AND_RETHROW_LABEL_QUITE(expand(min), cleanup_failed);
            CHECK_AND_RETHROW_LABEL_QUITE(allocate_internal(size, alignment, ptr, true, true), cleanup_failed);
        }
    }
    CHECK_AND_RETHROW_LABEL(verify_integrity(), cleanup_failed);
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
static error_t internal_free(mm_block_t* block) {
    block->alignment = 0;
    block->allocated = false;
    used_size -= block->size;

    return NO_ERROR;
}

////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////

error_t mm_init() {
    error_t err = NO_ERROR;

    CHECK_AND_RETHROW(vmm_allocate(kernel_address_space, (void *) MM_BASE, PAGE_ATTR_WRITE));
    first_block = (mm_block_t*)MM_BASE;
    last_block = first_block;
    free_block = first_block;
    total_size = KB(4);
    used_size = MM_BLOCK_SIZE;

    first_block->size = KB(4) - MM_BLOCK_SIZE;
    first_block->magic_front = MM_MAGIC;
    first_block->magic_back = MM_MAGIC;
    first_block->prev = first_block;
    first_block->next = first_block;
    first_block->allocated = false;
    first_block->alignment = 0;

cleanup:
    return err;
}

error_t mm_allocate(size_t size, void** out_ptr) {
    return mm_allocate_aligned(size, 8, out_ptr);
}

error_t mm_allocate_aligned(size_t size, size_t alignment, void** out_ptr) {
    error_t err = NO_ERROR;
    void* ptr = NULL;
    mm_block_t* block;

    lock_preemption(&mm_lock);

    CHECK_ERROR(size > 0, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(out_ptr, ERROR_INVALID_ARGUMENT);

    CHECK_AND_RETHROW(verify_integrity());
    CATCH(allocate_internal(size, alignment, &ptr, false, false), CHECK_FAIL_ERROR(err));
    CHECK(ptr != NULL);
    CHECK_AND_RETHROW(get_block_from_ptr(ptr, &block));
    memset(ptr, 0, size);
    CHECK_AND_RETHROW(verify_integrity());

    *out_ptr = ptr;

cleanup:
    unlock_preemption(&mm_lock);
    return err;
}

error_t mm_free(void* ptr) {
    error_t err = NO_ERROR;
    mm_block_t* block = NULL;

    lock_preemption(&mm_lock);

    CHECK_ERROR((void*)(last_block + last_block->size) > ptr && (void*)first_block < ptr, ERROR_INVALID_POINTER);

    CHECK_AND_RETHROW(verify_integrity());
    CHECK_ERROR(vmm_is_mapped(kernel_address_space, (uintptr_t) ptr), ERROR_NOT_MAPPED);
    CHECK_AND_RETHROW(get_block_from_ptr(ptr, &block));
    CHECK_ERROR(block->allocated, ERROR_INVALID_POINTER);

    CHECK_AND_RETHROW(internal_free(block));
    CHECK_AND_RETHROW(verify_integrity());

cleanup:
    unlock_preemption(&mm_lock);
    return err;
}

error_t mm_reallocate(void** ptr, size_t size) {
    error_t err = NO_ERROR;
    mm_block_t* new_block = NULL;
    mm_block_t* block = NULL;
    void* new = NULL;
    size_t old_size = 0;

    lock_preemption(&mm_lock);

    CHECK_ERROR(size > 0, ERROR_INVALID_ARGUMENT);

    CHECK_AND_RETHROW(verify_integrity());

    if(ptr == NULL || *ptr == NULL) {
        CATCH(allocate_internal(size, 8, &new, false, false), CHECK_FAIL_ERROR(err));
    }else {
        CHECK_AND_RETHROW(get_block_from_ptr(*ptr, &block));
        // TODO: CHECK_ERROR(block->allocated, ERROR_ALREADY_FREED);

        ptrdiff_t padding = get_padding(block, block->alignment);
        if(block->size - padding < (ptrdiff_t)size) {
            old_size = (size_t) (block->size - padding);
            CATCH(allocate_internal(size, block->alignment, &new, false, false), CHECK_FAIL_ERROR(err) );
        }else {
            new = *ptr;
        }
    }
    CHECK_AND_RETHROW(get_block_from_ptr(new, &new_block));

    if(*ptr != new && block != NULL) {
        // copy the old memory, zero the new memory and free the new one
        memcpy(new, *ptr, old_size);
        memset(new + old_size, 0, size - old_size);

        // free the block
        CHECK_AND_RETHROW(internal_free(block));
    }
    CHECK_AND_RETHROW(verify_integrity());

    *ptr = new;

cleanup:
    unlock_preemption(&mm_lock);
    return err;
}
