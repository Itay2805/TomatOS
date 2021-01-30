#include <util/except.h>
#include <cont/list.h>
#include <util/string.h>
#include "pmm.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Code related to the pmm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * The min order needs to fit a single pointer
 * for our linked list, meaning the min is 3
 */
#define MIN_ORDER 3

#define NEXT(ptr) *((directptr_t*)(ptr))

/**
 * Represents a single buddy
 */
typedef struct {
    /**
     * The max ordered supported by this structure
     */
    int max_order;

    /**
     * The base of the buddy
     */
    directptr_t base;

    /**
     * Singly linked list of all free pages in each of the orders
     */
    directptr_t* free_list;

    /**
     * The buddies lock
     */
    lock_t lock;
} buddy_t;

/**
 * The low memory buddy (<4GB)
 */
static buddy_t g_low_buddy = {
    .max_order = 32,
    .base = PHYS_TO_DIRECT(0),
    .free_list = (directptr_t[32 - MIN_ORDER]){},
    .lock = INIT_LOCK(TPL_HIGH_LEVEL)
};

/**
 * The high memory buddy allocator (>4GB)
 */
static buddy_t g_high_buddy = {
    .max_order = 64,
    .base = 0, // will be set from the first entry
    .free_list = (directptr_t[64 - MIN_ORDER]){},
    .lock = INIT_LOCK(TPL_HIGH_LEVEL)
};

/**
 * The amount of pages in the system
 */
static size_t g_highest_address;

static bool check_buddies(buddy_t* buddy, directptr_t a, directptr_t b, size_t size) {
    uintptr_t lower = MIN(a, b) - buddy->base;
    uintptr_t upper = MAX(a, b) - buddy->base;
    return (lower ^ size) == upper;
}

static void buddy_add_free_item(buddy_t* buddy, directptr_t address, size_t order, bool new) {
    directptr_t head = buddy->free_list[order - MIN_ORDER];
    NEXT(address) = 0;
    size_t size = 1ull << order;

    if (!new && head != 0) {
        directptr_t prev = 0;
        while (true) {
            if (check_buddies(buddy, head, address, size)) {
                if (prev != 0) {
                    NEXT(prev) = NEXT(head);
                } else {
                    buddy->free_list[order - MIN_ORDER] = NEXT(head);
                }

                buddy_add_free_item(buddy, MIN(head, address), order + 1, false);
                break;
            }

            if (NEXT(head) == 0) {
                NEXT(head) = address;
                break;
            }

            prev = head;
            head = NEXT(head);
        }
    } else {
        // just put in the head
        NEXT(address) = head;
        buddy->free_list[order - MIN_ORDER] = address;
    }
}

static void buddy_add_free_item_locked(buddy_t* buddy, directptr_t address, size_t order, bool new) {
    acquire_lock(&buddy->lock);
    buddy_add_free_item(buddy, address, order, new);
    release_lock(&buddy->lock);
}

static void buddy_add_range(buddy_t* buddy, directptr_t address, size_t size) {
    // as long as the chunk is big enough to fit
    while (size > (1ull << MIN_ORDER)) {
        // find the largest order and use that
        for (int order = buddy->max_order - 1; order >= MIN_ORDER; order--) {
            if (size >= (1ull << order)) {
                buddy_add_free_item(buddy, address, order, true);
                address += 1ull << order;
                size -= 1ull << order;
                break;
            }
        }
    }
}

static directptr_t buddy_alloc(buddy_t* buddy, size_t size) {
    int original_order = MAX(LOG2(size), MIN_ORDER);
    size_t want_size = 1ull << original_order;

    // make sure the buddy can actually do that
    if (original_order >= buddy->max_order) {
        WARN("Tried to allocate too much from buddy (requested %zx bytes, order %d, max order %d)", size, original_order, buddy->max_order);
        return NULL;
    }

    acquire_lock(&buddy->lock);

    // find the smallest order with space
    for (int order = original_order; order < buddy->max_order; order++) {
        if (buddy->free_list[order - MIN_ORDER] != 0) {
            // pop the head
            directptr_t address = buddy->free_list[order - MIN_ORDER];
            buddy->free_list[order - MIN_ORDER] = NEXT(address);

            // try to free the left overs
            size_t found_size = 1ull << order;
            while (found_size > want_size) {
                found_size >>= 1ull;
                buddy_add_free_item(buddy, address + found_size, LOG2(found_size), true);
            }

            // return the allocated address
            release_lock(&buddy->lock);
            return address;
        }
    }

    release_lock(&buddy->lock);
    return NULL;
}

