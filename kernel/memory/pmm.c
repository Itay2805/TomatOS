#include "pmm.h"

#include "../common/kernel_info.h"
#include "../common/common.h"
#include "../common/stdbool.h"

#include "../graphics/term.h"

////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////

uintptr_t end_of_kernel = 0;

/**
 * This is the bitmap itself
 */
static size_t* bitmap;

/**
 * This is the bitmap size in entries
 */
static size_t bitmap_size;

/**
 * Current index for allocating in the bitmap
 */
static size_t bitmap_index = 0;

////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////

static const char* mmap_type_names[] = {
        "Available",
        "Reserved",
        "ACPI reclaimable",
        "NVS",
        "Bad memory"
};

void pmm_init(multiboot_info_t* multiboot) {
    multiboot_memory_map_t* entries = (multiboot_memory_map_t*)multiboot->mmap_addr;
    multiboot_module_t* modules = (multiboot_module_t*)multiboot->mods_addr;
    multiboot_memory_map_t* entry;
    multiboot_module_t* module;
    uintptr_t ptr;

    // set the end of the kernel
    end_of_kernel = ALIGN_UP(KERNEL_END, 4096u);

    // set the bitmap start
    bitmap = (size_t*)end_of_kernel;
    bitmap_size = (multiboot->mem_lower + multiboot->mem_upper) / sizeof(size_t);
    end_of_kernel += ALIGN_UP((multiboot->mem_lower + multiboot->mem_upper), 4096u);

    // allocate all the pages set as unavailable ram as allocated
    term_print("[pmm_init] iterating memory map:\n");
    for(entry = entries; (char*)entry - (char*)entries < multiboot->mmap_length; entry++) {
        // print the entry
        uint32_t addr_start_lower = (uint32_t)(entry->addr & 0xFFFFFFFF);
        uint32_t addr_start_higher = (uint32_t)(entry->addr >> 32u);
        uint32_t addr_end_lower = (uint32_t)((entry->addr + entry->len) & 0xFFFFFFFF);
        uint32_t addr_end_higher = (uint32_t)((entry->addr + entry->len) >> 32u);
        term_print("[pmm_init] \t0x%08x_%08x-0x%08x_%08x : %s\n", addr_start_higher, addr_start_lower, addr_end_higher, addr_end_lower, mmap_type_names[entry->type - 1]);

        if(entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
            for(ptr = ALIGN_DOWN(entry->addr, 4096u); ptr < ALIGN_UP(entry->addr + entry->len, 4096u); ptr += 4096) {
                pmm_map((void*)ptr);
            }
        }
    }

    // set all of the modules as allocated
    term_print("[pmm_init] mapping kernel (0x%08x-0x%08x)\n", (uint32_t)KERNEL_START, (uint32_t)KERNEL_END);
    for(ptr = ALIGN_DOWN(KERNEL_START, 4096u); ptr < ALIGN_UP(KERNEL_END, 4096u); ptr += 4096) {
        pmm_map((void*)ptr);
    }
}

void pmm_map(void* phys) {
    uintptr_t addr = ALIGN_DOWN((uintptr_t)phys, 4096u);
    if((void*)addr >= bitmap + bitmap_size) {
        // TODO: Error, page outside of range
    }else {
        bitmap[addr / sizeof(size_t)] &= (1u << (addr & sizeof(size_t)));
    }
}

void* pmm_allocate(size_t count) {
    uintptr_t started_from = bitmap_index;
    uintptr_t start = bitmap_index;
    size_t count_found;
    bool found = false;
    uintptr_t final_addr, map_addr;
    int i;

    // search for memory, starting at the bitmap index, enough physical pages
    // linearly in memory. making sure they are free.
    for(
        count_found = 0; 
        count_found < count && start < bitmap_size * sizeof(size_t); 
        start++, count_found++) {
        if(bitmap[start / sizeof(size_t)] & (1u << (start & sizeof(size_t))) == 0) {
            break;                                
        }
    }

    if(count != count_found) {
        // we did not find enough linear space
        if(started_from == 0) {
            // we started from 0, that means we scanned everything
            // and we just can't find any free memory :(
            return 0;
        }else {
            // try to look from the start
            bitmap_index = 0;
            return pmm_allocate(count);
        }
    }else {
        // we found enough memory, lets map it and return the address to the
        // start of it
        map_addr = start * 4096;
        final_addr = map_addr;
        for(i = 0; i < count; i++, map_addr += 4096) {
            bitmap[map_addr / sizeof(size_t)] &= (1u << (map_addr & sizeof(size_t)));
        }
        return (void*)final_addr;
    }
}

void pmm_free(void* phys) {
    uintptr_t addr = ALIGN_DOWN((uintptr_t)phys, 4096u);
    if((void*)addr >= bitmap + bitmap_size) {
        // TODO: Error, page outside of range
    }else {
        bitmap[addr / sizeof(size_t)] = ~(bitmap[addr / sizeof(size_t)] & (1u << (addr & sizeof(size_t))));
    }
}