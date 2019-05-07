#include "vmm.h"

#include <common/common.h>
#include <common/kernel_info.h>
#include <graphics/term.h>
#include <common/string.h>
#include <cpu/control.h>
#include <cpu/msr.h>
#include <kernel.h>
#include "pmm.h"

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
// Variables and helper functions
////////////////////////////////////////////////////////////////////////////

/**
 * Invalidate a page
 */
static inline void invlpg(uintptr_t addr) {
    asm volatile ( "invlpg (%0)" : : "b"(addr) : "memory" );
}

address_space_t boot_address_space;
address_space_t kernel_address_space;

/**
 * This bitmap is used to check what physical pages have been allocated by the vmm
 * and which have been just mapped
 */
static uint64_t *bitmap;

/**
 * The size of the bitmap
 */
static size_t bitmap_size;


/**
 * This is mapped to the pte that contains the free page inside it,
 */
static uint64_t *pte_for_free_page;

/**
 * This is a free page we know we can always map to something else
 */
static char *free_page;

/**
 * Has the same virtual address as the free page, but is of an array type
 * just for ease of use
 */
static uint64_t* free_table;

/**
 * Will set the physical address as allocated by us
 */
void set_allocated(uintptr_t physical_address) {
    physical_address = ALIGN_DOWN(physical_address, 4096u) / 4096;
    bitmap[physical_address / sizeof(size_t)] |= (1u << (physical_address % sizeof(size_t)));
}

void set_free(uintptr_t physical_address) {
    physical_address = ALIGN_DOWN(physical_address, 4096u) / 4096;
    bitmap[physical_address / sizeof(size_t)] &= ~(1u << (physical_address % sizeof(size_t)));
}

/**
 * Will check if the given physical address was allocated by the vmm or not
 */
bool is_allocated(uintptr_t physical_address) {
    physical_address = ALIGN_DOWN(physical_address, 4096u) / 4096;
    return (bool) ((bitmap[physical_address / sizeof(size_t)] & (1u << (physical_address % sizeof(size_t)))) != 0);
}

////////////////////////////////////////////////////////////////////////////
// Helper functions prototypes
////////////////////////////////////////////////////////////////////////////

uint64_t *get_page(const uint64_t *table, size_t index);

////////////////////////////////////////////////////////////////////////////
// Free page manipulation
////////////////////////////////////////////////////////////////////////////

/**
 * Get the physical address of the free page
 */
void *get_free_page_physical() {
    return get_page(pte_for_free_page, PAGING_PTE_OFFSET(free_page));
}

/**
 * Map a physical page to the free page, returning the free page
 */
void *map_to_free_page(void *physicalPage) {
    invlpg((uintptr_t) free_page);
    pte_for_free_page[PAGING_PTE_OFFSET(free_page)] &= ~PAGING_4KB_ADDR_MASK;
    pte_for_free_page[PAGING_PTE_OFFSET(free_page)] |= ((uintptr_t) physicalPage & PAGING_4KB_ADDR_MASK);
    return free_page;
}

////////////////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////////////////

/**
 * Will get the page from the table
 *
 * Note: This assumes the table is mapped
 */
uint64_t *get_page(const uint64_t *table, size_t index) {
    if ((table[index] & PAGING_PRESENT_BIT) != 0) {
        return (uint64_t *) (table[index] & PAGING_4KB_ADDR_MASK);
    } else {
        return 0;
    }
}

/**
 * Will set the attributes for the entry, taking into account permission precedence
 */
void set_attributes(uint64_t *entry, int attrs) {
    if ((*entry & PAGING_READ_WRITE_BIT) == 0 && (attrs & PAGE_ATTR_WRITE) != 0) {
        *entry |= PAGING_READ_WRITE_BIT;
    }
    if ((*entry & PAGING_USER_BIT) == 0 && (attrs & PAGE_ATTR_USER) != 0) {
        *entry |= PAGING_USER_BIT;
    }
    if ((*entry & PAGING_NO_EXECUTE_BIT) != 0 && (attrs & PAGE_ATTR_EXECUTE) != 0) {
        *entry &= ~(PAGING_NO_EXECUTE_BIT);
    }
}

/**
 * Will get the page from the table, will create it if it does not exists
 *
 * Note: This assumes the table is mapped
 */
