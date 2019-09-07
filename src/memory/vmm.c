#include "vmm.h"
#include "pmm.h"
#include "mm.h"

#include <common.h>
#include <string.h>
#include <cpu/cr.h>
#include <cpu/msr.h>
#include <locks/spinlock.h>
#include <cpu/cpuid.h>
#include <cpu/atomic.h>

////////////////////////////////////////////////////////////////////////////
// Page attributes
////////////////////////////////////////////////////////////////////////////

#define PAGING_PRESENT_BIT          (1ul << 0ul)
#define PAGING_READ_WRITE_BIT       (1ul << 1ul)
#define PAGING_USER_BIT             (1ul << 2ul)
#define PAGING_PWT_BIT              (1ul << 3ul)
#define PAGING_PCD_BIT              (1ul << 4ul)
#define PAGING_ACCESSED_BIT         (1ul << 5ul)
#define PAGING_DIRTY_BIT            (1ul << 6ul)
// only valid on 2mb/1GB pages
#define PAGING_PAGE_SIZE_BIT        (1ul << 7ul)
// only valid on 4k pages
#define PAGING_PAT_BIT_4K           (1ul << 7ul)
#define PAGING_NO_EXECUTE_BIT       (1ul << 63ul)
#define PAGING_GLOBAL_BIT           (1ul << 8ul)
// only valid on 2m/1gb pages
#define PAGING_PAT_BIT              (1ul << 12ul)

// only valid when pat is enabled
#define PAGING_PAT0    (0)
#define PAGING_PAT1    (PAGING_PWT_BIT)
#define PAGING_PAT2    (PAGING_PCD_BIT)
#define PAGING_PAT3    (PAGING_PWT_BIT | PAGING_PCD_BIT)

#define PAGING_PAT4    (PAGING_PAT_BIT)
#define PAGING_PAT5    (PAGING_PAT_BIT | PAGING_PWT_BIT)
#define PAGING_PAT6    (PAGING_PAT_BIT | PAGING_PWT_BIT | PAGING_PCD_BIT)
#define PAGING_PAT7    (PAGING_PAT_BIT | PAGING_PAT_BIT)

#define PAGING_4K_PAT4    (PAGING_PAT_BIT_4K)
#define PAGING_4K_PAT5    (PAGING_PAT_BIT_4K | PAGING_PWT_BIT)
#define PAGING_4K_PAT6    (PAGING_PAT_BIT_4K | PAGING_PWT_BIT | PAGING_PCD_BIT)
#define PAGING_4K_PAT7    (PAGING_PAT_BIT_4K | PAGING_PAT_BIT)

#define PAGING_PAT_WRITE_BACK           PAGING_PAT0
#define PAGING_PAT_WRITE_THROUGH        PAGING_PAT1
#define PAGING_PAT_WRITE_COMBINING      PAGING_PAT2
#define PAGING_PAT_UNCACHEABLE          PAGING_PAT3

////////////////////////////////////////////////////////////////////////////
// Masks for the table entries
////////////////////////////////////////////////////////////////////////////

// ----------------------------------
// When using 1GB pages
// ----------------------------------
#define PAGING_1GB_PML3_MASK    (0x7FFFFFFFC0000000ul)

// ----------------------------------
// When using 2MB pages
// ----------------------------------
#define PAGING_2MB_PML2_MASK    (0x7FFFFFFFFFE00000ul)

// ----------------------------------
// When using 4KB pages
// ----------------------------------
#define PAGING_4KB_ADDR_MASK    (0x7FFFFFFFFFFFF000ul)

////////////////////////////////////////////////////////////////////////////
// Index addresses
////////////////////////////////////////////////////////////////////////////

#define PAGING_PML4E_OFFSET(addr) (((uint64_t)(addr) >> 39u) & 0x1FF)
#define PAGING_PML3E_OFFSET(addr) (((uint64_t)(addr) >> 30u) & 0x1FF)
#define PAGING_PML2E_OFFSET(addr) (((uint64_t)(addr) >> 21u) & 0x1FF)
#define PAGING_PML1E_OFFSET(addr) (((uint64_t)(addr) >> 12u) & 0x1FF)

