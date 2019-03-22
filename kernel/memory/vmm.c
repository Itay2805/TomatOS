#include <common/common.h>
#include <common/kernel_info.h>
#include <graphics/term.h>
#include <common/string.h>
#include "vmm.h"

#include "pmm.h"

////////////////////////////////////////////////////////////////////////////
// Page attributes
////////////////////////////////////////////////////////////////////////////

#define PAGING_PRESENT_BIT (1ul << 0ul)
#define PAGING_READ_WRITE_BIT (1ul << 1ul)
#define PAGING_USER_SUPREVISOR_BIT (1ul << 2ul)
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

address_space_t boot_address_space;
address_space_t kernel_address_space;

/**
 * This bitmap is used to check what physical pages have been allocated by the vmm
 * and which have been just mapped
 */
uint64_t* bitmap;

/**
 * The size of the bitmap
 */
size_t bitmap_size;


/**
 * This is mapped to the pte that contains the free page inside it,
 */
uint64_t* pte_for_free_page;

/**
 * This is a free page we know we can always map to something else
 */
char* free_page;

/**
 * Will set the physical address as allocated by us
 */
void set_allocated(uintptr_t physical_address) {
    bitmap[physical_address / sizeof(size_t)] |= (1u << (physical_address % sizeof(size_t)));
}

void set_free(uintptr_t physical_address) {
    bitmap[physical_address / sizeof(size_t)] &= ~(1u << (physical_address % sizeof(size_t)));
}

/**
 * Will check if the given physical address was allocated by the vmm or not
 */
bool is_allocated(uintptr_t physical_address) {
    return (bool) ((bitmap[physical_address / sizeof(size_t)] & (1u << (physical_address % sizeof(size_t)))) != 0);
}

////////////////////////////////////////////////////////////////////////////
// Early mapping (before all address spaces has been set)
////////////////////////////////////////////////////////////////////////////

uint64_t* early_get(const uint64_t* table, size_t index) {
    if((table[index] & PAGING_PRESENT_BIT) != 0) {
        return (uint64_t *)(table[index] & PAGING_4KB_ADDR_MASK);
    }else {
        return 0;
    }
}

/**
 * Will set the attributes for the entry, taking into account permission precedence
 */
void set_attributes(uint64_t* entry, page_attributes_t attrs) {
    if((*entry & PAGING_READ_WRITE_BIT) == 0 && attrs.write) {
        *entry |= PAGING_READ_WRITE_BIT;
    }
    if((*entry & PAGING_USER_SUPREVISOR_BIT) == 0 && attrs.user) {
        *entry |= PAGING_USER_SUPREVISOR_BIT;
    }
    if((*entry & PAGING_NO_EXECUTE_BIT) != 0 && attrs.execute) {
        *entry &= ~(PAGING_NO_EXECUTE_BIT);
    }
}

uint64_t* early_get_or_create(uint64_t* table, size_t index, page_attributes_t attrs) {
    uint64_t* ptr = early_get(table, index);
    if(ptr != 0) {
        set_attributes(&table[index], attrs);
        return ptr;
    }else {
        // not presented, allocate table
        uint64_t* physical_address = pmm_allocate(1);
        memset(physical_address, 0, 4096);
        set_allocated((uintptr_t) physical_address);

        // set the new address
        table[index] = (uintptr_t)physical_address & PAGING_4KB_ADDR_MASK;
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
void early_map(address_space_t address_space, uintptr_t virtual_addr, uintptr_t physical_addr, page_attributes_t attrs) {
    uint64_t* pml4 = address_space;

    // get the tables, allocate if needed to
    uint64_t* pdp = early_get_or_create(pml4, PAGING_PML4_OFFSET(virtual_addr), attrs);
    uint64_t* pd = early_get_or_create(pdp, PAGING_PDPE_OFFSET(virtual_addr), attrs);
    uint64_t* pt = early_get_or_create(pd, PAGING_PDE_OFFSET(virtual_addr), attrs);
    uint64_t* phys = early_get(pt, PAGING_PTE_OFFSET(virtual_addr));

    // check if we should free the current physical page there
    if(phys != 0 && is_allocated((uintptr_t) phys)) {
        pmm_free(phys);
        set_free((uintptr_t) phys);
    }

    // set the entry
    pt[PAGING_PTE_OFFSET(virtual_addr)] = physical_addr & PAGING_4KB_ADDR_MASK;
    pt[PAGING_PTE_OFFSET(virtual_addr)] |= PAGING_PRESENT_BIT;
    set_attributes(&pt[PAGING_PTE_OFFSET(virtual_addr)], attrs);
}

////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////

/**
 * The label to the boot pml4
 */
extern void* boot_pml4;

void vmm_init(multiboot_info_t* multiboot) {
    // set the bitmap
    bitmap = (size_t*)(uintptr_t)end_of_kernel;
    bitmap_size = (ALIGN_UP((multiboot->mem_lower + multiboot->mem_upper) * 1024, 4096u) / 4096) / sizeof(size_t);
    end_of_kernel += bitmap_size;

    // set the virtual addresses for the free page and pte for free page
    end_of_kernel = ALIGN_UP(end_of_kernel, 4096u);
    free_page = (char*)(uintptr_t)end_of_kernel;
    pte_for_free_page = (uint64_t *)(free_page + 4096u);

    // now map the kernel itself in the physical pages
    // we could not have done it until now because we had to allocate
    // the other bitmap first
    pmm_map_kernel();

    // set the boot address space
    boot_address_space = &boot_pml4;

    // create the kernel address space
    term_print("[vmm_init] Creating kernel address space\n");
    kernel_address_space = pmm_allocate(1);
    memset(kernel_address_space, 0, 4096);

    // Map the free page to 0 and the pte to the pte
    term_print("[vmm_init] \tMapping free page\n");
    early_map(kernel_address_space, (uintptr_t)free_page, 0, (page_attributes_t){ .write = true, .user = false, .execute = false });
    uint64_t* pdp = early_get(kernel_address_space, PAGING_PML4_OFFSET(free_page));
    uint64_t* pd = early_get(pdp, PAGING_PDPE_OFFSET(free_page));
    uint64_t* pt = early_get(pd, PAGING_PDE_OFFSET(free_page));
    early_map(kernel_address_space, (uintptr_t)pte_for_free_page, (uintptr_t)pt, (page_attributes_t){ .write = true, .user = false, .execute = false });

    // identity map the kernel now
    term_print("[vmm_init] \tIdentity mapping kernel\n");
    for(uintptr_t addr = ALIGN_DOWN(KERNEL_START, 4096u); addr < ALIGN_UP(end_of_kernel, 4096u); addr += 4096) {
        // TODO: Parse the kernel elf file properly to setup the correct attributes
        early_map(kernel_address_space, addr, addr, (page_attributes_t){ .write = true, .user = false, .execute = true });
    }

    // and now we can add the stuff to the end of the kernel
    end_of_kernel += 4096 * 2;

    // map the frame buffer
    uintptr_t framebuffer_end = multiboot->framebuffer_addr + multiboot->framebuffer_width * multiboot->framebuffer_height * (multiboot->framebuffer_bpp / 8);
    term_print("[vmm_init] \tIdentity mapping framebuffer\n");
    for(uintptr_t addr = ALIGN_DOWN(multiboot->framebuffer_addr, 4096u); addr < ALIGN_UP(framebuffer_end, 4096u); addr += 4096) {
        early_map(kernel_address_space, addr, addr, (page_attributes_t){ .write = true, .user = false, .execute = false });
    }

    term_print("[vmm_init] Switching from boot address space to kernel address space\n");
    vmm_set(kernel_address_space);
}