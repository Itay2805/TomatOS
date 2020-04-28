#include <arch/paging.h>
#include <util/except.h>
#include <util/list.h>

#include <string.h>
#include <stdbool.h>
#include <stivale.h>
#include <sync/spinlock.h>
#include <util/def.h>

#include "pmm.h"
#include "vmm.h"

typedef struct mem_entry {
    list_entry_t link;
    bool temp;

    bool allocated;
    uintptr_t start;
    uintptr_t end;
} mem_entry_t;

static list_entry_t mem_map = INIT_LIST_ENTRY(mem_map);
static uintptr_t mem_memory_top = 0;
static uintptr_t memory_base = 0;

#define TEMP_ENTRIES_COUNT 10
static mem_entry_t temp_entries[TEMP_ENTRIES_COUNT];
static size_t temp_entries_count = 0;
static bool freeing = false;

static list_entry_t free_entries_list = INIT_LIST_ENTRY(free_entries_list);

static spinlock_t lock = SPINLOCK_INIT;

////////////////////////////////////////////////////////////////////////
// Memory map manipulation
////////////////////////////////////////////////////////////////////////

// forward declare these
static uintptr_t find_free_pages(uintptr_t max_address, uintptr_t min_address, size_t page_count);
static err_t convert_page(uintptr_t start, size_t page_count, bool new_allocated);

/**
 * Remove the entry from the map
 *
 * will also add it to the free entries list
 * if it is not allocated in the temp entries
 *
 * @param entry [IN] Entry to remove
 */
static void remove_mem_map_entry(mem_entry_t* entry) {
    remove_entry_list(&entry->link);
    entry->link.next = NULL;

    // if this is not a null entry add it to the free entries list
    if(!entry->temp) {
        insert_tail_list(&free_entries_list, &entry->link);
    }
}

/**
 * Add a range of entries to the map
 *
 * @param allocated     [IN] Is the range allocated or not
 * @param start         [IN] The start of the range
 * @param end           [IN] The end of the range
 */
static void add_range(bool allocated, uintptr_t start, uintptr_t end) {
    ASSERT((start & PAGE_MASK) == 0);
    ASSERT(end > start);

    // see if can merge entries
    list_entry_t* link = mem_map.next;
    while(link != &mem_map) {
        mem_entry_t* entry = CR(link, mem_entry_t, link);
        link = link->next;

        if(entry->allocated != allocated) {
            continue;
        }

        if(entry->end + 1 == start) {
            start = entry->start;
            remove_mem_map_entry(entry);
        }else if(entry->start == end + 1) {
            end = entry->end;
            remove_mem_map_entry(entry);
        }
    }

    // add the entry
    temp_entries[temp_entries_count].temp = true;
    temp_entries[temp_entries_count].allocated = allocated;
    temp_entries[temp_entries_count].start = start;
    temp_entries[temp_entries_count].end = end;
    insert_tail_list(&mem_map, &temp_entries[temp_entries_count].link);

    temp_entries_count++;
    ASSERT(temp_entries_count < TEMP_ENTRIES_COUNT);
}

/**
 * Allocate memory without freeing the temp allocation list.
 *
 * This is useful when modifying the map and we need more entries for it
 *
 * @param type          [IN]
 * @param page_count    [IN]
 *
 * @return Direct physical address
 */
static err_t allocate_pool_pages(size_t page_count, void** address) {
    err_t err = NO_ERROR;

    uintptr_t start = find_free_pages(mem_memory_top, 0, page_count);
    CHECK_ERROR(start != 0, ERROR_OUT_OF_RESOURCES);
    CHECK_AND_RETHROW(convert_page(start, page_count, true));

    *address = (void*)(start + memory_base);

cleanup:
    return err;
}

/**
 * Allocate a memory map entry
 *
 * This will first attempt to use one which is already free
 * and if none are found it will allocate more
 *
 * @return Map entry
 */
static err_t allocate_memory_map_entry(mem_entry_t** entry) {
    err_t err = NO_ERROR;

    if(is_list_empty(&free_entries_list)) {
        mem_entry_t* free_descriptor_entries = NULL;
        CHECK_AND_RETHROW(allocate_pool_pages(1, (void**)&free_descriptor_entries));

        for(int i = 0; i < PAGE_SIZE / sizeof(mem_entry_t); i++) {
            insert_tail_list(&free_entries_list, &free_descriptor_entries[i].link);
        }
    }

    *entry = CR(free_entries_list.next, mem_entry_t, link);
    remove_entry_list(&(*entry)->link);

cleanup:
    return err;
}

/**
 * This will move the temp entries from the temp storage
 * into the main memory
 */
