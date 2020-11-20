#include <util/except.h>
#include <util/string.h>
#include <stdatomic.h>
#include <cont/slist.h>
#include <sync/lock.h>
#include "pmm.h"

static size_t g_total_pages = 0;
static size_t g_free_pages = 0;
static slist_entry_t g_freelist = INIT_SLIST;

typedef struct pages {
    slist_entry_t link;
    size_t page_count;
} pages_t;

static lock_t g_pmm_lock = INIT_LOCK();

static const char* memmap_type_to_string(int type) {
    switch (type) {
        case STIVALE2_MMAP_TYPE_BAD_MEMORY: return "Bad memory";
        case STIVALE2_MMAP_TYPE_USABLE: return "Usable";
        case STIVALE2_MMAP_TYPE_ACPI_NVS: return "ACPI NVS";
        case STIVALE2_MMAP_TYPE_ACPI_RECLAIMABLE: return "ACPI reclaimable";
        case STIVALE2_MMAP_TYPE_BOOTLOADER_RECLAIMABLE: return "Bootloader reclaimable";
        case STIVALE2_MMAP_TYPE_KERNEL_AND_MODULES: return "Kernel and modules";
        case STIVALE2_MMAP_TYPE_RESERVED: return "Reserved";
        default: return "unknown";
    }
}

void init_pmm(stivale2_struct_tag_memmap_t* memap) {
    TRACE("Initializing pmm");

    for (size_t i = 0; i < memap->entries; i++) {
        stivale2_mmap_entry_t* entry = &memap->memmap[i];
        TRACE("\t", (void*)entry->base," - ", (void*)(entry->base + entry->length), ": ", memmap_type_to_string(entry->type));
        if (entry->type == STIVALE2_MMAP_TYPE_USABLE) {
            // add the range of pages
            pages_t* base = PHYS_TO_DIRECT(entry->base);
            base->page_count = entry->length / PAGE_SIZE;
            g_total_pages += base->page_count;
            slist_push(&g_freelist, &base->link);
        }
    }

    g_free_pages = g_total_pages;
    TRACE("\tAvailable memory: ", SIZE(g_total_pages * PAGE_SIZE));
}

directptr_t page_alloc() {
    irq_lock(&g_pmm_lock);

    // make sure has pages
    ASSERT(!slist_empty(&g_freelist), "Out of memory!");

    // pop the page range, get the last page, and decrement
    // the page count
    pages_t* page = CR(slist_pop(&g_freelist), pages_t, link);
    page->page_count--;
    directptr_t res = (directptr_t)page + PAGE_SIZE * page->page_count;

    // if has more pages return
    if (page->page_count != 0) {
        slist_push(&g_freelist, &page->link);
    }

    irq_unlock(&g_pmm_lock);
    return res;
}

void page_free(directptr_t page) {
    uintptr_t index = (uintptr_t)ALIGN_DOWN(page, PAGE_SIZE) / PAGE_SIZE;
    _Atomic(uint16_t)* refcount = &((_Atomic(uint16_t)*)PAGE_REFCOUNT_START)[index];

    // TODO: too lazy to figure how to do this correctly lol
    if (atomic_fetch_sub(refcount, 1) == 0) {
        atomic_store(refcount, 0);

        // this is the last ref, free it
        irq_lock(&g_pmm_lock);
        slist_push(&g_freelist, page);
        irq_unlock(&g_pmm_lock);
    }
}

void page_ref(directptr_t page) {
    // increment the refcount
    uintptr_t index = (uintptr_t)ALIGN_DOWN(page, PAGE_SIZE) / PAGE_SIZE;
    atomic_fetch_add(&((_Atomic(uint16_t)*)PAGE_REFCOUNT_START)[index], 1);
}
