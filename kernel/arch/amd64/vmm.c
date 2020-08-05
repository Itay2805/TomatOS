#include <proc/process.h>
#include <util/except.h>
#include <mem/pmm.h>
#include <stdbool.h>
#include <util/string.h>

#include "stivale.h"
#include "mem/vmm.h"
#include "intrin.h"


typedef char symbol_t[0];

/**
 * Kernel regions
 */
symbol_t __kernel_start_text;
symbol_t __kernel_end_text;
symbol_t __kernel_end_rodata;
symbol_t __kernel_end_data;

/**
 * Bits
 */
#define PM_PRESENT  (BIT0)
#define PM_WRITE    (BIT1)
#define PM_USER     (BIT2)
#define PM_GLOBAL   (BIT8)
#define PM_SIZE     (BIT7)
#define PM_XD       (BIT63)

#define HUGE_PAGE_SIZE PAGE_SIZE

/**
 * Are global pages supported
 */
static bool g_global_pages = false;

/**
 * Is smap supported (and should we handle it)
 */
static bool g_smap;

/**
 * Figure which features are available and need to be enabled
 * @return
 */
err_t vmm_figure_features() {
    err_t err = NO_ERROR;
    IA32_CR4 cr4 = __readcr4();

    if(g_cpu_id == 0) TRACE("CPU Paging Features");

    // TODO: no execute

    //----------------------------------
    // Extended features
    //----------------------------------

    uint32_t num;
    cpuidex(0x7, 0x0, NULL, &num, NULL, NULL);

    if (num & BIT7) {
        if(g_cpu_id == 0) TRACE("\t* SMEP");
        cr4.SMEP = 1;
    }

    if (num & BIT20) {
        if(g_cpu_id == 0) TRACE("\t* SMAP");
        cr4.SMAP = 1;
    }

    //----------------------------------
    // Version id
    //----------------------------------
    cpuidex(0x0, 0, NULL, NULL, NULL, &num);

    if (num & BIT13) {
        if(g_cpu_id == 0) TRACE("\t* Global pages");
        cr4.PGE = 1;
        g_global_pages = true;
    }

    // apply it
    __writecr4(cr4);

    return err;
}

// We get in this function `applying non-zero offset FFFF800000000000 to null pointer`
// this is because one of the mapped addresses are 0, and adding offset to a null pointer
// is technically undefined but not in this specific case, so we will disable ubsan for it
__attribute__((no_sanitize("undefined")))
static err_t map_huge_page(physptr_t ptr) {
    err_t err = NO_ERROR;

    // TODO: support 2mb paging
    CHECK_AND_RETHROW(vmm_map(&g_kernel.address_space, PHYSICAL_TO_DIRECT(ptr), ptr, MAP_WRITE));

cleanup:
    return err;
}

err_t init_vmm(stivale_struct_t* stivale) {
    err_t err = NO_ERROR;

    // setup the cpu features
    CHECK_AND_RETHROW(vmm_figure_features());

    // initialize the kernel address space
    g_kernel.address_space.lock = INIT_LOCK();
    g_kernel.address_space.pml4 = pmallocz(PAGE_SIZE);

    // all the kernel entries are preallocated, this makes sure the
    // address space is always in sync between all the kernel threads
    uint64_t* table = g_kernel.address_space.pml4;
    for (int i = 256; i < 512; i++) {
        table[i] = (uint64_t)pmallocz(PAGE_SIZE);
        table[i] = DIRECT_TO_PHYSICAL(table[i]);
        table[i] |= (PM_PRESENT | PM_WRITE);
    }

    // figure the largest address and map everything to it linearly
    mmap_entry_t *entry = &stivale->memory_map_addr[stivale->memory_map_entries - 1];
    uintptr_t largest_addr = entry->base + entry->length;
    for (physptr_t ptr = 0; ptr < ALIGN_UP(largest_addr, HUGE_PAGE_SIZE); ptr += HUGE_PAGE_SIZE) {
        CHECK_AND_RETHROW(map_huge_page(ptr));
    }

    // now map the kernel (at -2GB)
    TRACE("Mapping kernel");
    for (void* ptr = __kernel_start_text;
         ptr < (void*)ALIGN_UP(__kernel_end_text, PAGE_SIZE);
         ptr += PAGE_SIZE) {
        CHECK_AND_RETHROW(vmm_map(&g_kernel.address_space, ptr, ptr - KERNEL_BASE, MAP_EXEC));
    }
    for (void* ptr = ALIGN_UP(__kernel_end_text, PAGE_SIZE);
         ptr < (void*)ALIGN_UP(__kernel_end_rodata, PAGE_SIZE);
         ptr += PAGE_SIZE) {
        CHECK_AND_RETHROW(vmm_map(&g_kernel.address_space, ptr, ptr - KERNEL_BASE, 0));
    }
    for (void* ptr = ALIGN_UP(__kernel_end_rodata, PAGE_SIZE);
         ptr < (void*)ALIGN_UP(__kernel_end_data, PAGE_SIZE);
         ptr += PAGE_SIZE) {
        CHECK_AND_RETHROW(vmm_map(&g_kernel.address_space, ptr, ptr - KERNEL_BASE, MAP_WRITE));
    }

    // set the kernel paging
    CHECK_AND_RETHROW(set_address_space(&g_kernel.address_space));

cleanup:
    return err;
}

