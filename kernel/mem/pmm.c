#include <util/string.h>
#include <util/list.h>
#include <sync/lock.h>
#include <util/defs.h>
#include "pmm.h"
#include "mm.h"

/**
 * The min order needs to fit a single pointer
 * for our linked list, meaning the min is 3
 */
#define MIN_ORDER 3

#define NEXT(ptr) RELAXED_POKE(directptr_t, ptr)

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
     ticket_lock_t lock;
} buddy_t;

/**
 * The low memory buddy (<4GB)
 */
static buddy_t g_low_buddy = {
    .max_order = 32,
    .base = PHYSICAL_TO_DIRECT(0),
    .free_list = (directptr_t[32 - MIN_ORDER]){},
};

/**
 * The high memory buddy allocator (>4GB)
 */
static buddy_t g_high_buddy = {
    .max_order = 64,
    .base = 0, // will be set from the first entry
    .free_list = (directptr_t[64 - MIN_ORDER]){},
};

/**
 * The amount of pages in the system
 */
static size_t g_page_count;

static bool check_buddies(buddy_t* buddy, directptr_t a, directptr_t b, size_t size) {
    uintptr_t lower = MIN(a, b) - buddy->base;
    uintptr_t upper = MAX(a, b) - buddy->base;
    return (lower ^ size) == upper;
}

static void buddy_add_free_item(buddy_t* buddy, directptr_t address, size_t order, bool new) {
    directptr_t head = buddy->free_list[order - MIN_ORDER];
    NEXT(address) = 0;
    size_t size = 1ull << order;

    ticket_lock(&buddy->lock);

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

    ticket_unlock(&buddy->lock);
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
    int original_order = MAX(64 - clz(size - 1), MIN_ORDER);
    size_t want_size = 1ull << original_order;

    // make sure the buddy can actually do that
    if (original_order >= buddy->max_order) {
        WARN(false, "Tried to allocate too much from buddy (requested %zx bytes, order %d, max order %d)", size, original_order, buddy->max_order);
        return NULL;
    }

    // TODO: maybe use a lock per free list
    ticket_lock(&buddy->lock);

    // find the smallest order with space
    for (int order = original_order; order < buddy->max_order; order++) {
        if (buddy->free_list[order - MIN_ORDER] != 0) {
            // pop the head
            directptr_t address = buddy->free_list[order - MIN_ORDER];
            buddy->free_list[order - MIN_ORDER] = NEXT(address);
            ticket_unlock(&buddy->lock);

            // try to free the left overs
            size_t found_size = 1ull << order;
            while (found_size > want_size) {
                found_size >>= 1ull;
                buddy_add_free_item(buddy, address + found_size, 64 - clz(found_size - 1), true);
            }

            // return the allocated address
            return address;
        }
    }

    ticket_unlock(&buddy->lock);

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Top level functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pmm_add_range(directptr_t base, size_t size) {
    if (base < PHYSICAL_TO_DIRECT(BASE_4GB)) {
        buddy_add_range(&g_low_buddy, base, size);
    } else {
        // allocate the high buddy if needed
        if (g_high_buddy.base == NULL) {
            g_high_buddy.base = base;
        }
        buddy_add_range(&g_high_buddy, base, size);
    }

    if (g_page_count < ALIGN_UP((uintptr_t)base + size - DIRECT_MAPPING_BASE, PAGE_SIZE) / PAGE_SIZE) {
        g_page_count = ALIGN_UP((uintptr_t)base + size - DIRECT_MAPPING_BASE, PAGE_SIZE) / PAGE_SIZE;
    }
}

directptr_t pmalloc_low(size_t size) {
    directptr_t ptr = buddy_alloc(&g_low_buddy, size);
    ASSERT_TRACE(ptr != NULL, "Kernel out of low memory");
    return ptr;
}

directptr_t pmalloc(size_t size) {
    directptr_t ptr = NULL;

    // check if has high memory
    if (g_high_buddy.base == 0) {
        ptr = buddy_alloc(&g_high_buddy, size);
    }

    // if still null try to allocate low memory
    if (ptr == NULL) {
        ptr = buddy_alloc(&g_low_buddy, size);
    }

    // TODO: trigger a full on cache flush
    //       and try again
    ASSERT_TRACE(ptr != NULL, "Kernel out of memory");

    return ptr;
}

directptr_t pmallocz(size_t size) {
    directptr_t ptr = pmalloc(size);
    memset(ptr, 0, size);
    return ptr;
}

directptr_t pmallocz_low(size_t size) {
    directptr_t ptr = pmalloc_low(size);
    memset(ptr, 0, size);
    return ptr;
}

void pmfree(directptr_t ptr, size_t size) {
    ASSERT(ptr >= (directptr_t)DIRECT_MAPPING_BASE);

    // figure which of the buddies to use
    buddy_t* buddy;
    if (ptr < PHYSICAL_TO_DIRECT(BASE_4GB)) {
        buddy = &g_low_buddy;
    } else {
        buddy = &g_high_buddy;
    }

    // get the order and add it
    int order = MAX(64 - clz(size - 1), MIN_ORDER);
    buddy_add_free_item(buddy, ptr, order, false);
}

static _Atomic(uint8_t)* g_page_refcount = NULL;

void pmalloc_page_init() {
    TRACE("Page count %d", g_page_count);
    g_page_refcount = kalloc(sizeof(uint8_t) * g_page_count);
}

directptr_t pmalloc_page() {
    directptr_t page = pmalloc(PAGE_SIZE);
    pmalloc_ref_page(page);
    return page;
}

void pmalloc_ref_page(directptr_t page) {
    g_page_refcount[DIRECT_TO_PHYSICAL(page) >> PAGE_SHIFT]++;
}

void pmfree_page(directptr_t page) {
    if (--g_page_refcount[DIRECT_TO_PHYSICAL(page) >> PAGE_SHIFT] == 0) {
        pmfree(page, PAGE_SIZE);
    }
}
