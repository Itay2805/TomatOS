#include <util/list_entry.hpp>
#include <util/debug.h>

#include <arch/paging.h>

#include <tboot.h>
#include <util/spinlock.hpp>
#include <util/critical_section.hpp>
#include "pmm.hpp"

#if 0
#define DEBUG(fmt, ...) debug_log("[*] PMM_DEBUG: %s:%d: " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define DEBUG(...)
#endif

using namespace util;

struct mem_entry {
    list_entry link;
    bool temp;

    bool allocated;
    uintptr_t start;
    uintptr_t end;
};

static list_entry mem_map = { &mem_map, &mem_map };
static uintptr_t mem_memory_top = 0;
static uintptr_t memory_base = 0;

#define TEMP_ENTRIES_COUNT 10
static mem_entry temp_entries[TEMP_ENTRIES_COUNT];
static size_t temp_entries_count = 0;
static bool freeing = false;

static list_entry free_entries_list = { &free_entries_list, &free_entries_list };

static spinlock pmm_lock;

////////////////////////////////////////////////////////////////////////
// Memory map manipulation
////////////////////////////////////////////////////////////////////////

// forward declare these
static uintptr_t find_free_pages(uintptr_t max_address, uintptr_t min_address, size_t page_count);
static void convert_page(uintptr_t start, size_t page_count, bool new_allocated);

/**
 * Remove the entry from the map
 *
 * will also add it to the free entries list
 * if it is not allocated in the temp entries
 *
 * @param entry [IN] Entry to remove
 */