err_t set_address_space(address_space_t* space) {
    err_t err = NO_ERROR;

    CHECK(space != NULL);

    if (PHYSICAL_TO_DIRECT(__readcr3()) != space->pml4) {
        __writecr3(DIRECT_TO_PHYSICAL(space->pml4));
    }

cleanup:
    return err;
}

err_t vmm_map(address_space_t* space, void* virtual, physptr_t physical, map_flag_t flag) {
    err_t err = NO_ERROR;
    bool map_global = false;
    uintptr_t virt = (uintptr_t)virtual;

    CHECK(space != NULL);
    ticket_lock(&space->lock);

    uint64_t flags = PM_PRESENT;

    // set the write flags
    if (flag & MAP_WRITE) {
        flags |= PM_WRITE;
    }

    // set the user permission flags
    if (virt < USERSPACE_END) {
        flags |= PM_USER;
    } else if (g_global_pages) {
        map_global = true;
    }

    // TODO: xd

    uint64_t* table = space->pml4;
    for (int level = 4; level > 1; level--) {
        uint64_t* entry = &table[(virt >> (12u + 9u * (level - 1))) & 0x1ffu];

        // check if need to allocate entry
        if (!(*entry & PM_PRESENT)) {
            directptr_t ptr = pmallocz(PAGE_SIZE);
            *entry = DIRECT_TO_PHYSICAL(ptr);
        }
        *entry |= flags;

        // get the next layer
        table = PHYSICAL_TO_DIRECT(*entry & 0x7ffffffffffff000ull);
    }

    // map it
    uint64_t* entry = &table[(virt >> 12u) & 0x1ffu];
    *entry = flags | physical;
    if (map_global) {
        *entry |= PM_GLOBAL;
    }

cleanup:
    if (space != NULL) {
        ticket_unlock(&space->lock);
    }
    return err;
}

err_t vmm_unmap(address_space_t* space, void* virtual) {
    err_t err = NO_ERROR;
    uintptr_t virt = (uintptr_t)virtual;

    CHECK(space != NULL);
    ticket_lock(&space->lock);

    uint64_t* table = space->pml4;
    for (int level = 4; level > 1; level--) {
        uint64_t* entry = &table[(virt >> (12u + 9u * (level - 1))) & 0x1ffu];

        // check if present
        CHECK_ERROR(*entry & PM_PRESENT, ERROR_NOT_FOUND);

        // get the next layer
        table = PHYSICAL_TO_DIRECT(*entry & 0x7ffffffffffff000ull);
    }

    uint64_t* entry = &table[(virt >> 12u) & 0x1ffu];
    *entry = 0;
    __invlpg((uintptr_t)virtual);

cleanup:
    if (space != NULL) {
        ticket_unlock(&space->lock);
    }
    return err;
}

// TODO: if we page fault in any of these we should return
//       ERROR_INVALID_ADDRESS

err_t copy_from_user(void* dst, void* src, size_t size) {
    err_t err = NO_ERROR;

    size_t addition;
    CHECK_ERROR(!__builtin_add_overflow((uintptr_t)src, size, &addition), ERROR_INVALID_ADDRESS);
    CHECK_ERROR(addition < USERSPACE_END, ERROR_INVALID_ADDRESS);

    if (g_smap) __stac();
    memcpy(dst, src, size);
    if (g_smap) __clac();

cleanup:
    return err;
}

err_t copy_to_user(void* dst, void* src, size_t size) {
    err_t err = NO_ERROR;

    size_t addition;
    CHECK_ERROR(!__builtin_add_overflow((uintptr_t)dst, size, &addition), ERROR_INVALID_ADDRESS);
    CHECK_ERROR(addition < USERSPACE_END, ERROR_INVALID_ADDRESS);

    if (g_smap) __stac();
    memcpy(dst, src, size);
    if (g_smap) __clac();

cleanup:
    return err;
}
