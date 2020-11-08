#include <util/string.h>
#include <util/except.h>
#include <util/trace.h>
#include <mem/pmm.h>
#include <proc/proc.h>
#include "mem/vmm.h"
#include "arch/intrin.h"

/**
 * Kernel regions
 */
extern symbol_t __kernel_start_text;
extern symbol_t __kernel_end_text;
extern symbol_t __kernel_end_rodata;
extern symbol_t __kernel_end_data;

/**
 * Bits
 */
#define PM_PRESENT  (BIT0)
#define PM_WRITE    (BIT1)
#define PM_USER     (BIT2)
#define PM_GLOBAL   (BIT8)
#define PM_SIZE     (BIT7)
#define PM_XD       (BIT63)

#define PM_ADDR(entry) ((entry) & 0x7ffffffffffff000ull)

static physptr_t g_zero_page = 0;

static void init_vmm_features() {
    IA32_CR4 cr4 = __readcr4();
    uint32_t num;

    // TODO: global pages

    ////////////////////////////////////
    // Feature Information
    ////////////////////////////////////
    cpuid(0x1, NULL, NULL, NULL, &num);

    //----------------------------------
    // PAT - Must be supported
    //----------------------------------
    ASSERT(num & BIT16, "PAT must be supported");
    TRACE("\t* PAT");

    uint64_t pat = __rdmsr(MSR_IA32_PAT);
    pat &= 0xFFFFFFFF;
    pat |= (PAT_WC << 32);
    __wrmsr(MSR_IA32_PAT, pat);

    ////////////////////////////////////
    // apply it
    ////////////////////////////////////

    __writecr4(cr4);
}