uint64_t *get_or_create_page(uint64_t *table, size_t index, int attrs) {
    uint64_t *ptr = get_page(table, index);
    if (ptr != 0) {
        set_attributes(&table[index], attrs);
        return ptr;
    } else {
        // not presented, allocate table
        uint64_t *physical_address = pmm_allocate(1);

        if (vmm_get() == boot_address_space) {
            // in the boot address space we are going to assume we can just set it
            memset(physical_address, 0, 4096);
        } else {
            // we need to map the free page
            void *physical = get_free_page_physical();
            map_to_free_page(physical_address);
            memset(free_page, 0, 4096);
            map_to_free_page(physical);
        }

        set_allocated((uintptr_t) physical_address);

        // set the new address
        table[index] = (uintptr_t) physical_address & PAGING_4KB_ADDR_MASK;
        table[index] |= PAGING_PRESENT_BIT;
        set_attributes(&table[index], attrs);
        return physical_address;
    }
}

/**
 * Will map the virtual address to the physical address
 *
 * will not map the physical pages!
 */
void early_map(address_space_t address_space, uintptr_t virtual_addr, uintptr_t physical_addr, int attrs) {
    uint64_t *pml4 = address_space;

    // get the tables, allocate if needed to
    uint64_t *pdp = get_or_create_page(pml4, PAGING_PML4_OFFSET(virtual_addr), attrs);
    uint64_t *pd = get_or_create_page(pdp, PAGING_PDPE_OFFSET(virtual_addr), attrs);
    uint64_t *pt = get_or_create_page(pd, PAGING_PDE_OFFSET(virtual_addr), attrs);
    uint64_t *phys = get_page(pt, PAGING_PTE_OFFSET(virtual_addr));

    // check if we should free the current physical page there
    if (phys != 0 && is_allocated((uintptr_t) phys)) {
        pmm_free(phys);
        set_free((uintptr_t) phys);
    }

    // set the entry
    invlpg(virtual_addr);
    pt[PAGING_PTE_OFFSET(virtual_addr)] = physical_addr & PAGING_4KB_ADDR_MASK;
    pt[PAGING_PTE_OFFSET(virtual_addr)] |= PAGING_PRESENT_BIT;
    if ((attrs & PAGE_ATTR_WRITE) != 0) {
        pt[PAGING_PTE_OFFSET(virtual_addr)] |= PAGING_READ_WRITE_BIT;
    }
    if ((attrs & PAGE_ATTR_USER) != 0) {
        pt[PAGING_PTE_OFFSET(virtual_addr)] |= PAGING_USER_BIT;
    }
    if ((attrs & PAGE_ATTR_EXECUTE) == 0) {
        pt[PAGING_PTE_OFFSET(virtual_addr)] |= PAGING_NO_EXECUTE_BIT;
    }
}

/**
 * Will return the pointer to the page table, will create the needed pages if does not exists
 */
uint64_t* get_or_create_page_table(address_space_t address_space, void *virtual_addr, int attributes) {
    uint64_t *pml4 = address_space;

    // get the tables, allocate if needed to
    map_to_free_page(pml4);
    uint64_t *pdp = get_or_create_page(free_table, PAGING_PML4_OFFSET(virtual_addr), attributes);

    map_to_free_page(pdp);
    uint64_t *pd = get_or_create_page(free_table, PAGING_PDPE_OFFSET(virtual_addr), attributes);

    map_to_free_page(pd);
    return get_or_create_page(free_table, PAGING_PDE_OFFSET(virtual_addr), attributes);
}

/**
 * Will return the pointer to the page table
 */
void *get_page_table(address_space_t address_space, const void *virtual_addr) {
    uint64_t *pml4 = address_space;
    if (pml4 == NULL) {
        return NULL;
    }

    // get the tables, allocate if needed to
    map_to_free_page(pml4);
    uint64_t *pdp = get_page((uint64_t *) free_page, PAGING_PML4_OFFSET(virtual_addr));
    if (pdp == NULL) {
        return NULL;
    }

    map_to_free_page(pdp);
    uint64_t *pd = get_page((uint64_t *) free_page, PAGING_PDPE_OFFSET(virtual_addr));
    if (pd == NULL) {
        return NULL;
    }

    map_to_free_page(pd);
    return get_page((uint64_t *) free_page, PAGING_PDE_OFFSET(virtual_addr));
}


