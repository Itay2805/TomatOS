#include <util/string.h>
#include <util/except.h>
#include <util/trace.h>
#include <mem/pmm.h>

#include "arch/intrin.h"
#include "mem/vmm.h"

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

/**
 * A page that is always zeroed out
 */
static physptr_t m_zero_page = 0;

/**
 * The vmm editing lock
 */
static lock_t m_vmm_lock;

/**
 * The top level table
 */
static directptr_t m_pml4;

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VMM initialization
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

err_t init_vmm() {
    err_t err = NO_ERROR;
    TRACE("Initializing VMM");

    stivale2_struct_tag_memmap_t* memap = get_stivale2_tag(STIVALE2_STRUCT_TAG_MEMMAP_ID);
    CHECK(memap != NULL);

    // enable cpu features
    init_vmm_features();

    // allocate the zero page
    directptr_t zero_page = page_alloc();
    memset(zero_page, 0, PAGE_SIZE);
    m_zero_page = DIRECT_TO_PHYS(zero_page);

    TRACE("Creating mappings");

    // initialize the kernel address space
    m_vmm_lock = INIT_LOCK(TPL_HIGH_LEVEL);
    m_pml4 = page_alloc();

    // create the kernel mappings
    TRACE("\t* mapping physical memory");
    for (int i = 0; i < memap->entries; i++) {
        stivale2_mmap_entry_t* entry = &memap->memmap[i];
        if (
            entry->type == STIVALE2_MMAP_RESERVED ||
            entry->type == STIVALE2_MMAP_USABLE ||
            entry->type == STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE
        ) {
            uintptr_t base = ALIGN_DOWN(entry->base, PAGE_SIZE);
            size_t page_count = (ALIGN_UP(entry->base + entry->length + entry->unused, PAGE_SIZE) - base) / PAGE_SIZE;
            CHECK_AND_RETHROW(vmm_map((uintptr_t)PHYS_TO_DIRECT(base), base, page_count, MAP_READ | MAP_WRITE));
        }
    }

    // create kernel mappings
    TRACE("\t* mapping kernel");
    size_t rx_pages = (__kernel_end_text - __kernel_start_text) / PAGE_SIZE;
    size_t ro_pages = (__kernel_end_rodata - __kernel_end_text) / PAGE_SIZE;
    size_t rw_pages = (__kernel_end_data - __kernel_end_rodata) / PAGE_SIZE;
    CHECK_AND_RETHROW(vmm_map((uintptr_t)__kernel_start_text, (uintptr_t)(__kernel_start_text - KERNEL_BASE), rx_pages, MAP_READ | MAP_EXEC));
    CHECK_AND_RETHROW(vmm_map((uintptr_t)__kernel_end_text, (uintptr_t)(__kernel_end_text - KERNEL_BASE), ro_pages, MAP_READ));
    CHECK_AND_RETHROW(vmm_map((uintptr_t)__kernel_end_rodata, (uintptr_t)(__kernel_end_rodata - KERNEL_BASE), rw_pages, MAP_READ | MAP_WRITE));

    // switch to kernel address space
    set_address_space();

cleanup:
    return err;
}

void set_address_space() {
    __writecr3(DIRECT_TO_PHYS(m_pml4));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Map and Unmap primitives
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static err_t get_page(uintptr_t virt, uint64_t** page) {
    err_t err = NO_ERROR;

    CHECK(page != NULL);

    uint64_t* table = m_pml4;
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

static uint64_t* get_or_alloc_page(uintptr_t virt, uint64_t flags_and, uint64_t flags_or) {
    uint64_t* table = m_pml4;
    for (int level = 4; level > 1; level--) {
        uint64_t* entry = &table[(virt >> (12u + 9u * (level - 1))) & 0x1ffu];

        // check if need to allocate entry
        if (!(*entry & PM_PRESENT)) {
            directptr_t ptr = page_alloc();
            if (ptr == NULL) {
                return NULL;
            }

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

err_t vmm_map(uintptr_t virt, physptr_t phys, size_t pages, page_perms_t perms) {
    err_t err = NO_ERROR;

    acquire_lock(&m_vmm_lock);

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
        uint64_t* entry = get_or_alloc_page(virt, ~flags_remove, flags_add);
        CHECK_ERROR(entry != NULL, ERROR_OUT_OF_RESOURCES);
        *entry = flags_add | phys;

        // next page
        virt += PAGE_SIZE;
        phys += PAGE_SIZE;
    }

cleanup:
    release_lock(&m_vmm_lock);

    return err;
}

err_t vmm_unmap(uintptr_t virt, size_t pages) {
    err_t err = NO_ERROR;

    acquire_lock(&m_vmm_lock);

    while (pages--) {
        // get and unmap the page
        uint64_t* entry;
        CHECK_AND_RETHROW(get_page(virt, &entry));
        *entry = 0;

        // invalidate the page
        __invlpg(virt);

        // next page
        virt += PAGE_SIZE;
    }

cleanup:
    release_lock(&m_vmm_lock);

    return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Page fault handling
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

err_t vmm_handle_pagefault(uintptr_t addr, page_fault_params_t params) {
    err_t err = NO_ERROR;

    if (
        (KERNEL_HEAP_START <= addr && addr < KERNEL_HEAP_END)
    ) {
        //
        // on-demand kernel paging ranges
        //
        // our on-demand mapping is limited to very specific set of ranges which are the kernel
        // heap and the pmm bitmap, both of those are grown dynamically as more memory is allocated
        // in the system. both these ranges are defined to always be rw
        //
        // for pages which are just being read we map to the same zero page and we map them as read only,
        // for pages which are written we allocate a new page and map it as rw.
        //

        if (params.write) {
            // this is on a write, we don't care if there was a mapping or not because we
            // create a new page anyways
            directptr_t dpage = page_alloc();
            CHECK_ERROR(dpage != NULL, ERROR_OUT_OF_RESOURCES);
            physptr_t page = DIRECT_TO_PHYS(dpage);
            CHECK_AND_RETHROW(vmm_map(ALIGN_DOWN(addr, PAGE_SIZE), page, 1, MAP_WRITE));
        } else {
            // this is on a read, there is no way to disable reading on a page
            // so this 100% a missing page, map the zero page onto it
            CHECK_AND_RETHROW(vmm_map(ALIGN_DOWN(addr, PAGE_SIZE), m_zero_page, 1, MAP_READ));
        }

        // make sure the address is invalidated
        // no matter what
        __invlpg(addr);

    } else {
        //
        // This was a real page fault, fail the check so
        // we will put a register dump.
        //
        UNLOCKED_ERROR("This is a real kernel page fault :(");
        err = ERROR_CHECK_FAILED;
    }

cleanup:
    return err;
}
