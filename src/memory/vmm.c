#include "vmm.h"
#include "pmm.h"

#include <common.h>
#include <string.h>
#include <boot/multiboot.h>
#include <elf64.h>
#include <cpu/cr.h>

////////////////////////////////////////////////////////////////////////////
// Page attributes
////////////////////////////////////////////////////////////////////////////

#define PAGING_PRESENT_BIT (1ul << 0ul)
#define PAGING_READ_WRITE_BIT (1ul << 1ul)
#define PAGING_USER_BIT (1ul << 2ul)
#define PAGING_ACCESSED_BIT (1ul << 5ul)
#define PAGING_DIRTY_BIT (1ul << 6ul)
#define PAGING_PAGE_SIZE_BIT (1ul << 7ul)
#define PAGING_NO_EXECUTE_BIT (1ul << 63ul)

////////////////////////////////////////////////////////////////////////////
// Masks for the table entries
////////////////////////////////////////////////////////////////////////////

// ----------------------------------
// When using 1GB pages
// ----------------------------------
#define PAGING_1GB_PDPE_MASK    (0x7FFFFFFFC0000000)
#define PAGING_1GB_PML4_MASK    (0x7FFFFFFFFFFFF000)

// ----------------------------------
// When using 2MB pages
// ----------------------------------
#define PAGING_2MB_PDE_MASK     (0x7FFFFFFFFFE00000)
#define PAGING_2MB_PDPE_MASK    (0x7FFFFFFFFFFFF000)
#define PAGING_2MB_PML4_MASK    (0x7FFFFFFFFFFFF000)

// ----------------------------------
// When using 4KB pages
// ----------------------------------
#define PAGING_4KB_ADDR_MASK    (0x7FFFFFFFFFFFF000u)

////////////////////////////////////////////////////////////////////////////
// Index addresses
////////////////////////////////////////////////////////////////////////////

#define PAGING_PML4_OFFSET(addr) (((uint64_t)(addr) >> 39u) & 0x1FF)
#define PAGING_PDPE_OFFSET(addr) (((uint64_t)(addr) >> 30u) & 0x1FF)
#define PAGING_PDE_OFFSET(addr) (((uint64_t)(addr) >> 21u) & 0x1FF)
#define PAGING_PTE_OFFSET(addr) (((uint64_t)(addr) >> 12u) & 0x1FF)
#define PAGING_PAGE_OFFSET(addr) (((uint64_t)(addr) & 0xFFF))

////////////////////////////////////////////////////////////////////////////
// Index addresses
////////////////////////////////////////////////////////////////////////////

/*
 * This is the physical memory start
 *
 * this is set to 0 at the start and later on is set to 0xFFFF800000000000
 */
static char* physical_memory = 0;

/*
 * This contains the paging of the kernel, when creating new address spaces
 * we are going to copy from this.
 */