////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////

/**
 * The label to the boot pml4
 */
extern void *boot_pml4;
extern void *boot_pdpe;

void vmm_early_init() {
    // set the boot address space
    boot_address_space = &boot_pml4;

    uint64_t* pdpe = (uint64_t *) &boot_pdpe;
    for(size_t i = 0; i < 4; i++) {
        uint64_t addr = GB(1) * i;
        pdpe[i] = (addr & PAGING_1GB_PDPE_MASK) | (PAGING_PRESENT_BIT | PAGING_READ_WRITE_BIT | PAGING_PAGE_SIZE_BIT);
    }
}

void vmm_init(multiboot_info_t *multiboot) {
    // set the bitmap
    bitmap = (size_t *) (uintptr_t) end_of_kernel;
    bitmap_size = (ALIGN_UP((multiboot->mem_lower + multiboot->mem_upper) * 1024, 4096u) / 4096) / sizeof(size_t);
    end_of_kernel += bitmap_size;

    // set the virtual addresses for the free page and pte for free page
    end_of_kernel = ALIGN_UP(end_of_kernel, KB(4));
    free_page = (char*)(uintptr_t)end_of_kernel;
    free_table = (uint64_t*)(uintptr_t)end_of_kernel;
    pte_for_free_page = (uint64_t *)(free_page + KB(4));

    // now map the kernel itself in the physical pages
    // we could not have done it until now because we had to allocate
    // the other bitmap first
    pmm_map_kernel();

    term_write("[vmm_init] Enabling features\n");
    term_write("[vmm_init] \t* Page Global Enable\n");
    set_cr4(get_cr4() | CR4_PAGE_GLOBAL_ENABLED);
    term_write("[vmm_init] \t* PCID\n");
    set_cr4(get_cr4() | CR4_PCID_ENABLED);
    term_write("[vmm_init] \t* No execute\n");
    wrmsr(MSR_EFER, rdmsr(MSR_EFER) | EFER_NO_EXECUTE_ENABLE);

    // create the kernel address space
    term_print("[vmm_init] Creating kernel address space\n");
    kernel_address_space = pmm_allocate(1);
    memset(kernel_address_space, 0, KB(4));

    // Map the free page to 0 and the pte to the pte
    uint64_t *pdp = get_or_create_page(kernel_address_space, PAGING_PML4_OFFSET(free_page), PAGE_ATTR_WRITE);
    uint64_t *pd = get_or_create_page(pdp, PAGING_PDPE_OFFSET(free_page), PAGE_ATTR_WRITE);
    uint64_t *pt = get_or_create_page(pd, PAGING_PDE_OFFSET(free_page), PAGE_ATTR_WRITE);
    term_print("[vmm_init] \tMapping free page (pt=0x%p>0x%p, page=0x%p)\n", pte_for_free_page, pt, (void *) free_page);
    early_map(kernel_address_space, (uintptr_t) pte_for_free_page, (uintptr_t) pt, PAGE_ATTR_WRITE);
    early_map(kernel_address_space, (uintptr_t) free_page, 0, PAGE_ATTR_WRITE);

    // identity map the kernel now
    term_print("[vmm_init] \tIdentity mapping kernel\n");
    for (uintptr_t addr = ALIGN_DOWN(KERNEL_START, KB(4)); addr < ALIGN_UP(end_of_kernel, KB(4)); addr += KB(4)) {
        // TODO: Parse the kernel elf file properly to setup the correct attributes
        early_map(kernel_address_space, addr, addr, PAGE_ATTR_WRITE | PAGE_ATTR_EXECUTE);
    }

    // and now we can add the stuff to the end of the kernel
    // TODO: Find a better way to do this, we will disable it for now cause otherwise we will identity map pages that should not be identity mapped
    // end_of_kernel += 4096 * 2;

    // map the frame buffer
    // TODO: Find a way to map these without keeping multiboot stuff
    uintptr_t framebuffer_end = multiboot->framebuffer_addr +
                                multiboot->framebuffer_width * multiboot->framebuffer_height *
                                (multiboot->framebuffer_bpp / 8);
    term_print("[vmm_init] \tIdentity mapping framebuffer\n");
    for (uintptr_t addr = ALIGN_DOWN(multiboot->framebuffer_addr, 4096u);
         addr < ALIGN_UP(framebuffer_end, 4096u); addr += 4096) {
        early_map(kernel_address_space, addr, addr, PAGE_ATTR_WRITE);
    }

    vmm_set(kernel_address_space);
    term_print("[vmm_init] Now using kernel address space\n");
}

