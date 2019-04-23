#include <common/string.h>
#include <common/common.h>
#include <graphics/term.h>
#include "mm.h"

#define MM_MAGIC (void*)0xCAFEBABE

////////////////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////////////////

/**
 * Will expand the heap
 */
static void expand(mm_context_t* context, size_t min) {
    // allocate all the required pages
    int attrs = PAGE_ATTR_WRITE;
    if(vmm_get() != kernel_address_space) {
        attrs |= PAGE_ATTR_USER;
    }
    min = ALIGN_UP(min, 4096u);
    size_t page_count = min / 4096u;
    char* start = (void *) ((uintptr_t)context->last + sizeof(mm_block_t) + context->last->size);
    for(size_t i = 0; i < page_count; i++) {
        vmm_allocate(vmm_get(), start + i * 4096, attrs);
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

}


/**
 * Will iterate the blocks and attempt to join free blocks that are next to each other
 */
static void join(mm_context_t* context) {
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
    return (bool) ((ptrdiff_t)block->size - sizeof(mm_block_t) - get_size_with_padding(block, size, alignment) - sizeof(size_t) * 3 > 0);
}

/**
 * Will attempt to get the block from the ptr
 */
static mm_block_t* get_block_from_ptr(void* ptr) {
    mm_block_t* block = ptr - sizeof(mm_block_t);
    int padding = 0;
    while(block->magic != MM_MAGIC) {
        block = (mm_block_t *) ((uintptr_t)block - 1);
        padding++;
    }
    if(padding == get_padding(block, block->alignment)) {
        return block;
    }else {
        return NULL;
    }
}

/**
 * Will update the free block
 */
static void update_free(mm_context_t* context, bool tried_join, bool tried_expand) {
    mm_block_t* current = context->free;
    bool first = true;
    while(current->allocated) {
        if(!first && current == context->free) {
            // we went through a full loop
            context->free = NULL;
            break;
        }
        current = current->next;
        first = false;
    }

    if(context->free == NULL) {
        // join and expand if needed
        if(!tried_join) {
            join(context);
            update_free(context, true, false);
        }else if(!tried_expand) {
            expand(context, sizeof(size_t) * 3);
            update_free(context, true, true);
        }else {
            // TODO this is a major problem...
            term_write("[update_free] could not find a free block even after join/expand!\n");
        }
    }
}

/**
 * Handles the actual allocation, will also try to join/expand while allocating when an empty block
 * sufficient in size is not found
 */
static void* allocate_internal(mm_context_t* context, size_t size, size_t alignment, bool tried_join, bool tried_expand) {
    mm_block_t* current = context->free;
    bool first = true;
    while(current != NULL) {
        if(!first && current == context->free) {
            // No free block found...
            break;
        }
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
            update_free(context, tried_join, tried_expand);

            // set as allocated
            current->alignment = alignment;
            current->allocated = true;
            context->used_size += current->size;
            return current->data + get_padding(current, alignment);
        }

        first = false;
        current = current->next;
    }

    if(!tried_join) {
        // first try joining empty blocks
        join(context);
        return allocate_internal(context, size, alignment, true, false);
    }else if(!tried_expand) {
        // joining didn't help, try to expand the heap
        expand(context, size + alignment * 3);
        return allocate_internal(context, size, alignment, true, true);
    }else {
        // We are really out of memory...
        term_write("[allocate_internal] out of memory!\n");
        return NULL;
    }
}

/**
 * Actual implementation of free
 */
static void internal_free(mm_block_t* block) {
    block->alignment = 0;
    block->allocated = false;
}

////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////


void mm_context_init(mm_context_t* context, uintptr_t virtual_start) {
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
}

void* mm_allocate(mm_context_t* context, size_t size) {
    return mm_allocate_aligned(context, size, 8);
}

void* mm_allocate_aligned(mm_context_t* context, size_t size, size_t alignment) {
    return allocate_internal(context, size, alignment, false, false);
}

void mm_free(mm_context_t* context, void* ptr) {
    if((void*)context->last <= ptr || (void*)context->first >= ptr) {
        // outside of this context range
        return;
    }

    mm_block_t* block = get_block_from_ptr(ptr);
    if(block != NULL && block->allocated) {
        internal_free(block);
    }
}

void* mm_reallocate(mm_context_t* context, void* ptr, size_t size) {
    if(ptr == NULL) {
        return mm_allocate(context, size);
    }

    mm_block_t* block = get_block_from_ptr(ptr);
    if(block == NULL || !block->allocated) {
        return mm_allocate(context, size);
    }
    if(block->size <= (ptrdiff_t)size) {
        return ptr;
    }
    // allocate the new block
    size_t s = (size_t) (block->size - get_padding(block, block->alignment));
    void* new = mm_allocate_aligned(context, size, block->alignment);

    // copy the old memory and free the new one
    memcpy(new, ptr, s);
    memset(new + s, 0, size - s);

    // free the block
    internal_free(block);
    return new;
}