static void remove_mem_map_entry(mem_entry* entry) {
    entry->link.remove();
    entry->link.next = NULL;

    // if this is not a null entry add it to the free entries list
    if(!entry->temp) {
        free_entries_list.insert_tail(&entry->link);
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
    DEBUG("%lx-%lx", start, end);

    ASSERT((start & arch::paging::page_mask) == 0);
    ASSERT(end > start);

    // see if can merge entries
    list_entry* link = mem_map.next;
    while(link != &mem_map) {
        mem_entry* entry = CR(link, mem_entry, link);
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
    mem_map.insert_tail(&temp_entries[temp_entries_count].link);

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
static void* allocate_pool_pages(size_t page_count) {
    uintptr_t start = find_free_pages(mem_memory_top, 0, page_count);
    if(start == 0) {
        DEBUG("failed to allocate %d pages", page_count);
    }else {
        convert_page(start, page_count, true);
    }

    return (void*)(start + memory_base);
}

/**
 * Allocate a memory map entry
 *
 * This will first attempt to use one which is already free
 * and if none are found it will allocate more
 *
 * @return Map entry
 */
static mem_entry* allocate_memory_map_entry() {
    if(free_entries_list.is_empty()) {
        mem_entry* free_descriptor_entries = (mem_entry*)allocate_pool_pages(1);

        if(free_descriptor_entries != NULL) {
            for(int i = 0; i < arch::paging::page_size / sizeof(mem_entry); i++) {
                free_entries_list.insert_tail(&free_descriptor_entries[i].link);
            }
        }else {
            return NULL;
        }
    }

    mem_entry* entry = CR(free_entries_list.next, mem_entry, link);
    entry->link.remove();

    return entry;
}

/**
 * This will move the temp entries from the temp storage
 * into the main memory
 */
static void move_temp_entries() {
    if(freeing) {
        return;
    }

    freeing = true;

    while(temp_entries_count != 0) {
        mem_entry* entry = allocate_memory_map_entry();
        ASSERT(entry);

        temp_entries_count -= 1;

        if(temp_entries[temp_entries_count].link.next != NULL) {
            // move entry to global memory
            temp_entries[temp_entries_count].link.remove();
            temp_entries[temp_entries_count].link.next = NULL;

            *entry = temp_entries[temp_entries_count];
            entry->temp = false;

            // find insertion location
            list_entry* link = NULL;
            for(link = mem_map.next; link != &mem_map; link = link->next) {
                mem_entry* entry2 = CR(link, mem_entry, link);
                if(!entry2->temp && entry2->start > entry->start) {
                    break;
                }
            }

            link->insert_tail(&entry->link);
        }else {
            // already dequeued
            free_entries_list.insert_tail(&entry->link);
        }
    }

    freeing = false;
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
    if(max_address < arch::paging::page_mask || page_count == 0) {
        return 0;
    }

    // align max address
    if((max_address & arch::paging::page_mask) != arch::paging::page_mask) {
        max_address -= (arch::paging::page_mask + 1);
        max_address &= ~(uintptr_t)arch::paging::page_mask;
        max_address |= arch::paging::page_mask;
    }

    size_t length = page_count << arch::paging::page_shift;
    uintptr_t target = 0;

    for(list_entry* link = mem_map.next; link != &mem_map; link = link->next) {
        mem_entry* entry = CR(link, mem_entry, link);

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
        desc_end = ((desc_end + 1u) & (~(arch::paging::page_size - 1u))) - 1u;

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

    if((target & arch::paging::page_mask) != 0) {
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
static void convert_page(uintptr_t start, size_t page_count, bool new_allocated) {
    size_t length = arch::paging::pages_to_size(page_count);
    uintptr_t end = start + length - 1;

    ASSERT(page_count != 0);
    ASSERT((start & arch::paging::page_mask) == 0);
    ASSERT(end > start);

    while(start < end) {

        // find good entry
        mem_entry* entry = NULL;
        list_entry* link = NULL;
        for(link = mem_map.next; link != &mem_map; link = link->next) {
            entry = CR(link, mem_entry, link);
            if(entry->start <= start && entry->end > start) {
                break;
            }
        }

        if(link == &mem_map) {
            DEBUG("failed to find range 0x%lx-0x%lx", start, end);
            ASSERT(false);
        }

        if(new_allocated) {
            if(entry->end < end) {
                DEBUG("range 0x%lx-0x%lx covers multiple entries", start, end);
                ASSERT(false);
            }
        }

        uintptr_t range_end = end;
        if(entry->end < end) {
            range_end = entry->end;
        }

        DEBUG("%lx-%lx to %s", start, range_end, new_allocated ? "allocated" : "free");

        if((!new_allocated ? 0u : 1u) ^ (!entry->allocated ? 1u : 0u)) {
            if(!entry->allocated) {
                DEBUG("incompatible memory types, already free");
            }else {
                DEBUG("incompatible memory types, already allocated");
            }
            ASSERT(false);
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
            ASSERT(entry->start < entry->end);

            entry = &temp_entries[temp_entries_count];
            mem_map.insert_tail(&entry->link);

            temp_entries_count++;
            ASSERT(temp_entries_count < TEMP_ENTRIES_COUNT);
        }

        // if empty remove from map
        if(entry->start == entry->end + 1) {
            remove_mem_map_entry(entry);
            entry = NULL;
        }


        if(new_allocated) {
            add_range(true, start, range_end);
        }

        move_temp_entries();

        start = range_end + 1;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// pmm function implementations
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* tboot_mmap_names[] = {
    [TBOOT_MEMORY_TYPE_RESERVED] = "Reserved",
    [TBOOT_MEMORY_TYPE_BAD_MEMORY] = "Bad memory",
    [TBOOT_MEMORY_TYPE_ACPI_RECLAIM] = "ACPI Reclaim",
    [TBOOT_MEMORY_TYPE_USABLE] = "Useable",
    [TBOOT_MEMORY_TYPE_ACPI_NVS] = "ACPI NVS"
};

namespace mem::pmm {

    void init(tboot_info_t* info) {
        /*
         * NOTE: we assume the bootloader set the range of the kernel
         *       stuff as reserved
         */

        debug_log("[+] Preparing pmm\n");

        // go over the entries
        for(int i = 0; i < info->mmap.count; i++) {
            tboot_mmap_entry_t* entry = &info->mmap.entries[i];
            debug_log("[*] \t%p-%p: (%lx) %s\n", entry->addr, entry->addr + entry->len, entry->len, tboot_mmap_names[entry->type]);

            // add useable entries
            if(entry->type == TBOOT_MEMORY_TYPE_USABLE) {
                // set the memory top
                if(mem_memory_top < entry->addr + entry->len) {
                    mem_memory_top = entry->addr + entry->len;
                }

                // add the range of addresses
                add_range(false, entry->addr, entry->addr + entry->len);
                move_temp_entries();
            }

        }
    }

    void post_vmm_init() {
        debug_log("[*] Pmm post vmm setup\n");
    }

//    void post_vmm_init() {
//        debug_log("[*] Pmm post vmm setup\n");
//
//        // set the memory base
//        memory_base = DIRECT_MAPPING_BASE;
//
//        // update the mem map
//        if(!IS_KERNEL_PTR(mem_map.prev)) {
//            mem_map.prev = PHYSICAL_TO_DIRECT(mem_map.prev);
//        }
//
//        if(!IS_KERNEL_PTR(mem_map.next)) {
//            mem_map.next = PHYSICAL_TO_DIRECT(mem_map.next);
//        }
//
//        for(list_entry_t* link = mem_map.next; link != &mem_map; link = link->next) {
//            if(!IS_KERNEL_PTR(link->prev)) {
//                link->prev = PHYSICAL_TO_DIRECT(link->prev);
//            }
//            if(!IS_KERNEL_PTR(link->next)) {
//                link->next = PHYSICAL_TO_DIRECT(link->next);
//            }
//        }
//
//
//        // update the free entries list
//        if(!IS_KERNEL_PTR(free_entries_list.prev)) {
//            free_entries_list.prev = PHYSICAL_TO_DIRECT(free_entries_list.prev);
//        }
//
//        if(!IS_KERNEL_PTR(free_entries_list.next)) {
//            free_entries_list.next = PHYSICAL_TO_DIRECT(free_entries_list.next);
//        }
//
//        for(list_entry_t* link = free_entries_list.next; link != &free_entries_list; link = link->next) {
//            if(!IS_KERNEL_PTR(link->prev)) {
//                link->prev = PHYSICAL_TO_DIRECT(link->prev);
//            }
//            if(!IS_KERNEL_PTR(link->next)) {
//                link->next = PHYSICAL_TO_DIRECT(link->next);
//            }
//        }
//    }

    uintptr_t allocate_pages(allocate_type type, size_t page_count, uintptr_t base) {
        ASSERT(type == ALLOCATE_ADDRESS || type == ALLOCATE_ANY || type == ALLOCATE_BELOW);
        ASSERT(base != NULL);
        ASSERT(page_count != 0);

        util::critical_section section(pmm_lock);

        uintptr_t start = base;
        uintptr_t end = 0;
        uintptr_t max_address = mem_memory_top;

        if(type == ALLOCATE_ADDRESS) {
            ASSERT((page_count != 0) && (page_count < arch::paging::size_to_pages(max_address)));

            uintptr_t length = page_count << arch::paging::page_shift;
            end = start + length - 1;

            ASSERT(start < end);
            ASSERT(start < max_address);
            ASSERT(end < max_address);
        }

        if(type == ALLOCATE_BELOW) {
            max_address = start;
        }

        if(type != ALLOCATE_ADDRESS) {
            start = find_free_pages(max_address, 0, page_count);
            ASSERT(start != 0);
        }

        convert_page(start, page_count, true);

        return start;
    }

    void free_pages(uintptr_t base, size_t page_count) {
        util::critical_section section(pmm_lock);

        // find the entry
        mem_entry* entry;
        list_entry* link;
        for(link = mem_map.next; link != &mem_map; link = link->next) {
            entry = CR(link, mem_entry, link);

            if(entry->start <= base && entry->end > base) {
                break;
            }
        }

        ASSERT(link != &mem_map);

        convert_page(base, page_count, false);
    }

}