void vmm_set(address_space_t addrspace) {
    set_cr3((get_cr3() & ~CR3_PML4_MASK) | (uint64_t)addrspace);
}

address_space_t vmm_get() {
    return (address_space_t) (get_cr3() & CR3_PML4_MASK);
}

address_space_t vmm_create_address_space() {
    // allocate the address space
    address_space_t address_space = pmm_allocate(1);
    map_to_free_page(address_space);
    memset(free_page, 0, KB(4));

    // Map the free page to 0 and the pte to the pte
    uint64_t *pdp = get_or_create_page((uint64_t *) free_page, PAGING_PML4_OFFSET(free_page), PAGE_ATTR_WRITE);
    map_to_free_page(pdp);
    uint64_t *pd = get_or_create_page((uint64_t *) free_page, PAGING_PDPE_OFFSET(free_page), PAGE_ATTR_WRITE);
    map_to_free_page(pd);
    uint64_t *pt = get_or_create_page((uint64_t *) free_page, PAGING_PDE_OFFSET(free_page), PAGE_ATTR_WRITE);
    map_to_free_page(pt);
    vmm_map(address_space, pte_for_free_page, pt, PAGE_ATTR_WRITE);
    vmm_map(address_space, free_page, 0, PAGE_ATTR_WRITE);

    // identity map the kernel
    for (uintptr_t addr = ALIGN_DOWN(KERNEL_START, KB(4)); addr < ALIGN_UP(end_of_kernel, KB(4)); addr += KB(4)) {
        // TODO: Parse the kernel elf file properly to setup the correct attributes
        vmm_map(address_space, (void *) addr, (void *) addr, PAGE_ATTR_WRITE | PAGE_ATTR_EXECUTE);
    }

    // map the terminal


    return address_space;
}

void vmm_free_address_space(address_space_t address_space) {
    uint64_t* pml4 = address_space;
    map_to_free_page(pml4);

    /*
     * For the tables we are just assuming we allocated them
     * only for the actual pages we check if we allocated them or not
     */

    ///------------------
    // PML4
    ///------------------
    for(uint64_t* pml4e = pml4; pml4e < pml4 + 512; pml4e++) {
        if(*pml4e & PAGING_PRESENT_BIT) {
            uint64_t* pdp = (uint64_t *) (*pml4e & PAGING_4KB_ADDR_MASK);
            map_to_free_page(pdp);

            ///------------------
            // PDP
            ///------------------
            for(uint64_t* pdpe = pdp; pdpe < pdp + 512; pdpe++) {
                if(*pml4e & PAGING_PRESENT_BIT) {
                    uint64_t *pd = (uint64_t *) (*pml4e & PAGING_4KB_ADDR_MASK);
                    map_to_free_page(pd);

                    ///------------------
                    // PD
                    ///------------------
                    for(uint64_t* pde = pd; pde < pd + 512; pde++) {
                        if(*pml4e & PAGING_PRESENT_BIT) {
                            uint64_t *pt = (uint64_t *) (*pml4e & PAGING_4KB_ADDR_MASK);
                            map_to_free_page(pt);

                            ///------------------
                            // PT
                            ///------------------
                            for(uint64_t* pte = pt; pte < pt + 512; pte++) {
                                if(*pml4e & PAGING_PRESENT_BIT) {
                                    void* phys = (uint64_t *) (*pml4e & PAGING_4KB_ADDR_MASK);

                                    // check if we should free the page
                                    if(is_allocated((uintptr_t) phys)) {
                                        set_free((uintptr_t) phys);
                                        pmm_free(phys);
                                    }
                                }
                            }

                            map_to_free_page(pd);
                            set_free((uintptr_t) pt);
                            pmm_free(pt);
                        }
                    }

                    map_to_free_page(pdp);
                    set_free((uintptr_t) pd);
                    pmm_free(pd);
                }
            }

            map_to_free_page(pml4);
            set_free((uintptr_t) pdp);
            pmm_free(pdp);
        }
    }

    set_free((uintptr_t) pml4);
    pmm_free(pml4);
}

