#include "vmm.h"
#include "pmm.h"

typedef struct map_entry {
    list_entry_t link;

    bool allocated;
    uintptr_t start;
    uintptr_t end;
} map_entry_t;

static lock_t  free_entries_lock;
static list_entry_t free_entries_list = { &free_entries_list, &free_entries_list };

////////////////////////////////////////////////////////////////////////
// Memory map manipulation
////////////////////////////////////////////////////////////////////////

static uintptr_t find_free_pages(vmm_handle_t* handle, uintptr_t max_address, uintptr_t min_address, size_t page_count);
static error_t convert_page(vmm_handle_t* handle, uintptr_t start, size_t page_count, memory_type_t new_allocated);

/**
 * Remove the entry from the map
 *
 * @param entry [IN] Entry to remove
 */
static void remove_mem_map_entry(vmm_handle_t* handle, map_entry_t* entry) {
    remove_entry_list(&entry->link);
    entry->link.next = NULL;

    aquire_lock(&free_entries_lock);
    insert_tail_list(&free_entries_list, &entry->link);
    release_lock(&free_entries_lock);
}

/**
 * Allocate a memory map entry
 *
 * This will first attempt to use one which is already free
 * and if none are found it will allocate more
 *
 * @return Map entry
 */
static map_entry_t* allocate_memory_map_entry() {
    aquire_lock(&free_entries_lock);
    if(is_list_empty(&free_entries_list)) {
        map_entry_t* free_descriptor_entries = NULL;
        pmm_allocate_pages(ALLOCATE_ANY, MEM_VMM, 1, (uintptr_t*)&free_descriptor_entries);

        if(free_descriptor_entries != NULL) {
            for(int i = 0; i < PAGE_SIZE / sizeof(map_entry_t); i++) {
                insert_tail_list(&free_entries_list, &free_descriptor_entries[i].link);
            }
        }else {
            release_lock(&free_entries_lock);
            return NULL;
        }
    }
    release_lock(&free_entries_lock);

    map_entry_t* entry = CR(free_entries_list.next, map_entry_t, link);
    remove_entry_list(&entry->link);

    return entry;
}

/**
 * Add a range of entries to the map
 *
 * @param type  [IN] The type of the range
 * @param start [IN] The start of the range
 * @param end   [IN] The end of the range
 */
static void add_range(vmm_handle_t* handle, bool allocated, uintptr_t start, uintptr_t end) {
    ASSERT((start & PAGE_MASK) == 0);
    ASSERT(end > start);

    // see if can merge entries
    list_entry_t* link = handle->map.next;
    while(link != &handle->map) {
        map_entry_t* entry = CR(link, map_entry_t, link);
        link = link->next;

        if(entry->allocated != allocated) {
            continue;
        }

        if(entry->end + 1 == start) {
            start = entry->start;
            remove_mem_map_entry(handle, entry);
        }else if(entry->start == end + 1) {
            end = entry->end;
            remove_mem_map_entry(handle, entry);
        }
    }

    // add the entry
    map_entry_t* entry = allocate_memory_map_entry();
    entry->allocated = allocated;
    entry->start = start;
    entry->end = end;
    insert_tail_list(&handle->map, &entry->link);
}


/**
 * Find a range of free pages which fits the constraints given
 *
 * @param max_address   [IN] The top most address can allocate
 * @param min_address   [IN] The bottom most address can allocate
 * @param page_count    [IN] The amount of pages to allocate
 *
 * @return The base address if found
 */
