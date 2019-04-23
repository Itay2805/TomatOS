#include "pmm.h"

#include "../common/kernel_info.h"
#include "../common/common.h"
#include "../common/stdbool.h"

#include "../graphics/term.h"

////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////

uint32_t end_of_kernel = 0;

/**
 * This is the bitmap itself
 */
static size_t* bitmap;

/**
 * This is the bitmap size in bits
 */
static size_t bitmap_size;

/**
 * Current index for allocating in the bitmap
 * in bits
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
    multiboot_memory_map_t* entries = (multiboot_memory_map_t*)(uintptr_t)multiboot->mmap_addr;
    multiboot_memory_map_t* entry;
    uintptr_t ptr;

    // set the end of the kernel
    end_of_kernel = (uint32_t)ALIGN_UP((uint32_t)KERNEL_END, 4096u);

    // set the bitmap start
    bitmap = (size_t*)(uintptr_t)end_of_kernel;
    bitmap_size = (ALIGN_UP((multiboot->mem_lower + multiboot->mem_upper) * 1024, 4096u) / 4096);
    end_of_kernel += bitmap_size;

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

    // setting the first 1MB as allocated, since it might have important stuff in there
    for(ptr = 0; ptr < ALIGN_UP(0x100000, 4096u); ptr += 4096) {
        pmm_map((void*)ptr);
    }
}

void pmm_map_kernel(void) {
    uintptr_t ptr;

    term_print("[pmm_init] mapping kernel (0x%08x-0x%08x)\n", (uint32_t)KERNEL_START, end_of_kernel);
    for(ptr = ALIGN_DOWN(KERNEL_START, 4096u); ptr < ALIGN_UP(end_of_kernel, 4096u); ptr += 4096) {
        pmm_map((void*)ptr);
    }
}

void pmm_map(void* phys) {
    uintptr_t addr = ALIGN_DOWN((uintptr_t)phys, 4096u) / 4096;
    if(addr >= bitmap_size) {
        // TODO: Address outside of range
    }else {
        bitmap[addr / sizeof(size_t)] |= (1u << (addr % sizeof(size_t)));
    }
}

void* pmm_allocate(size_t count) {
    uintptr_t started_from = bitmap_index;
    uintptr_t start = bitmap_index;
    size_t count_found;
    uintptr_t final_addr;
    size_t i;

    // search for memory, starting at the bitmap index, enough physical pages
    // linearly in memory. making sure they are free.
    for(
        count_found = 0; 
        count_found < count && bitmap_index < bitmap_size;
        bitmap_index++) {
        if((bitmap[bitmap_index / sizeof(size_t)] & (1u << (bitmap_index % sizeof(size_t)))) != 0) {
            count_found = 0;
        }else {
            if(count_found == 0) {
                start = bitmap_index;
            }
            count_found++;
        }
    }

    if(count != count_found) {
        // we did not find enough linear space
        if(started_from == 0) {
            // we started from 0, that means we scanned everything
            // and we just can't find any free memory :(
            term_print("[pmm_allocate] no linear buffer with %d free pages found!\n", (int) count);
            return 0;
        }else {
            // try to look from the start
            bitmap_index = 0;
            return pmm_allocate(count);
        }
    }else {
        // we found enough memory, lets map it and return the address to the
        // start of it
        final_addr = start * 4096;
        for(i = 0; i < count; i++, start++) {
            bitmap[start / sizeof(size_t)] |= (1u << (start % sizeof(size_t)));
        }
        return (void*)final_addr;
    }
}

void pmm_free(void* phys) {
    uintptr_t addr = ALIGN_DOWN((uintptr_t)phys, 4096u) / 4096;
    if(addr >= bitmap_size) {
        // TODO: Address outside of range
    }else {
        bitmap[addr / sizeof(size_t)] &= ~(1u << (addr % sizeof(size_t)));
    }
}