directptr_t palloc(size_t size) {
    directptr_t ptr = NULL;

    // check if has high memory
    if (g_high_buddy.base == 0) {
        ptr = buddy_alloc(&g_high_buddy, size);
    }

    // if still null try to allocate low memory
    if (ptr == NULL) {
        ptr = buddy_alloc(&g_low_buddy, size);
    }

    return ptr;
}

directptr_t palloc_low(size_t size) {
    return buddy_alloc(&g_low_buddy, size);
}

void pfree(directptr_t ptr, size_t size) {
    // figure which of the buddies to use
    buddy_t* buddy;
    if (DIRECT_TO_PHYS(ptr) < BASE_4GB) {
        buddy = &g_low_buddy;
    } else {
        buddy = &g_high_buddy;
    }

    // get the order and add it
    int order = MAX(LOG2(size), MIN_ORDER);
    buddy_add_free_item_locked(buddy, ptr, order, false);
}

directptr_t pallocz(size_t size) {
    void* ptr = palloc(size);
    memset(ptr, 0, size);
    return ptr;
}

directptr_t pallocz_low(size_t size) {
    void* ptr = palloc_low(size);
    memset(ptr, 0, size);
    return ptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Code related to initializing the pmm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* stivale2_type_to_str(int type) {
    switch (type) {
        case STIVALE2_MMAP_USABLE: return "Usable";
        case STIVALE2_MMAP_RESERVED: return "Reserved";
        case STIVALE2_MMAP_ACPI_RECLAIMABLE: return "ACPI Reclaimable";
        case STIVALE2_MMAP_ACPI_NVS: return "ACPI NVS";
        case STIVALE2_MMAP_BAD_MEMORY: return "Bad Memory";
        case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE: return "Bootloader Reclaimable";
        case STIVALE2_MMAP_KERNEL_AND_MODULES: return "Kernel and Modules";
        default: return NULL;
    }
}

static void pmm_add_range(physptr_t base, size_t size) {
    if (base < BASE_4GB) {
        buddy_add_range(&g_low_buddy, PHYS_TO_DIRECT(base), size);
    } else {
        // allocate the high buddy if needed
        if (g_high_buddy.base == NULL) {
            g_high_buddy.base = PHYS_TO_DIRECT(base);
        }
        buddy_add_range(&g_high_buddy, PHYS_TO_DIRECT(base), size);
    }
}

err_t init_pmm() {
    err_t err = NO_ERROR;

    UNLOCKED_TRACE("Initializing pmm");

    stivale2_struct_tag_memmap_t* memap = get_stivale2_tag(STIVALE2_STRUCT_TAG_MEMMAP_ID);
    CHECK(memap != NULL);

    // calculate the max allocation size
    g_highest_address = 0;
    for (int i = 0; i < memap->entries; i++) {
        stivale2_mmap_entry_t* entry = &memap->memmap[i];
        if (entry->type == STIVALE2_MMAP_USABLE) {
            uintptr_t top_addr = entry->base + entry->length;
            if (top_addr > g_highest_address) {
                g_highest_address = top_addr;
            }
        }
    }

    // go over all the entries and put them correctly in the bitmap
    for (int i = 0; i < memap->entries; i++) {
        stivale2_mmap_entry_t* entry = &memap->memmap[i];

        // trace nicely
        const char* type_str = stivale2_type_to_str(entry->type);
        if (type_str != NULL) {
            TRACE("\t%016p-%016p: %s", entry->base, entry->base + entry->length + entry->unused, type_str);
        } else {
            TRACE("\t%016p-%016p: %d", entry->base, entry->base + entry->length + entry->unused, entry->type);
        }

        // if the range is free add it to our memory map
        if (entry->type == STIVALE2_MMAP_USABLE) {
            pmm_add_range(entry->base, entry->length);
        }
    }

cleanup:
    return err;
}


directptr_t page_alloc() {
    return palloc(PAGE_SIZE);
}

void page_free(directptr_t ptr) {
    pfree(ptr, PAGE_SIZE);
}