err_t init_vmm(stivale2_struct_tag_memmap_t* memap) {
    err_t err = NO_ERROR;
    TRACE("Initializing VMM");

    // enable cpu features
    init_vmm_features();

    // allocate the zero page
    directptr_t zero_page = page_alloc();
    memset(zero_page, 0, PAGE_SIZE);
    g_zero_page = DIRECT_TO_PHYS(zero_page);

    TRACE("Creating kernel mappings");

    // initialize the kernel address space
    g_kernel.address_space.lock = INIT_LOCK();
    g_kernel.address_space.pml4 = page_alloc();

    // all the kernel entries are preallocated, this makes sure the
    // address space is always in sync between all the kernel threads
    TRACE("\t* top level");
    uint64_t* table = g_kernel.address_space.pml4;
    for (int i = 256; i < 512; i++) {
        directptr_t ptr = page_alloc();
        memset(ptr, 0, PAGE_SIZE);
        table[i] = DIRECT_TO_PHYS(ptr);
        table[i] |= (PM_PRESENT | PM_WRITE);
    }

    // create the kernel mappings
    TRACE("\t* direct map");
    for (int i = 0; i < memap->entries; i++) {
        stivale2_mmap_entry_t* entry = &memap->memmap[i];
        if (entry->type != STIVALE2_MMAP_TYPE_BAD_MEMORY) {
            vmm_map(
                    &g_kernel.address_space,
                    (uintptr_t)PHYS_TO_DIRECT(entry->base),
                    ALIGN_DOWN(entry->base, PAGE_SIZE),
                    ALIGN_UP(entry->length, PAGE_SIZE) / PAGE_SIZE,
                    MAP_READ
            );
        }
    }

    // create kernel mappings
    TRACE("\t* kernel");
    size_t rx_pages = (__kernel_end_text - __kernel_start_text) / PAGE_SIZE;
    size_t ro_pages = (__kernel_end_rodata - __kernel_end_text) / PAGE_SIZE;
    size_t rw_pages = (__kernel_end_data - __kernel_end_rodata) / PAGE_SIZE;
    CHECK_AND_RETHROW(vmm_map(&g_kernel.address_space, (uintptr_t)__kernel_start_text, (uintptr_t)(__kernel_start_text - KERNEL_BASE), rx_pages, MAP_EXEC));
    CHECK_AND_RETHROW(vmm_map(&g_kernel.address_space, (uintptr_t)__kernel_end_text, (uintptr_t)(__kernel_end_text - KERNEL_BASE), ro_pages, MAP_READ));
    CHECK_AND_RETHROW(vmm_map(&g_kernel.address_space, (uintptr_t)__kernel_end_rodata, (uintptr_t)(__kernel_end_rodata - KERNEL_BASE), rw_pages, MAP_WRITE));

    // switch to kernel address space
    CHECK_AND_RETHROW(set_address_space(&g_kernel.address_space));

cleanup:
    return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Map and Unmap primitives
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static err_t get_page(address_space_t* space, uintptr_t virt, uint64_t** page) {
    err_t err = NO_ERROR;

    CHECK(page != NULL);
    CHECK(space != NULL);

    uint64_t* table = space->pml4;
    for (int level = 4; level > 1; level--) {
        uint64_t* entry = &table[(virt >> (12u + 9u * (level - 1))) & 0x1ffu];

        // check if present
        CHECK_ERROR(*entry & PM_PRESENT, ERROR_NOT_FOUND);

        // get the next layer
        table = PHYS_TO_DIRECT(PM_ADDR(*entry));
    }

    CHECK_ERROR(table[(virt >> 12u) & 0x1ffu] & PM_PRESENT, ERROR_NOT_FOUND);
    *page = &table[(virt >> 12u) & 0x1ffu];

cleanup:
    return err;
}

static uint64_t* get_or_alloc_page(address_space_t* space, uintptr_t virt, uint64_t flags_and, uint64_t flags_or) {
    uint64_t* table = space->pml4;
    for (int level = 4; level > 1; level--) {
        uint64_t* entry = &table[(virt >> (12u + 9u * (level - 1))) & 0x1ffu];

        // check if need to allocate entry
        if (!(*entry & PM_PRESENT)) {
            directptr_t ptr = page_alloc();
            memset(ptr, 0, PAGE_SIZE);
            *entry = DIRECT_TO_PHYS(ptr);
        }
        *entry &= flags_and;
        *entry |= flags_or;

        // get the next layer
        table = PHYS_TO_DIRECT(PM_ADDR(*entry));
    }

    return &table[(virt >> 12u) & 0x1ffu];
}

err_t set_address_space(address_space_t* space) {
    err_t err = NO_ERROR;

    CHECK(space != NULL);

    if (PHYS_TO_DIRECT(__readcr3()) != space->pml4) {
        __writecr3(DIRECT_TO_PHYS(space->pml4));
    }

cleanup:
    return err;
}

err_t vmm_map(address_space_t* space, uintptr_t virt, physptr_t phys, size_t pages, page_perms_t perms) {
    err_t err = NO_ERROR;

    CHECK(space != NULL);

    ticket_lock(&space->lock);

    uint64_t flags_add = PM_PRESENT;
    if (perms & MAP_WRITE) {
        flags_add |= PM_WRITE;
    }

    uint64_t flags_remove = 0;
    if (perms & MAP_EXEC) {
        flags_remove |= PM_XD;
    } else {
        // TODO: enable xd
        // flags_add |= PM_XD;
    }

    while (pages--) {
        // set this page as mapped
        uint64_t* entry = get_or_alloc_page(space, virt, ~flags_remove, flags_add);
        *entry = flags_add | phys;

        // next page
        virt += PAGE_SIZE;
        phys += PAGE_SIZE;
    }

cleanup:
    if (space != NULL) {
        ticket_unlock(&space->lock);
    }

    return err;
}

err_t vmm_unmap(address_space_t* space, uintptr_t virt, size_t pages) {
    err_t err = NO_ERROR;

    CHECK(space != NULL);

    ticket_lock(&space->lock);

    while (pages--) {
        // get and unmap the page
        uint64_t* entry;
        CHECK_AND_RETHROW(get_page(space, virt, &entry));
        *entry = 0;

        // invalidate the page
        __invlpg(virt);

        // next page
        virt += PAGE_SIZE;
    }

cleanup:
    if (space != NULL) {
        ticket_unlock(&space->lock);
    }

    return err;
}