static uintptr_t find_free_pages(vmm_handle_t* handle, uintptr_t max_address, uintptr_t min_address, size_t page_count) {
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

    for(list_entry_t* link = handle->map.next; link != &handle->map; link = link->next) {
        map_entry_t* entry = CR(link, map_entry_t, link);

        // only check free entries
        if(!entry->allocated) {
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
        desc_end = ((desc_end + 1) & (~(PAGE_SIZE - 1))) - 1;

        // check the clipped end is good
        if(desc_end < desc_start) {
            continue;
        }

        size_t desc_length = desc_end - desc_start + 1;

        if(desc_length > length) {
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

/**
 * Will convert the pages to a certain memory type
 *
 * @param start         [IN] Base address
 * @param page_count    [IN] Amount of pages to convert
 * @param new_allocated      [IN] The new type
 *
 * @retval ERROR_NOT_FOUND - Could not find the range where this page is in
 */
static error_t convert_page(vmm_handle_t* handle, uintptr_t start, size_t page_count, memory_type_t new_allocated) {
    error_t err = NO_ERROR;
    size_t length = PAGES_TO_SIZE(page_count);
    uintptr_t end = start + length - 1;

    ASSERT(page_count != 0);
    ASSERT((start & PAGE_MASK) == 0);
    ASSERT(end > start);

    while(start < end) {

        // find good entry
        map_entry_t* entry = NULL;
        list_entry_t* link = NULL;
        for(link = handle->map.next; link != &handle->map; link = link->next) {
            entry = CR(link, map_entry_t, link);
            if(entry->start <= start && entry->end > start) {
                break;
            }
        }

        CHECK_ERROR(link != &handle->map, ERROR_NOT_FOUND);

        if(!new_allocated) {
            CHECK_ERROR(entry->end >= end, ERROR_NOT_FOUND);
        }

        uintptr_t range_end = end;
        if(entry->end < end) {
            range_end = entry->end;
        }

        if((new_allocated ? 0 : 1) ^ (entry->allocated ? 1 : 0)) {
//            if(entry->allocated) {
//                DEBUG("incompatible memory types, already free");
//            }else {
//                DEBUG("incompatible memory types, already allocated");
//            }
            CHECK_FAILED_ERROR(ERROR_NOT_FOUND);
        }

        if(entry->start == start) {
            // clip start
            entry->start = range_end + 1;
        }else if(entry->end == range_end) {
            // clip end
            entry->end = start - 1;
        }else {
            // pull from center

            map_entry_t* new_entry = allocate_memory_map_entry();
            new_entry->allocated = entry->allocated;
            new_entry->start = range_end + 1;
            new_entry->end = entry->end;

            entry->end = start - 1;
            ASSERT(entry->start < entry->end);

            entry = new_entry;
            insert_tail_list(&handle->map, &entry->link);
        }

        // if empty remove from map
        if(entry->start == entry->end + 1) {
            remove_mem_map_entry(handle, entry);
            entry = NULL;
        }


        if(new_allocated != MEM_FREE) {
            add_range(handle, new_allocated, start, range_end);
        }

        start = range_end + 1;
    }

cleanup:
    // TODO: revert or something?
    return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// vmm userspace function implementations
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

error_t vmm_allocate(vmm_handle_t* handle, uintptr_t* virt, size_t size, page_perms_t perms) {
    error_t err = NO_ERROR;

    ASSERT(handle != NULL);
    ASSERT(size != 0);

    aquire_lock(&handle->lock);

    uintptr_t start = *virt;
    uintptr_t end = 0;
    uintptr_t max_address = 0x00007FFFFFFFFFFF;
    uintptr_t page_count = SIZE_TO_PAGES(size);

    if(*virt != 0) {
        ASSERT((page_count != 0) && (page_count < SIZE_TO_PAGES(max_address)));

        uintptr_t length = page_count << PAGE_SHIFT;
        end = start + length - 1;

        ASSERT(start < end);
        ASSERT(start < max_address);
        ASSERT(end < max_address);
    }

    if(*virt == 0) {
        start = find_free_pages(handle, max_address, 0, page_count);
        CHECK_ERROR(start != 0, ERROR_OUT_OF_RESOURCES);
    }

    CHECK_AND_RETHROW(convert_page(handle, start, page_count, true));

    *virt = start;

    // now do the actual mapping
    uintptr_t current_va = start;
    while(current_va < start + size) {
        uintptr_t addr;
        pmm_allocate_pages(ALLOCATE_ANY, MEM_OTHER, 1, &addr);
        vmm_map(handle, addr, current_va, PAGE_SIZE, perms, DEFAULT_CACHE);
        current_va += PAGE_SIZE;
    }

cleanup:
    release_lock(&handle->lock);
    return err;
}

error_t vmm_free(vmm_handle_t* handle, uintptr_t virt, size_t size) {
    error_t err = NO_ERROR;

    aquire_lock(&handle->lock);

    // find the entry
    map_entry_t* entry;
    list_entry_t* link;
    for(link = handle->map.next; link != &handle->map; link = link->next) {
        entry = CR(link, map_entry_t, link);

        if(entry->start <= virt && entry->end > virt) {
            break;
        }
    }

    CHECK_ERROR(link != &handle->map, ERROR_NOT_FOUND);

    CHECK_AND_RETHROW(convert_page(handle, virt, SIZE_TO_PAGES(size), MEM_FREE));

cleanup:
    release_lock(&handle->lock);
    return err;
}
