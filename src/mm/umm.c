#include <util/list.h>
#include <arch/paging.h>
#include "vmm.h"
#include "mm.h"

typedef enum vmm_umem_block_type {
    UMEM_FREE,
    UMEM_ALLOCATED,
    // TODO: shared memory
    // TODO: memory mapped file
    // TODO: so on
} vmm_umem_block_type_t;

typedef struct vmm_mem_block {
    vmm_umem_block_type_t type;
    list_entry_t link;
    uintptr_t start;
    uintptr_t end;
} vmm_umem_block_t;

static spinlock_t free_block_list_lock = SPINLOCK_INIT;
static list_entry_t free_block_list = INIT_LIST_ENTRY(free_block_list);

static void vmm_umem_remove_memory_map_entry(vmm_umem_block_t* block) {
    remove_entry_list(&block->link);
    block->link.next = NULL;

    spinlock_acquire(&free_block_list_lock);
    insert_tail_list(&free_block_list, &block->link);
    spinlock_release(&free_block_list_lock);
}

/**
 * Allocate a umem block descriptor thingy
 *
 * We allocate it from a global, shared list of descriptors, just
 * for saving memory on allocations, might be a bit slower because
 * of the locks but lets hope for the best :)
 */
static vmm_umem_block_t* allocate_block() {
    spinlock_acquire(&free_block_list_lock);

    // allocate some entries if we have none
    if (is_list_empty(&free_block_list)) {
        vmm_umem_block_t* base;
        ASSERT(!IS_ERROR(pmm_allocate_pages(ALLOCATE_ANY, 1, (uintptr_t*)&base)));
        base = PHYSICAL_TO_DIRECT(base);

        for (int i = 0; i < PAGE_SIZE / sizeof(vmm_umem_block_t); i++, base++) {
            insert_tail_list(&free_block_list, &base->link);
        }

        ASSERT(!is_list_empty(&free_block_list));
    }

    // pop an available block
    vmm_umem_block_t* block = CR(free_block_list.next, vmm_umem_block_t, link);
    remove_entry_list(&block->link);

    spinlock_release(&free_block_list_lock);

    // return it
    return block;
}

static void vmm_umem_add_range(vmm_handle_t* handle, vmm_umem_block_type_t type, uintptr_t start, uintptr_t end) {
    ASSERT((start & PAGE_MASK) == 0);
    ASSERT(end > start);

    // merge with any blocks that are next to it and have the same type
    FOR_EACH_IN_LIST(handle->umem, link) {
        vmm_umem_block_t* block = CR(link, vmm_umem_block_t, link);

        if (block->type != type) {
            continue;
        }

        if (block->end + 1 == start) {
            start = block->start;
            vmm_umem_remove_memory_map_entry(block);
        } else if (block->start == end + 1) {
            end = block->end;
            vmm_umem_remove_memory_map_entry(block);
        }
    }

    // add it
    vmm_umem_block_t* block = allocate_block();
    block->type = type;
    block->start = start;
    block->end = end;
    insert_tail_list(&handle->umem, &block->link);
}

static uintptr_t vmm_umem_find_free_pages(vmm_handle_t* handle, uintptr_t max_address, uintptr_t min_address, size_t page_count) {
    if(max_address < PAGE_MASK || page_count == 0) {
        return 0;
    }

    // align max address
    if((max_address & PAGE_MASK) != PAGE_MASK) {
        max_address -= (PAGE_MASK + 1);
        max_address &= ~(uintptr_t)PAGE_MASK;
        max_address |= PAGE_MASK;
    }

    size_t length = page_count << PAGE_SHIFT;
    uintptr_t target = 0;

    for(list_entry_t* link = handle->umem.next; link != &handle->umem; link = link->next) {
        vmm_umem_block_t* entry = CR(link, vmm_umem_block_t, link);

        // only check free entries
        if(entry->type != UMEM_FREE) {
            continue;
        }

        uintptr_t desc_start = entry->start;
        uintptr_t desc_end = entry->end;

        // check if a valid range
        if((desc_start >= max_address) || (desc_end < min_address)) {
            continue;
        }

        // clip end
        if(desc_end >= max_address) {
            desc_end = max_address;
        }
        desc_end = ((desc_end + 1lu) & (~(PAGE_SIZE - 1lu))) - 1lu;

        // check the clipped end is good
        if(desc_end < desc_start) {
            continue;
        }

        size_t desc_length = desc_end - desc_start + 1;

        if(desc_length >= length) {
            // make sure the min is good
            if((desc_end - length + 1) < min_address) {
                continue;
            }

            // remember the best choice (prefer higher pages)
            if(desc_end > target) {
                target = desc_end;
            }
        }
    }

    target -= length - 1;

    if((target & PAGE_MASK) != 0) {
        return 0;
    }

    return target;
}