static uint64_t kernel_paging;

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
static inline void set_attributes(uint64_t* entry, int attributes) {
    if(attributes == 0) return;

    if(!(*entry & PAGING_USER_BIT) && (attributes & PAGE_ATTR_USER)) {
        *entry |= PAGING_USER_BIT;
    }
    if(!(*entry & PAGING_READ_WRITE_BIT) && (attributes & PAGE_ATTR_WRITE)) {
        *entry |= PAGING_READ_WRITE_BIT;
    }
    if((*entry & PAGING_NO_EXECUTE_BIT) && (attributes & PAGE_ATTR_EXECUTE)) {
        *entry &= ~PAGING_NO_EXECUTE_BIT;
    }
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
static uint64_t get_page(uint64_t *entry, int attributes) {
    if(*entry & PAGING_PRESENT_BIT) {
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
static error_t get_or_create_page(uint64_t *entry, int attributes, uint64_t *out_page) {
    error_t err = NO_ERROR;
    uint64_t new_page = get_page(entry, attributes);

    if(!new_page) {
        // allocate and zero new page
        CHECK_AND_RETHROW(pmm_allocate(&new_page));
        memset(&physical_memory[new_page], 0, KB(4));

        // set the entry
        // no execute by default
        *entry = PAGING_PRESENT_BIT | PAGING_NO_EXECUTE_BIT | (new_page & PAGING_4KB_ADDR_MASK);
        set_attributes(entry, attributes);
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
static uint64_t get_pt(uint64_t pml4, uint64_t virt_addr, int attributes) {
    uint64_t* table = (uint64_t *) &physical_memory[pml4];

    // get pdp
    {
        uint64_t next = get_page(&table[PAGING_PML4_OFFSET(virt_addr)], attributes);
        if(next == 0) return NULL;
        table = (uint64_t *) &physical_memory[next];
    }

    // get the pd
    {
        uint64_t next = get_page(&table[PAGING_PDPE_OFFSET(virt_addr)], attributes);
        if(next == 0) return NULL;
        table = (uint64_t *) &physical_memory[next];
    }

    // get the pt
    {
        uint64_t next = get_page(&table[PAGING_PDE_OFFSET(virt_addr)], attributes);
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
static error_t get_or_create_pt(uint64_t pml4, uint64_t virt_addr, int attributes, uint64_t* pt) {
    error_t err = NO_ERROR;

    uint64_t* table = &physical_memory[pml4];

    // get pdp
    {
        uint64_t next;
        CHECK_AND_RETHROW(get_or_create_page((uint64_t*)&table[PAGING_PML4_OFFSET(virt_addr)], attributes, &next));
        table = (uint64_t *) &physical_memory[next];
    }

    // get the pd
    {
        uint64_t next;
        CHECK_AND_RETHROW(get_or_create_page((uint64_t*)&table[PAGING_PDPE_OFFSET(virt_addr)], attributes, &next));
        table = (uint64_t*) &physical_memory[next];
    }

    // get the pt
    {
        uint64_t next;
        CHECK_AND_RETHROW(get_or_create_page((uint64_t*)&table[PAGING_PDE_OFFSET(virt_addr)], attributes, &next));
        *pt = (uint64_t) next;
    }

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

error_t vmm_init(multiboot_info_t* info) {
    error_t err = NO_ERROR;

    uint64_t total_phys_mem = (info->mem_upper + info->mem_lower) * KB(1);
    uint64_t total_phys_mem_kb = ALIGN_UP(total_phys_mem, KB(4)) / KB(4);
    uint64_t kpml4;

    physical_memory = 0;

    CHECK_AND_RETHROW(pmm_allocate(&kpml4));
    memset((void *)kpml4, 0, KB(4));

    log_debug("mapping the physical memory");
    for(int i = 0; i < total_phys_mem_kb; i++) {
        CHECK_AND_RETHROW(vmm_map(kpml4, (void *) (i * KB(4) + 0xFFFF800000000000), (void *) (i * KB(4)), PAGE_ATTR_WRITE));
    }

    log_debug("mapping the kernel");

    // TODO: Use the kernel elf header to set the attributes correctly

    for(uint64_t addr = ALIGN_DOWN(KERNEL_PHYSICAL_START, KB(4)); addr < ALIGN_UP(KERNEL_PHYSICAL_END, KB(4)); addr += KB(4)) {
        CHECK_AND_RETHROW(vmm_map(kpml4, (void *)(addr + 0xFFFFFFFFC0000000), (void *) (addr), PAGE_ATTR_WRITE | PAGE_ATTR_EXECUTE));
    }

    kernel_paging = kpml4;
    vmm_set(kpml4);

    physical_memory += 0xFFFF800000000000;

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

error_t vmm_map(address_space_t address_space, void* virtual_address, void* physical_address, int attributes) {
    error_t err = NO_ERROR;
    uint64_t pt = 0;
    uint64_t* table = NULL;

    // get the page table
    CHECK_AND_RETHROW(get_or_create_pt(address_space, (uint64_t)virtual_address, attributes, &pt));
    table = (uint64_t *) &physical_memory[pt];

    // make sure nothing has mapped this page
    CHECK_ERROR(!(table[PAGING_PTE_OFFSET(virtual_address)] & PAGING_PRESENT_BIT), ERROR_ALREADY_MAPPED);

    // set the entry
    table[PAGING_PTE_OFFSET(virtual_address)] = PAGING_PRESENT_BIT | ((uint64_t)physical_address & PAGING_4KB_ADDR_MASK);
    set_attributes(&table[PAGING_PTE_OFFSET(virtual_address)], attributes);

cleanup:
    return err;
}

error_t vmm_unmap(address_space_t address_space, void* virtual_address) {
    error_t err = NO_ERROR;
    uint64_t* table = NULL;

    // get the page table
    uint64_t pt = get_pt(address_space, (uint64_t)virtual_address, 0);
    CHECK_ERROR(pt, ERROR_NOT_MAPPED);
    table = (uint64_t *) &physical_memory[pt];

    // unset the given page
    uint64_t addr = table[PAGING_PTE_OFFSET(virtual_address)] & PAGING_4KB_ADDR_MASK;
    CHECK_ERROR(table[PAGING_PTE_OFFSET(virtual_address)] & PAGING_PRESENT_BIT, ERROR_NOT_MAPPED);
    table[PAGING_PTE_OFFSET(virtual_address)] = 0;
    invlpg(addr);

cleanup:
    return err;
}

error_t vmm_allocate(address_space_t address_space, void* virtual_address, int attributes) {
    error_t err = NO_ERROR;
    uint64_t phys = 0;

    CHECK_AND_RETHROW(pmm_allocate(&phys));
    CHECK_AND_RETHROW(vmm_map(address_space, virtual_address, (void*)phys, attributes));

cleanup:
    if(err != NO_ERROR) {
        if(phys) pmm_free(phys);
    }
    return err;
}