#define PAGING_4KB_PAGE_OFFSET(addr)        (((uint64_t)(addr) & ~(PAGING_4KB_ADDR_MASK | BIT(63ul))))
#define PAGING_2MB_PAGE_OFFSET(addr)    (((uint64_t)(addr) & ~(PAGING_2MB_PML2_MASK | BIT(63ul))))
#define PAGING_1GB_PAGE_OFFSET(addr)    (((uint64_t)(addr) & ~(PAGING_1GB_PML3_MASK | BIT(63ul))))
////////////////////////////////////////////////////////////////////////////
// Index addresses
////////////////////////////////////////////////////////////////////////////

/*
 * This is the physical memory start
 *
 * this is set to 0 at the start and later on is set to 0xFFFF800000000000
 */
char* physical_memory = 0;

/*
 * This contains the paging of the kernel, when creating new address spaces
 * we are going to copy from this.
 */
address_space_t kernel_address_space;

/*
 * Global VMM lock (should probably have one lock per address space instead)
 */
static spinlock_t vmm_lock;

// features
static bool supports_global_pages;
static bool supports_1gb_paging;
static bool supports_pat;

////////////////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////////////////

/**
 * Will set the attributes as needed
 *
 * If it should have user access, set the user bit;
 * If it should have write access, set the write bit;
 * If it should have execute access, unset the execute bit;
 *
 * @param entry         [IN/OUT]    The entry to change
 * @param attributes    [IN]        The attributes to set
 */
static inline void set_attributes(uint64_t* entry, page_attrs_t attributes) {
    if(!(*entry & PAGING_USER_BIT) && (attributes.user)) {
        *entry |= PAGING_USER_BIT;
    }
    if(!(*entry & PAGING_READ_WRITE_BIT) && (attributes.write)) {
        *entry |= PAGING_READ_WRITE_BIT;
    }
    if((*entry & PAGING_NO_EXECUTE_BIT) && (attributes.execute)) {
        *entry &= ~PAGING_NO_EXECUTE_BIT;
    }
    _barrier();
}

/**
 * will set the attributes for caching
 *
 * This should only be called on the last page entry
 *
 * TODO: Maybe make into two functions
 *
 * @remark
 * If the PAT is not available, WRITE COMBINING is going to be ignored
 *
 * @param entry         [IN/OUT]    The entry to change
 * @param attributes    [IN]        The attributes to set
 */
static inline void set_cache(uint64_t* entry, page_attrs_t attributes) {
    switch(attributes.caching) {
        case PAGE_CACHE_UNCACHABLE:
            *entry |= PAGING_PAT_UNCACHEABLE;
            break;

        case PAGE_CACHE_WRITE_BACK:
            *entry |= PAGING_PAT_WRITE_BACK;
            break;

        case PAGE_CACHE_WRITE_COMBINING:
            if(supports_pat) {
                *entry |= PAGING_PAT_WRITE_COMBINING;
            }
            break;

        case PAGE_CACHE_WRITE_THROUGH:
            *entry |= PAGING_PAT_WRITE_THROUGH;
            break;

        /*
         * Ignore invalid
         */
        default:
            break;
    }
    _barrier();
}

/**
 * Get the page from the table entry
 *
 * Will also set the top entry's attributes if needed
 *
 * @param entry         [IN/OUT]    The entry, attributes may change
 * @param attributes    [IN]        The attributes
 *
 * @return 0 if not found, otherwise the address of the table
 */
static uint64_t get_page(uint64_t *entry, page_attrs_t attributes) {
    if((*entry & PAGING_PRESENT_BIT) != 0) {
        set_attributes(entry, attributes);
        return *entry & PAGING_4KB_ADDR_MASK;
    }else {
        return NULL;
    }
}

/**
 * Will get the page, if not presented it will create a new one
 *
 * @param entry         [IN/OUT]    The entry to get the table from, or set the table to
 * @param attributes    [IN]        The attributes the table should have
 * @param out_page      [OUT]       The new table address (in physical memory)
 */
