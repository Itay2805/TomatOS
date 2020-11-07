#include <util/except.h>
#include <util/string.h>
#include <stdatomic.h>
#include <cont/slist.h>
#include "pmm.h"

static size_t g_total_pages = 0;
static size_t g_free_pages = 0;
static slist_entry_t g_freelist = INIT_SLIST;

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

    // TODO: don't go over the max phsy memory

    for (size_t i = 0; i < memap->entries; i++) {
        stivale2_mmap_entry_t* entry = &memap->memmap[i];
        TRACE("\t", (void*)entry->base," - ", (void*)(entry->base + entry->length), ": ", memmap_type_to_string(entry->type));
        if (entry->type == STIVALE2_MMAP_TYPE_USABLE) {
            directptr_t base = PHYS_TO_DIRECT(entry->base);
            size_t pages = entry->length / PAGE_SIZE;
            g_total_pages += pages;
            // TODO: maybe have this range based instead,
            //       so we will not have to do so many
            //       iterations
            do {
                slist_push(&g_freelist, base);
                base += PAGE_SIZE;
            } while (--pages);
        }
    }

    g_free_pages = g_total_pages;
    TRACE("\tAvailable memory: ", SIZE(g_total_pages * PAGE_SIZE));
}

directptr_t page_alloc() {
    // TODO: lock
    ASSERT(!slist_empty(&g_freelist), "Out of memory!");
    directptr_t page = slist_pop(&g_freelist);
    // TODO: unlock
    return page;
}

void page_free(directptr_t page) {
    // TODO: lock
    // TODO: check if refcount is zero then free
}

void page_ref(directptr_t page) {
    // TODO: increase refcount
}