void vmm_map(address_space_t address_space, void *virtual_addr, void *physical_addr, int attributes) {
    uint64_t* pt = get_or_create_page_table(address_space, virtual_addr, attributes);
    map_to_free_page(pt);

    uint64_t* phys = get_page(free_table, PAGING_PTE_OFFSET(virtual_addr));

    // check if we should free the current physical page there
    if (phys != 0 && is_allocated((uintptr_t) phys)) {
        pmm_free(phys);
        set_free((uintptr_t) phys);
    }

    // set the entry
    invlpg((uintptr_t)virtual_addr);
    free_table[PAGING_PTE_OFFSET(virtual_addr)] = (uintptr_t) physical_addr & PAGING_4KB_ADDR_MASK;
    free_table[PAGING_PTE_OFFSET(virtual_addr)] |= PAGING_PRESENT_BIT;
    if ((attributes & PAGE_ATTR_WRITE) != 0) {
        free_table[PAGING_PTE_OFFSET(virtual_addr)] |= PAGING_READ_WRITE_BIT;
    }
    if ((attributes & PAGE_ATTR_USER) != 0) {
        free_table[PAGING_PTE_OFFSET(virtual_addr)] |= PAGING_USER_BIT;
    }
    if ((attributes & PAGE_ATTR_EXECUTE) == 0) {
        free_table[PAGING_PTE_OFFSET(virtual_addr)] |= PAGING_NO_EXECUTE_BIT;
    }
}

void vmm_unmap(address_space_t address_space, void *virtual_addr) {
    uint64_t *pt = get_page_table(address_space, virtual_addr);
    if (pt == NULL) {
        return;
    }

    map_to_free_page(pt);
    uint64_t *phys = get_page(free_table, PAGING_PTE_OFFSET(virtual_addr));

    if (phys != 0) {
        invlpg((uintptr_t)virtual_addr);
        free_table[PAGING_PTE_OFFSET(virtual_addr)] = 0;
    }
}

void vmm_allocate(address_space_t address_space, void *virtual_addr, int attributes) {
    uint64_t *pt = get_or_create_page_table(address_space, virtual_addr, attributes);
    map_to_free_page(pt);

    uint64_t *phys = get_page(free_table, PAGING_PTE_OFFSET(virtual_addr));

    // check if we should free the current physical page there
    if (phys != 0 && is_allocated((uintptr_t) phys)) {
        pmm_free(phys);
        set_free((uintptr_t) phys);
    }

    // set the entry
    invlpg((uintptr_t)virtual_addr);
    free_table[PAGING_PTE_OFFSET(virtual_addr)] = (uintptr_t) pmm_allocate(1) & PAGING_4KB_ADDR_MASK;
    free_table[PAGING_PTE_OFFSET(virtual_addr)] |= PAGING_PRESENT_BIT;
    if ((attributes & PAGE_ATTR_WRITE) != 0) {
        free_table[PAGING_PTE_OFFSET(virtual_addr)] |= PAGING_READ_WRITE_BIT;
    }
    if ((attributes & PAGE_ATTR_USER) != 0) {
        free_table[PAGING_PTE_OFFSET(virtual_addr)] |= PAGING_USER_BIT;
    }
    if ((attributes & PAGE_ATTR_EXECUTE) == 0) {
        free_table[PAGING_PTE_OFFSET(virtual_addr)] |= PAGING_NO_EXECUTE_BIT;
    }
}

void vmm_free(address_space_t address_space, void *virtual_addr) {
    uint64_t *pt = get_page_table(address_space, virtual_addr);
    if (pt == NULL) {
        return;
    }

    map_to_free_page(pt);
    uint64_t *phys = get_page(free_table, PAGING_PTE_OFFSET(virtual_addr));

    if (phys != 0) {
        if (is_allocated((uintptr_t) phys)) {
            pmm_free(phys);
            set_free((uintptr_t) phys);
        }

        invlpg((uintptr_t)virtual_addr);
        free_table[PAGING_PTE_OFFSET(virtual_addr)] = 0;
    }
}