static error_t get_or_create_page(uint64_t *entry, page_attrs_t attributes, uint64_t *out_page) {
    error_t err = NO_ERROR;
    uint64_t new_page = get_page(entry, attributes);

    if(!new_page) {
        // allocate and zero new page
        CHECK_AND_RETHROW(pmm_allocate(&new_page));
        memset(&physical_memory[new_page], 0, KB(4));

        // set the entry
        *entry = PAGING_PRESENT_BIT | (new_page & PAGING_4KB_ADDR_MASK);
        set_attributes(entry, attributes);
        _barrier();
    }

    *out_page = new_page;

cleanup:
    return err;
}

/**
 * Will get the page table from the pml4.
 *
 * @param pml4          [IN] The pml4 physical address
 * @param virt_addr     [IN] The virtual address to get the page table for
 * @param attributes    [IN] The attributes to set
 *
 * @return The physical address of the page table
 */
static uint64_t get_pml1(uint64_t pml4, uint64_t virt_addr, page_attrs_t attributes) {
    uint64_t* table = (uint64_t *) &physical_memory[pml4];

    // get pdp
    {
        uint64_t next = get_page(&table[PAGING_PML4E_OFFSET(virt_addr)], attributes);
        if(next == 0) return NULL;
        table = (uint64_t *) &physical_memory[next];
    }

    // get the pd
    {
        uint64_t next = get_page(&table[PAGING_PML3E_OFFSET(virt_addr)], attributes);
        if(next == 0) return NULL;
        table = (uint64_t *) &physical_memory[next];
    }

    // get the pt
    {
        uint64_t next = get_page(&table[PAGING_PML2E_OFFSET(virt_addr)], attributes);
        return next;
    }
}

/**
 * Will get the page table from the pml4. Will create the required entries if needed
 *
 * @param pml4          [IN]    The physical address of the pml4
 * @param virt_addr     [IN]    The virtual address we wanna get the page table for
 * @param attributes    [IN]    The attributes to give until the page table
 * @param pt            [OUT]   The physical address of the new page table
 */
static error_t get_or_create_pml1(uint64_t pml4, uint64_t virt_addr, page_attrs_t attributes, uint64_t *pt) {
    error_t err = NO_ERROR;

    uint64_t* table = (uint64_t*)&physical_memory[pml4];

    // get pml3
    {
        uint64_t next = 0;
        CHECK_AND_RETHROW(get_or_create_page((uint64_t*)&table[PAGING_PML4E_OFFSET(virt_addr)], attributes, &next));
        table = (uint64_t *) &physical_memory[next];
    }

    // get the pml2
    {
        uint64_t next = 0;
        CHECK_AND_RETHROW(get_or_create_page((uint64_t*)&table[PAGING_PML3E_OFFSET(virt_addr)], attributes, &next));
        table = (uint64_t*) &physical_memory[next];
    }

    // get the pml1
    {
        uint64_t next = 0;
        CHECK_AND_RETHROW(get_or_create_page((uint64_t*)&table[PAGING_PML2E_OFFSET(virt_addr)], attributes, &next));
        *pt = (uint64_t) next;
    }

cleanup:
    return err;
}

static uintptr_t internal_virt_to_phys(address_space_t address_space, uintptr_t virtual_address) {
    uint64_t* table = (uint64_t *) &physical_memory[address_space];

    // get pml3
    {
        uint64_t next = get_page(&table[PAGING_PML4E_OFFSET(virtual_address)], (page_attrs_t){0});
        if(next == 0) return (uintptr_t) -1;
        table = (uint64_t *) &physical_memory[next];

        // check for 1gb pages
        if(supports_1gb_paging && (table[PAGING_PML3E_OFFSET(virtual_address)] & PAGING_PAGE_SIZE_BIT) != 0) {
            return (table[PAGING_PML3E_OFFSET(virtual_address)] & PAGING_1GB_PML3_MASK) + PAGING_1GB_PAGE_OFFSET(virtual_address);
        }
    }

    // get the pml2
    {
        uint64_t next = get_page(&table[PAGING_PML3E_OFFSET(virtual_address)], (page_attrs_t){0});
        if(next == 0) return (uintptr_t) -1;
        table = (uint64_t *) &physical_memory[next];

        // check for 2mb pages
        if((table[PAGING_PML2E_OFFSET(virtual_address)] & PAGING_PAGE_SIZE_BIT) != 0) {
            return (table[PAGING_PML2E_OFFSET(virtual_address)] & PAGING_2MB_PML2_MASK) + PAGING_2MB_PAGE_OFFSET(virtual_address);
        }
    }

    // get the pml1
    {
        uint64_t next = get_page(&table[PAGING_PML2E_OFFSET(virtual_address)], (page_attrs_t){0});
        if(next == 0) return (uintptr_t) -1;
        table = (uint64_t *) &physical_memory[next];
    }

    // check if the entry exists
    if((table[PAGING_PML1E_OFFSET(virtual_address)] & PAGING_PRESENT_BIT) == 0) return (uintptr_t)-1;

    // return it
    return (table[PAGING_PML1E_OFFSET(virtual_address)] & PAGING_4KB_ADDR_MASK) + PAGING_4KB_PAGE_OFFSET(virtual_address);
}

