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

static ticket_lock_t g_pmm_lock = INIT_LOCK();

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
    ticket_lock(&g_pmm_lock);

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

    ticket_unlock(&g_pmm_lock);
    return res;
}

void page_free(directptr_t page) {
    // TODO: lock
    // TODO: check if refcount is zero then free
}

void page_ref(directptr_t page) {
    // TODO: increase refcount
}