error_t vmm_get_physical(address_space_t address_space, const void* virtual_addr, void** physical_addr) {
    error_t err = NO_ERROR;

    uint64_t* pt = get_page_table(address_space, virtual_addr);
    CHECK_ERROR(pt != NULL, ERROR_INVALID_POINTER);

    map_to_free_page(pt);
    uint64_t *phys = get_page(free_table, PAGING_PTE_OFFSET(virtual_addr));
    CHECK_ERROR(phys != NULL, ERROR_INVALID_POINTER);

    *physical_addr = phys;

cleanup:
    return err;
}

error_t vmm_copy_to_kernel(address_space_t addrspace, const void* _from, void* _to, size_t len) {
    error_t err = NO_ERROR;
    void* physical_addr = NULL;
    char* tmp_page = NULL;
    void* orig_tmp_page_phys = NULL;
    char* ptr = NULL;
    const char* from = _from;
    char* to = _to;
    int padding;

    // check arguments
    CHECK_ERROR(addrspace != 0, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(from != NULL, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(to != NULL, ERROR_INVALID_ARGUMENT);

    // if the len is 0 do nothing (maybe we want to return an error?)
    if(len == 0) return 0;

    // setup for the alignment in the first page
    CHECK_AND_RETHROW(vmm_get_physical(addrspace, from, &physical_addr));
    CHECK_AND_RETHROW(mm_allocate_aligned(&kernel_memory_manager, KB(4), KB(4), (void**)&tmp_page));
    CHECK_AND_RETHROW(vmm_get_physical(kernel_address_space, tmp_page, &orig_tmp_page_phys));
    vmm_map(kernel_address_space, tmp_page, physical_addr, 0);
    padding = (int) ((uintptr_t)from - ALIGN_DOWN((uintptr_t)from, KB(4)));
    ptr = tmp_page + padding;

    while(len) {
        *to = *ptr;
        len--;
        if(ptr >= tmp_page + KB(4)) {
            // if we got the ptr to the end of the tmp page lets 
            // put the ptr to the start and remap the tmp page
            ptr = tmp_page;
            from += KB(4);
            CHECK_AND_RETHROW(vmm_get_physical(addrspace, from, &physical_addr));
            vmm_map(kernel_address_space, tmp_page, physical_addr, 0);
        }
    }

cleanup:
    // restore the original physical page and free the tmp page
    vmm_map(kernel_address_space, tmp_page, orig_tmp_page_phys, 0);
    mm_free(&kernel_memory_manager, tmp_page);

    return err;
}

error_t vmm_copy_string_to_kernel(address_space_t address_space, const char* from, char* to, size_t* length) {
    error_t err = NO_ERROR;
    void* physical_addr = NULL;
    char* tmp_page = NULL;
    void* orig_tmp_page_phys = NULL;
    char* ptr = NULL;
    int padding;

    // check arguments
    CHECK_ERROR(address_space != 0, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(from != NULL, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(length != NULL, ERROR_INVALID_ARGUMENT);

    // setup for the alignment in the first page
    CHECK_AND_RETHROW(vmm_get_physical(address_space, from, &physical_addr));
    CHECK_AND_RETHROW(mm_allocate_aligned(&kernel_memory_manager, KB(4), KB(4), (void**)&tmp_page));
    CHECK_AND_RETHROW(vmm_get_physical(kernel_address_space, tmp_page, &orig_tmp_page_phys));
    vmm_map(kernel_address_space, tmp_page, physical_addr, 0);
    padding = (int) ((uintptr_t)from - ALIGN_DOWN((uintptr_t)from, KB(4)));
    ptr = tmp_page + padding;

    size_t len = 0;
    while(*ptr) {
        // only copy if the to is not null and we are not exceeding the buffer
        if(to != NULL && len < *length) *to = *ptr;
        len++;
        if(ptr >= tmp_page + KB(4)) {
            // if we got the ptr to the end of the tmp page lets 
            // put the ptr to the start and remap the tmp page
            ptr = tmp_page;
            from += KB(4);
            CHECK_AND_RETHROW(vmm_get_physical(address_space, from, &physical_addr));
            vmm_map(kernel_address_space, tmp_page, physical_addr, 0);
        }
    }

    // out the actual string length
    *length = len;

cleanup:
    // restore the original physical page and free the tmp page
    if(tmp_page != NULL) {
        vmm_map(kernel_address_space, tmp_page, orig_tmp_page_phys, 0);
        mm_free(&kernel_memory_manager, tmp_page);
    }

    return err;
}

error_t vmm_copy_to_user(address_space_t addrspace, const void *_from, void *_to, size_t len) {
    error_t err = NO_ERROR;
    void *physical_addr = NULL;
    char *tmp_page = NULL;
    void *orig_tmp_page_phys = NULL;
    char *ptr = NULL;
    const char *from = _from;
    char *to = _to;
    int padding;

    // check arguments
    CHECK_ERROR(addrspace != 0, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(from != NULL, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(to != NULL, ERROR_INVALID_ARGUMENT);

    // if the len is 0 do nothing (maybe we want to return an error?)
    if (len == 0) return 0;

    // setup for the alignment in the first page
    CHECK_AND_RETHROW(vmm_get_physical(addrspace, to, &physical_addr));
    CHECK_AND_RETHROW(mm_allocate_aligned(&kernel_memory_manager, KB(4), KB(4), (void **) &tmp_page));
    CHECK_AND_RETHROW(vmm_get_physical(kernel_address_space, tmp_page, &orig_tmp_page_phys));
    vmm_map(kernel_address_space, tmp_page, physical_addr, 0);
    padding = (int) ((uintptr_t) to - ALIGN_DOWN((uintptr_t) to, KB(4)));
    ptr = tmp_page + padding;

    while (len) {
        *ptr = *from;
        len--;
        if (ptr >= tmp_page + KB(4)) {
            // if we got the ptr to the end of the tmp page lets
            // put the ptr to the start and remap the tmp page
            ptr = tmp_page;
            to += KB(4);
            CHECK_AND_RETHROW(vmm_get_physical(addrspace, to, &physical_addr));
            vmm_map(kernel_address_space, tmp_page, physical_addr, 0);
        }
    }

cleanup:
    // restore the original physical page and free the tmp page
    vmm_map(kernel_address_space, tmp_page, orig_tmp_page_phys, 0);
    mm_free(&kernel_memory_manager, tmp_page);

    return err;
}


error_t vmm_clear_user(address_space_t addrspace, void *_to, size_t len) {
    error_t err = NO_ERROR;
    void *physical_addr = NULL;
    char *tmp_page = NULL;
    void *orig_tmp_page_phys = NULL;
    char *ptr = NULL;
    char *to = _to;
    int padding;

    // check arguments
    CHECK_ERROR(addrspace != 0, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(to != NULL, ERROR_INVALID_ARGUMENT);

    // if the len is 0 do nothing (maybe we want to return an error?)
    if (len == 0) return 0;

    // setup for the alignment in the first page
    CHECK_AND_RETHROW(vmm_get_physical(addrspace, to, &physical_addr));
    CHECK_AND_RETHROW(mm_allocate_aligned(&kernel_memory_manager, KB(4), KB(4), (void **) &tmp_page));
    CHECK_AND_RETHROW(vmm_get_physical(kernel_address_space, tmp_page, &orig_tmp_page_phys));
    vmm_map(kernel_address_space, tmp_page, physical_addr, 0);
    padding = (int) ((uintptr_t) to - ALIGN_DOWN((uintptr_t) to, KB(4)));
    ptr = tmp_page + padding;

    while (len) {
        *ptr = 0;
        len--;
        if (ptr >= tmp_page + KB(4)) {
            // if we got the ptr to the end of the tmp page lets
            // put the ptr to the start and remap the tmp page
            ptr = tmp_page;
            to += KB(4);
            CHECK_AND_RETHROW(vmm_get_physical(addrspace, to, &physical_addr));
            vmm_map(kernel_address_space, tmp_page, physical_addr, 0);
        }
    }

    cleanup:
    // restore the original physical page and free the tmp page
    vmm_map(kernel_address_space, tmp_page, orig_tmp_page_phys, 0);
    mm_free(&kernel_memory_manager, tmp_page);

    return err;
}