static error_t internal_map(address_space_t address_space, void* virtual_address, void* physical_address, page_attrs_t attributes) {
    error_t err = NO_ERROR;
    uint64_t pt = 0;
    uint64_t* table = NULL;

    // get the page table
    CHECK_AND_RETHROW(get_or_create_pml1(address_space, (uint64_t) virtual_address, attributes, &pt));
    table = (uint64_t *) &physical_memory[pt];

    // make sure nothing has mapped this page
    CHECK_ERROR((table[PAGING_PML1E_OFFSET(virtual_address)] & PAGING_PRESENT_BIT) == 0, ERROR_ALREADY_MAPPED);

    // set the entry
    table[PAGING_PML1E_OFFSET(virtual_address)] = PAGING_PRESENT_BIT | ((uint64_t)physical_address & PAGING_4KB_ADDR_MASK);
    set_attributes(&table[PAGING_PML1E_OFFSET(virtual_address)], attributes);
    set_cache(&table[PAGING_PML1E_OFFSET(virtual_address)], attributes);
    _barrier();

cleanup:
    return err;

}

/**
 * Will map the given virtual address to the 1gb continues address
 *
 * @remark
 * This only works if 1gb pages are enabled
 */
static error_t internal_map_1gb(address_space_t address_space, uintptr_t virt_addr, uintptr_t phys_addr, page_attrs_t attributes) {
    error_t err = NO_ERROR;

    uint64_t* table = (uint64_t*)&physical_memory[address_space];

    // make sure we have this
    CHECK_ERROR(supports_1gb_paging, ERROR_NOT_SUPPORTED);

    // get pml3
    {
        uint64_t next = 0;
        CHECK_AND_RETHROW(get_or_create_page((uint64_t*)&table[PAGING_PML4E_OFFSET(virt_addr)], attributes, &next));
        table = (uint64_t *) &physical_memory[next];
    }

    // get the pml3e
    CHECK_ERROR((table[PAGING_PML3E_OFFSET(virt_addr)] & PAGING_PRESENT_BIT) == 0, ERROR_ALREADY_MAPPED);

    // set the page attributes
    table[PAGING_PML3E_OFFSET(virt_addr)] = PAGING_PRESENT_BIT | PAGING_PAGE_SIZE_BIT;
    table[PAGING_PML3E_OFFSET(virt_addr)] |= phys_addr & PAGING_1GB_PML3_MASK;
    set_attributes(&table[PAGING_PML3E_OFFSET(virt_addr)], attributes);
    set_cache(&table[PAGING_PML3E_OFFSET(virt_addr)], attributes);
    _barrier();

cleanup:
    return err;
}

/**
 * Will map the given virtual address to the 2mb continues address
 */