static err_t vmm_umem_convert_page(vmm_handle_t* handle, uintptr_t start, size_t page_count, vmm_umem_block_type_t type) {
    err_t err = NO_ERROR;
    size_t length = PAGES_TO_SIZE(page_count);
    uintptr_t end = start + length - 1;

    ASSERT(page_count != 0);
    ASSERT((start & PAGE_MASK) == 0);
    ASSERT(end > start);

    while(start < end) {

        // find good entry
        vmm_umem_block_t* entry = NULL;
        list_entry_t* link = NULL;
        for(link = handle->umem.next; link != &handle->umem; link = link->next) {
            entry = CR(link, vmm_umem_block_t, link);
            if(entry->start <= start && entry->end > start) {
                break;
            }
        }

        CHECK_ERROR(link != &handle->umem, ERROR_NOT_FOUND);

        if(type != UMEM_FREE) {
            CHECK_ERROR(entry->end >= end, ERROR_NOT_FOUND);
        }

        uintptr_t range_end = end;
        if(entry->end < end) {
            range_end = entry->end;
        }

        if((type != UMEM_FREE ? 0u : 1u) ^ (entry->type != UMEM_FREE ? 1u : 0u)) {
            CHECK_FAIL_ERROR(ERROR_NOT_FOUND);
        }

        if(entry->start == start) {
            // clip start
            entry->start = range_end + 1;
        }else if(entry->end == range_end) {
            // clip end
            entry->end = start - 1;
        }else {
            // pull from center
            vmm_umem_block_t* new_entry = allocate_block();
            new_entry->type = entry->type;
            new_entry->start = range_end + 1;
            new_entry->end = entry->end;

            entry->end = start - 1;
            ASSERT(entry->start < entry->end);

            entry = new_entry;
            insert_tail_list(&handle->umem, &entry->link);
        }

        // if empty remove from map
        if(entry->start == entry->end + 1) {
            vmm_umem_remove_memory_map_entry(entry);
            entry = NULL;
        }


        if(type != UMEM_FREE) {
            vmm_umem_add_range(handle, type, start, range_end);
        }

        start = range_end + 1;
    }

cleanup:
    // TODO: revert or something?
    return err;
}

err_t vmm_user_allocate(vmm_handle_t* handle, allocate_type_t type, uintptr_t* virt, size_t page_count, page_perms_t perms) {
    err_t err = NO_ERROR;

    CHECK_ERROR(handle != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(page_count != 0, ERROR_INVALID_PARAM);

    spinlock_acquire(&handle->lock);

    uintptr_t start = *virt;
    uintptr_t end = 0;
    uintptr_t max_address = USERSPACE_END;

    if(type == ALLOCATE_ADDRESS) {
        CHECK((page_count != 0) && (page_count < SIZE_TO_PAGES(max_address)));

        uintptr_t length = page_count << PAGE_SHIFT;
        end = start + length - 1;

        CHECK(start < end);
        CHECK(start < max_address);
        CHECK(end < max_address);
    }

    if (type == ALLOCATE_BELOW) {
        max_address = start;
    }

    if(type != ALLOCATE_ADDRESS) {
        start = vmm_umem_find_free_pages(handle, max_address, 0, page_count);
        CHECK_ERROR(start != 0, ERROR_OUT_OF_RESOURCES);
    }

    CHECK_AND_RETHROW(vmm_umem_convert_page(handle, start, page_count, true));

    *virt = start;

    // now do the actual mapping
    uintptr_t current_va = start;
    while(current_va < start + PAGES_TO_SIZE(page_count)) {
        uintptr_t addr;
        ASSERT(!IS_ERROR(pmm_allocate_pages(ALLOCATE_ANY, 1, &addr)));
        vmm_map(handle, addr, current_va, PAGE_SIZE, perms, DEFAULT_CACHE);
        current_va += PAGE_SIZE;
    }

cleanup:
    spinlock_release(&handle->lock);
    return err;
}

err_t vmm_user_free(vmm_handle_t* handle, uintptr_t virt, size_t page_count) {
    err_t err = NO_ERROR;

    spinlock_acquire(&handle->lock);

    // find the entry
    vmm_umem_block_t* entry = NULL;
    list_entry_t* link;
    for(link = handle->umem.next; link != &handle->umem; link = link->next) {
        entry = CR(link, vmm_umem_block_t, link);

        if(entry->start <= virt && entry->end > virt) {
            break;
        }
    }
    CHECK_ERROR(link != &handle->umem, ERROR_NOT_FOUND);

    CHECK_AND_RETHROW(vmm_umem_convert_page(handle, virt, page_count, false));

    // now do the actual unmapping
    if (entry->type == UMEM_ALLOCATED) {
        // free and unmap each of the pages
        for (uintptr_t addr = entry->start; addr < entry->end; addr += PAGE_SIZE) {
            uintptr_t physical = 0;
            vmm_virtual_to_physical(handle, addr, &physical, NULL);
            vmm_unmap(handle, addr, PAGE_SIZE);
            pmm_free_pages(physical, 1);
        }
    }

cleanup:
    spinlock_release(&handle->lock);
    return err;
}

void vmm_create_user_mem(vmm_handle_t* handle) {
    handle->umem = INIT_LIST_ENTRY(handle->umem);
    vmm_umem_add_range(handle, UMEM_FREE, PAGE_SIZE, USERSPACE_END);
}

void vmm_destroy_user_mem(vmm_handle_t* handle) {
    // free all the blocks
    vmm_umem_block_t* entry;
    for(list_entry_t* link = handle->umem.next; link != &handle->umem; link = link->next) {
        entry = CR(link, vmm_umem_block_t, link);
        vmm_user_free(handle, entry->start, SIZE_TO_PAGES(entry->end - entry->start));
    }

    // we should be left with exactly one entry that covers the whole memory range
    vmm_umem_remove_memory_map_entry(CR(handle->umem.next, vmm_umem_block_t, link));
    ASSERT(is_list_empty(&handle->umem));
}