static err_t move_temp_entries() {
    err_t err = NO_ERROR;

    // no recursion please
    if(freeing) {
        goto cleanup;
    }
    freeing = true;

    while(temp_entries_count != 0) {
        mem_entry_t* entry = NULL;
        CHECK_AND_RETHROW(allocate_memory_map_entry(&entry));

        temp_entries_count -= 1;

        if(temp_entries[temp_entries_count].link.next != NULL) {
            // move entry to global memory
            remove_entry_list(&temp_entries[temp_entries_count].link);
            temp_entries[temp_entries_count].link.next = NULL;

            memcpy(entry, &temp_entries[temp_entries_count], sizeof(mem_entry_t));
            entry->temp = false;

            // find insertion location
            list_entry_t* link = NULL;
            for(link = mem_map.next; link != &mem_map; link = link->next) {
                mem_entry_t* entry2 = CR(link, mem_entry_t, link);
                if(!entry2->temp && entry2->start > entry->start) {
                    break;
                }
            }

            insert_tail_list(link, &entry->link);
        }else {
            // already dequeued
            insert_tail_list(&free_entries_list, &entry->link);
        }
    }

cleanup:
    freeing = false;
    return err;
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
static uintptr_t find_free_pages(uintptr_t max_address, uintptr_t min_address, size_t page_count) {
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

    for(list_entry_t* link = mem_map.next; link != &mem_map; link = link->next) {
        mem_entry_t* entry = CR(link, mem_entry_t, link);

        // only check free entries
        if(entry->allocated) {
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
        desc_end = ((desc_end + 1u) & (~(PAGE_SIZE - 1u))) - 1u;

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
 * @param new_type      [IN] The new type
 *
 * @retval ERROR_NOT_FOUND - Could not find the range where this page is in
 */
static err_t convert_page(uintptr_t start, size_t page_count, bool new_allocated) {
    err_t err = NO_ERROR;
    size_t length = PAGES_TO_SIZE(page_count);
    uintptr_t end = start + length - 1;

    CHECK_ERROR(page_count != 0, ERROR_INVALID_PARAM);
    CHECK_ERROR((start & PAGE_MASK) == 0, ERROR_INVALID_PARAM);
    CHECK_ERROR(end > start, ERROR_INVALID_PARAM);

    while(start < end) {

        // find good entry
        mem_entry_t* entry = NULL;
        list_entry_t* link = NULL;
        for(link = mem_map.next; link != &mem_map; link = link->next) {
            entry = CR(link, mem_entry_t, link);
            if(entry->start <= start && entry->end > start) {
                break;
            }
        }

        CHECK_TRACE(link != &mem_map, "failed to find range");

        if(new_allocated) {
            CHECK_TRACE(entry->end >= end, "range covers multiple entries");
        }

        uintptr_t range_end = end;
        if(entry->end < end) {
            range_end = entry->end;
        }

        if((!new_allocated ? 0u : 1u) ^ (!entry->allocated ? 1u : 0u)) {
            if(!entry->allocated) {
                CHECK_FAIL_TRACE("incompatible memory types, already free");
            }else {
                CHECK_FAIL_TRACE("incompatible memory types, already allocated");
            }
        }

        if(entry->start == start) {
            // clip start
            entry->start = range_end + 1;
        }else if(entry->end == range_end) {
            // clip end
            entry->end = start - 1;
        }else {
            // pull from center

            temp_entries[temp_entries_count].temp = true;
            temp_entries[temp_entries_count].allocated = entry->allocated;
            temp_entries[temp_entries_count].start = range_end + 1;
            temp_entries[temp_entries_count].end = entry->end;

            entry->end = start - 1;
            CHECK(entry->start < entry->end);

            entry = &temp_entries[temp_entries_count];
            insert_tail_list(&mem_map, &entry->link);

            temp_entries_count++;
            CHECK_ERROR(temp_entries_count < TEMP_ENTRIES_COUNT, ERROR_OUT_OF_RESOURCES);
        }

        // if empty remove from map
        if(entry->start == entry->end + 1) {
            remove_mem_map_entry(entry);
            entry = NULL;
        }


        if(new_allocated) {
            add_range(true, start, range_end);
        }

        CHECK_AND_RETHROW(move_temp_entries());

        start = range_end + 1;
    }

cleanup:
    return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// pmm function implementations
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* e820_mmap_names[] = {
    [E820_TYPE_RESERVED] = "Reserved",
    [E820_TYPE_BAD_MEMORY] = "Bad memory",
    [E820_TYPE_ACPI_RECLAIM] = "ACPI Reclaim",
    [E820_TYPE_USABLE] = "Useable",
    [E820_TYPE_ACPI_NVS] = "ACPI NVS",
};

void pmm_init(stivale_struct_t* info) {
    /*
     * NOTE: we assume the bootloader set the range of the kernel
     *       stuff as reserved
     */

    TRACE("Preparing pmm");

    // go over the entries
    e820_entry_t* entry = (e820_entry_t*)info->memory_map_addr;
    for(int i = 0; i < info->memory_map_entries; i++, entry++) {

        if (ARRAY_LEN(e820_mmap_names) > entry->type) {
            TRACE("\t%lx-%lx: (%lx) %s", entry->base, entry->base + entry->length, entry->length, e820_mmap_names[entry->type]);
        } else {
            TRACE("\t%lx-%lx: (%lx) %d", entry->base, entry->base + entry->length, entry->length, entry->type);
        }

        // add useable entries
        if(entry->type == E820_TYPE_USABLE) {
            // set the memory top
            if(mem_memory_top < entry->base + entry->length) {
                mem_memory_top = entry->base + entry->length;
            }

            // clear memory every 5 entries
            if (i != 0 && i % (ARRAY_LEN(temp_entries) / 2) == 0) {
                ASSERT(!IS_ERROR(move_temp_entries()));
            }

            // add the range of addresses
            add_range(false, entry->base, entry->base + entry->length);
        }
    }

    // make sure to clean all temp entries
    ASSERT(!IS_ERROR(move_temp_entries()));
}

void pmm_post_vmm() {
    TRACE("Pmm post vmm setup");

    // set the memory base
    memory_base = DIRECT_MAPPING_BASE;

    // update the mem map
    if(!IS_KERNEL_PTR(mem_map.prev)) {
        mem_map.prev = PHYSICAL_TO_DIRECT(mem_map.prev);
    }

    if(!IS_KERNEL_PTR(mem_map.next)) {
        mem_map.next = PHYSICAL_TO_DIRECT(mem_map.next);
    }

    for(list_entry_t* link = mem_map.next; link != &mem_map; link = link->next) {
        if(!IS_KERNEL_PTR(link->prev)) {
            link->prev = PHYSICAL_TO_DIRECT(link->prev);
        }
        if(!IS_KERNEL_PTR(link->next)) {
            link->next = PHYSICAL_TO_DIRECT(link->next);
        }
    }


    // update the free entries list
    if(!IS_KERNEL_PTR(free_entries_list.prev)) {
        free_entries_list.prev = PHYSICAL_TO_DIRECT(free_entries_list.prev);
    }

    if(!IS_KERNEL_PTR(free_entries_list.next)) {
        free_entries_list.next = PHYSICAL_TO_DIRECT(free_entries_list.next);
    }

    for(list_entry_t* link = free_entries_list.next; link != &free_entries_list; link = link->next) {
        if(!IS_KERNEL_PTR(link->prev)) {
            link->prev = PHYSICAL_TO_DIRECT(link->prev);
        }
        if(!IS_KERNEL_PTR(link->next)) {
            link->next = PHYSICAL_TO_DIRECT(link->next);
        }
    }
}

err_t pmm_allocate_pages(allocate_type_t type, size_t page_count, uintptr_t* base) {
    err_t err = NO_ERROR;

    spinlock_acquire(&lock);

    CHECK_ERROR(type == ALLOCATE_ADDRESS || type == ALLOCATE_ANY || type == ALLOCATE_BELOW, ERROR_INVALID_PARAM);
    CHECK_ERROR(base != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(page_count != 0, ERROR_INVALID_PARAM);

    uintptr_t start = *base;
    uintptr_t end = 0;
    uintptr_t max_address = mem_memory_top;

    if(type == ALLOCATE_ADDRESS) {
        CHECK_ERROR((page_count != 0) && (page_count < SIZE_TO_PAGES(max_address)), ERROR_INVALID_PARAM);

        uintptr_t length = page_count << PAGE_SHIFT;
        end = start + length - 1;

        CHECK_ERROR(start < end, ERROR_INVALID_PARAM);
        CHECK_ERROR(start < max_address, ERROR_INVALID_PARAM);
        CHECK_ERROR(end < max_address, ERROR_INVALID_PARAM);
    }

    if(type == ALLOCATE_BELOW) {
        max_address = start;
    }

    if(type != ALLOCATE_ADDRESS) {
        start = find_free_pages(max_address, 0, page_count);
        CHECK_ERROR(start != 0, ERROR_OUT_OF_RESOURCES);
    }

    CHECK_AND_RETHROW(convert_page(start, page_count, true));

    *base = start;

cleanup:
    spinlock_release(&lock);
    return err;
}

err_t pmm_free_pages(uintptr_t base, size_t page_count) {
    err_t err = NO_ERROR;
    spinlock_acquire(&lock);

    // find the entry
    mem_entry_t* entry = NULL;
    list_entry_t* link = NULL;
    for(link = mem_map.next; link != &mem_map; link = link->next) {
        entry = CR(link, mem_entry_t, link);

        if(entry->start <= base && entry->end > base) {
            break;
        }
    }

    CHECK_ERROR(link != &mem_map, ERROR_INVALID_POINTER);

    CHECK_AND_RETHROW(convert_page(base, page_count, false));

cleanup:
    spinlock_release(&lock);
    return err;
}