static error_t internal_map_2mb(address_space_t address_space, uintptr_t virt_addr, uintptr_t phys_addr, page_attrs_t attributes) {
    error_t err = NO_ERROR;

    uint64_t* table = (uint64_t*)&physical_memory[address_space];

    // get pml3
    {
        uint64_t next = 0;
        CHECK_AND_RETHROW(get_or_create_page((uint64_t*)&table[PAGING_PML4E_OFFSET(virt_addr)], attributes, &next));
        table = (uint64_t *) &physical_memory[next];
    }

    // get the pml2
    {
        uint64_t next = 0;
        CHECK_AND_RETHROW(get_or_create_page((uint64_t*)&table[PAGING_PML3E_OFFSET(virt_addr)], attributes, &next));
        table = (uint64_t*) &physical_memory[next];
    }

    // get the pml2e
    CHECK_ERROR((table[PAGING_PML2E_OFFSET(virt_addr)] & PAGING_PRESENT_BIT) == 0, ERROR_ALREADY_MAPPED);

    // set the page attributes
    table[PAGING_PML2E_OFFSET(virt_addr)] = PAGING_PRESENT_BIT | PAGING_PAGE_SIZE_BIT;
    table[PAGING_PML2E_OFFSET(virt_addr)] |= phys_addr & PAGING_2MB_PML2_MASK;
    set_attributes(&table[PAGING_PML2E_OFFSET(virt_addr)], attributes);
    set_cache(&table[PAGING_PML2E_OFFSET(virt_addr)], attributes);
    _barrier();

cleanup:
    return err;
}

/**
 * This will use the internal_map_1gb/internal_map_2mb to map the given continues physical memory
 * into virtual memory using big pages in order to save on space and time.
 *
 * This will take care of alignment by aligning down the start as needed and aligning up the end as needed
 *
 * TODO: Allow to mix 2mb and 1gb pages
 *
 * @remark
 * For right now we are not using 1GB pages because we want the PAT to work without setting 1GB range to WC
 *
 * @remark
 * This also deals with the edge case where the end of memory is reached (and overflows to 0)
 */
static error_t internal_map_big_pages(address_space_t address_space, uintptr_t virt_addr, uintptr_t phys_addr, size_t length, page_attrs_t attributes) {
    error_t err = NO_ERROR;

//    if(supports_1gb_paging) {
//        phys_addr = ALIGN_DOWN(phys_addr, GB(1));
//
//        uint64_t max = UINT64_MAX;
//        if(ALIGN_UP(virt_addr + length, GB(1)) != 0) {
//            max = ALIGN_UP(virt_addr + length, GB(1));
//        }
//
//        for(uintptr_t addr = ALIGN_DOWN(virt_addr, GB(1)); addr < max; addr += GB(1), phys_addr += GB(1)) {
//            if(addr == 0) {
//                break;
//            }
//            if(internal_virt_to_phys(address_space, addr) == -1) {
//                CHECK_AND_RETHROW(internal_map_1gb(address_space, addr, phys_addr, attributes));
//            }
//        }
//    }else {
    phys_addr = ALIGN_DOWN(phys_addr, MB(2));

    uint64_t max = UINT64_MAX;
    if(ALIGN_UP(virt_addr + length, MB(2)) != 0) {
        max = ALIGN_UP(virt_addr + length, MB(2));
    }

    for(uintptr_t addr = ALIGN_DOWN(virt_addr, MB(2)); addr < max; addr += MB(2), phys_addr += MB(2)) {
        if(addr == 0) {
            break;
        }
        if(internal_virt_to_phys(address_space, addr) == -1) {
            CHECK_AND_RETHROW(internal_map_2mb(address_space, addr, phys_addr, attributes));
        }
    }
//    }

cleanup:
    return err;
}

/**
 * Invalidate a page
 *
 * This should be used whenever we change the mapping for a page, noteable when unmapping a page.
 *
 * @param addr  [IN]    The page to invalidate
 *
 * @see https://www.felixcloutier.com/x86/invlpg
 *
 */
static inline void invlpg(uintptr_t addr) {
    asm volatile ( "invlpg (%0)" : : "b"(addr) : "memory" );
}

////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////

error_t vmm_init(tboot_info_t* info) {
    error_t err = NO_ERROR;

    cpu_features_t features;
    extended_cpu_features_t exfeatures;
    get_cpu_features(&features);
    get_extended_cpu_features(&exfeatures);

    // enable whatever features we wanna use
    log_info("Enabling features");

    log_info("\t* No execute");
    CHECK_ERROR_TRACE(exfeatures.nx, ERROR_NOT_SUPPORTED, "NX Bit support is required!");
    efer_t efer = (efer_t){.raw = _rdmsr(IA32_EFER)};
    efer.no_execute_enable = true;
    _wrmsr(IA32_EFER, efer.raw);

    log_info("\t* Write Protect");
    set_cr0(get_cr0() | CR0_WRITE_PROTECT);

    if(exfeatures.pdpe1gb) {
        log_info("\t* 1GB paging");
        supports_1gb_paging = true;
    }else {
        log_warn("\t* 1GB paging not available");
        supports_1gb_paging = false;
    }

    if(features.pge) {
        log_info("\t* Global pages");
        set_cr4(get_cr4() | CR4_PAGE_GLOBAL_ENABLED);
        supports_global_pages = true;
    }else {
        log_warn("\t* Global pages are not available");
        supports_global_pages = false;
    }

    if(features.pat) {
        log_info("\t* PAT (WC available)");
        /**
         * 00: Uncacheable
         * 01: Write combining
         * 04: Write through
         * 06: Write back
         *
         * PAT0: Write back
         * PAT1: Write through
         * PAT2: Write combining (Uncacheable by default)
         * PAT3: Uncacheable
         * PAT4: Write back
         * PAT5: Write through
         * PAT6: Write combining (Uncacheable by default)
         * PAT7: Uncacheable
         */
        _wrmsr(IA32_PAT, 0x0604010006040100ul);
        supports_pat = true;
    }else {
        log_warn("\t* No PAT (WC unavailable)");
        supports_pat = false;
    }

    physical_memory = 0;

    // allocate the kernel memory map
    CHECK_AND_RETHROW(pmm_allocate(&kernel_address_space ));
    memset((void *)kernel_address_space , 0, KB(4));

    // TODO: Use large pages as optimization
    log_info("creating direct memory map");
    for(tboot_mmap_entry_t* it = info->mmap.entries; it < info->mmap.entries + info->mmap.count; it++) {
        switch(it->type) {
            case TBOOT_MEMORY_TYPE_USABLE:
            case TBOOT_MEMORY_TYPE_ACPI_NVS:
            case TBOOT_MEMORY_TYPE_ACPI_RECLAIM:
            case TBOOT_MEMORY_TYPE_RESERVED: {
                CHECK_AND_RETHROW(internal_map_big_pages(kernel_address_space, CONVERT_TO_DIRECT(it->addr), it->addr, it->len, (page_attrs_t){ .write = true }));
            } break;
            default: break;
        }
    }

    log_info("mapping the kernel");
    // TODO: Use the kernel elf header to set the attributes correctly
    // TODO: For the kernel we might not use big pages so we can have better protections :smug:
    CHECK_AND_RETHROW(internal_map_big_pages(kernel_address_space, KERNEL_PHYSICAL_START + KERNEL_MAPPING_BASE, KERNEL_PHYSICAL_START, KERNEL_PHYSICAL_END - KERNEL_PHYSICAL_START, (page_attrs_t){ .global = true, .write = true, .execute = true }));

    vmm_set(kernel_address_space);

    physical_memory += DIRECT_MAPPING_BASE;

cleanup:
    // we are not doing to clean anything because failing this stage
    // means we can not continue running
    return err;
}

void vmm_set(address_space_t address_space) {
    set_cr3(address_space);
}

address_space_t vmm_get() {
    return get_cr3();
}

error_t vmm_map(address_space_t address_space, void* virtual_address, void* physical_address, page_attrs_t attributes) {
    error_t err = NO_ERROR;

    lock_preemption(&vmm_lock);

    if(physical_address == 0) {
        log_warn("Mapping physical address 0!");
    }

    CHECK_AND_RETHROW(internal_map(address_space, virtual_address, physical_address, attributes));

cleanup:
    unlock_preemption(&vmm_lock);
    return err;
}

error_t vmm_map_direct(uintptr_t physical_start, size_t size, page_attrs_t attributes) {
    error_t err = NO_ERROR;

    lock_preemption(&vmm_lock);

    // make sure in range, takes care of overlap
    CHECK_TRACE(CONVERT_TO_DIRECT(physical_start + size) <= MM_BASE && CONVERT_TO_DIRECT(physical_start + size) >= DIRECT_MAPPING_BASE, "Could not identity map physical memory range 0x%016p-0x%016p (overlapped with kernel heap/code)", physical_start, physical_start + size);

    // map it
    CHECK_AND_RETHROW(internal_map_big_pages(kernel_address_space, CONVERT_TO_DIRECT(physical_start), physical_start, size, attributes));

    uintptr_t got = internal_virt_to_phys(kernel_address_space, CONVERT_TO_DIRECT(physical_start));
    CHECK(got == physical_start);

cleanup:
    unlock_preemption(&vmm_lock);
    return err;
}

error_t vmm_unmap(address_space_t address_space, void* virtual_address) {
    error_t err = NO_ERROR;
    uint64_t* table = NULL;

    lock_preemption(&vmm_lock);

    // get the page table
    uint64_t pt = get_pml1(address_space, (uint64_t) virtual_address, (page_attrs_t){0});
    CHECK_ERROR(pt, ERROR_NOT_MAPPED);
    table = (uint64_t *) &physical_memory[pt];

    // unset the given page
    uint64_t addr = table[PAGING_PML1E_OFFSET(virtual_address)] & PAGING_4KB_ADDR_MASK;
    CHECK_ERROR(table[PAGING_PML1E_OFFSET(virtual_address)] & PAGING_PRESENT_BIT, ERROR_NOT_MAPPED);
    table[PAGING_PML1E_OFFSET(virtual_address)] = 0;
    invlpg(addr);

cleanup:
    unlock_preemption(&vmm_lock);
    return err;
}

error_t vmm_allocate(address_space_t address_space, void* virtual_address, page_attrs_t attributes) {
    error_t err = NO_ERROR;
    uint64_t phys = 0;

    lock_preemption(&vmm_lock);

    CHECK_AND_RETHROW(pmm_allocate(&phys));
    CHECK_AND_RETHROW(internal_map(address_space, virtual_address, (void*)phys, attributes));

cleanup:
    if(err != NO_ERROR) {
        if(phys) pmm_free(phys);
    }
    unlock_preemption(&vmm_lock);
    return err;
}

error_t vmm_free(address_space_t address_space, void* virtual_address) {
    error_t err = NO_ERROR;
    uint64_t* table = NULL;

    lock_preemption(&vmm_lock);

    // get the page table
    uint64_t pt = get_pml1(address_space, (uint64_t) virtual_address, (page_attrs_t){0});
    CHECK_ERROR(pt, ERROR_NOT_MAPPED);
    table = (uint64_t *) &physical_memory[pt];

    // unset the given page
    uint64_t addr = table[PAGING_PML1E_OFFSET(virtual_address)] & PAGING_4KB_ADDR_MASK;
    CHECK_ERROR(table[PAGING_PML1E_OFFSET(virtual_address)] & PAGING_PRESENT_BIT, ERROR_NOT_MAPPED);
    CATCH(pmm_free(addr));
    table[PAGING_PML1E_OFFSET(virtual_address)] = 0;
    invlpg(addr);

cleanup:
    unlock_preemption(&vmm_lock);
    return err;
}

// TODO: add locks (will need to not use internally)
error_t vmm_virt_to_phys(address_space_t address_space, uintptr_t virtual_address, uintptr_t* physical_address) {
    error_t err = NO_ERROR;

    lock_preemption(&vmm_lock);

    uintptr_t addr = internal_virt_to_phys(address_space, virtual_address);
    CHECK_ERROR(addr != (uintptr_t)-1, ERROR_NOT_MAPPED);

    if(physical_address) *physical_address = addr;

cleanup:
    unlock_preemption(&vmm_lock);
    return err;
}

bool vmm_is_mapped(address_space_t address_space, uintptr_t virtual_address) {
    lock_preemption(&vmm_lock);
    bool is_mapped = internal_virt_to_phys(address_space, virtual_address) != -1;
    unlock_preemption(&vmm_lock);
    return is_mapped;
}