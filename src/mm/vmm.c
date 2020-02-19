
#include <arch/paging.h>

#include <stdbool.h>
#include <util/except.h>
#include <string.h>
#include <arch/intrin.h>
#include <arch/msr.h>
#include <arch/cpuid.h>
#include <sync/spinlock.h>
#include <proc/process.h>

#include "vmm.h"
#include "pmm.h"

static uintptr_t memory_base = 0;

static bool support_1gb = false;
static bool support_global = false;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Internal functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Will get the bottom most entry of that virtual address
 *
 * basically this will take to account the 1gb and 2mb pages
 *
 * @param handle            [IN]    The handle to the vmm
 * @param virtual_address   [IN]    The virtual address to get the entry of
 * @param entry             [OUT]   The entry (pointer, direct mapped)
 * @param page_type         [OUT]   The page type (4kb,2mb or 1gb)
 *
 * @retval true - the page is mapped
 * @retval false - the page is not mapped
 */
static bool get_entry_at_virtual_address(vmm_handle_t* handle, uintptr_t virtual_address, void** entry, page_type_t* page_type) {
    VA_ADDRESS va = { virtual_address };
    PML4E* pml4 = (PML4E*)(memory_base + handle->pml4_physical);

    PML4E pml4e = pml4[va.one_gb.pml4e];
    if(!pml4e.present) {
        return false;
    }

    PDPTE* pdpt = (PDPTE*)(memory_base + (pml4e.addr << PAGE_SHIFT));
    PDPTE pdpte = pdpt[va.one_gb.pdpte];
    if(!pdpte.present) {
        *page_type = PAGE_TYPE_1GB;
        return false;
    }

    if(pdpte.page_size) {
        *entry = &pdpt[va.one_gb.pdpte];
        *page_type = PAGE_TYPE_1GB;
        return true;
    }

    PDE* pd = (PDE*)(memory_base + (pdpte.addr << PAGE_SHIFT));
    PDE pde = pd[va.two_mb.pde];
    if(!pde.present) {
        *page_type = PAGE_TYPE_2MB;
        return false;
    }

    if(pde.page_size) {
        *entry = &pd[va.two_mb.pde];
        *page_type = PAGE_TYPE_2MB;
        return true;
    }

    PTE* pt = (PTE*)(memory_base + (pde.addr << PAGE_SHIFT));
    PTE pte = pt[va.four_kb.pte];
    if(!pte.present) {
        *page_type = PAGE_TYPE_4KB;
        return false;
    }

    *entry = &pt[va.four_kb.pte];
    *page_type = PAGE_TYPE_4KB;
    return true;
}

/**
 * Will return the size of the page in bytes based on the type
 *
 * @param type [IN] the type
 *
 * @return the size in bytes
 */
static size_t page_size_by_type(page_type_t type) {
    switch(type) {
        case PAGE_TYPE_4KB: return 4096l;
        case PAGE_TYPE_2MB: return 4096l * 512l;
        case PAGE_TYPE_1GB: return 4096l * 512l * 512l;
        default:
            ASSERT(false);
    }
}

/**
 * Will check what pat entry has the same memory type that we want
 * for this page table
 *
 * @param mem_type  [IN] The memory type to check
 * @param pwt_flag  [OUT] The out pwt flag
 * @param pcd_flag  [OUT] The out pcd flag
 * @param pat_flag  [OUT] The out pat flag
 *
 * @retval false - no such pat value, or pat is not supported
 * @retval true - found it
 */
//static bool get_pat_flags_from_mem_type(IA32_PAT_MEMTYPE mem_type, bool* pwt_flag, bool* pcd_flag, bool* pat_flag) {
//    if(support_pat) {
//
//        // Vol 3A, Table 11-11. Selection of PAT Entries with PAT, PCD, and PWT Flags
//        for(int i = 0; i < 8; i++) {
//            if(pat_types[i] == mem_type) {
//                *pwt_flag = (pat_types[i] & (1 << 0)) != 0;
//                *pcd_flag = (pat_types[i] & (1 << 1)) != 0;
//                *pat_flag = (pat_types[i] & (1 << 2)) != 0;
//                return true;
//            }
//        }
//
//        return false;
//
//    }else {
//
//        // Vol 3A, 11.12.5 PAT Compatibility with Earlier IA-32 Processors
//        switch(mem_type) {
//            case IA32_PAT_MEMTYPE_WB:
//                *pwt_flag = 0;
//                *pcd_flag = 0;
//                *pat_flag = 0;
//                return true;
//
//            case IA32_PAT_MEMTYPE_WT:
//                *pwt_flag = 1;
//                *pcd_flag = 0;
//                *pat_flag = 0;
//                return true;
//
//            case IA32_PAT_MEMTYPE_UCM:
//                *pwt_flag = 0;
//                *pcd_flag = 1;
//                *pat_flag = 0;
//                return true;
//
//            case IA32_PAT_MEMTYPE_UC:
//                *pwt_flag = 1;
//                *pcd_flag = 1;
//                *pat_flag = 0;
//                return true;
//
//            default:
//                return false;
//        }
//    }
//}

static void set_pte(
        vmm_handle_t* handle,
        uintptr_t virt, uintptr_t phys,
        bool user, bool write, bool no_exec,
        bool pwt, bool pcd, bool pat,
        bool* invlpg) {

    VA_ADDRESS va = { virt };

    PML4E* pml4 = (PML4E*)(memory_base + handle->pml4_physical);
    PML4E* pml4e = &pml4[va.one_gb.pml4e];
    if(!pml4e->present) {
        uintptr_t addr;
        pmm_allocate_pages(ALLOCATE_ANY, 1, &addr);
        memset((void*)(addr + memory_base), 0, PAGE_SIZE);

        pml4e->present = true;
        pml4e->addr = addr >> PAGE_SHIFT;

        pml4e->us = (uint64_t) user;
        pml4e->rw = (uint64_t) write;
        pml4e->nx = (uint64_t) no_exec;
    }else {
        pml4e->us = (uint64_t) (pml4e->us ? 1 : user);
        pml4e->rw = (uint64_t) (pml4e->rw ? 1 : write);
        pml4e->nx = (uint64_t) (pml4e->nx ? no_exec : 0);
        *invlpg = true;
    }

    PDPTE* pdpt = (PDPTE*)(memory_base + (pml4e->addr << PAGE_SHIFT));
    PDPTE* pdpte = &pdpt[va.one_gb.pdpte];
    if(!pdpte->present) {
        uintptr_t addr;
        pmm_allocate_pages(ALLOCATE_ANY, 1, &addr);
        memset((void*)(addr + memory_base), 0, PAGE_SIZE);

        pdpte->present = true;
        pdpte->addr = addr >> PAGE_SHIFT;

        pdpte->us = (uint64_t) user;
        pdpte->rw = (uint64_t) write;
        pdpte->nx = (uint64_t) no_exec;
    }else {
        pdpte->us = (uint64_t) (pdpte->us ? 1 : user);
        pdpte->rw = (uint64_t) (pdpte->rw ? 1 : write);
        pdpte->nx = (uint64_t) (pdpte->nx ? no_exec : 0);
        *invlpg = true;
    }

    // make sure not a huge page
    ASSERT(!pdpte->page_size);

    PDE* pd = (PDE*)(memory_base + (pdpte->addr << PAGE_SHIFT));
    PDE* pde = &pd[va.two_mb.pde];
    if(!pde->present) {
        uintptr_t addr;
        pmm_allocate_pages(ALLOCATE_ANY, 1, &addr);
        memset((void*)(addr + memory_base), 0, PAGE_SIZE);

        pde->present = true;
        pde->addr = addr >> PAGE_SHIFT;

        pde->us = (uint64_t) user;
        pde->rw = (uint64_t) write;
        pde->nx = (uint64_t) no_exec;
    }else {
        pde->us = (uint64_t) (pde->us ? 1 : user);
        pde->rw = (uint64_t) (pde->rw ? 1 : write);
        pde->nx = (uint64_t) (pde->nx ? no_exec : 0);
        *invlpg = true;
    }

    // make sure not a huge page
    ASSERT(!pde->page_size);

    PTE* pt = (PTE*)(memory_base + (pde->addr << PAGE_SHIFT));
    PTE* pte = &pt[va.four_kb.pte];

    // if mapped then remap (requests invalidation)
    if(pte->present) {
        *invlpg = true;
    }

    pte->present = true;
    pte->addr = phys >> PAGE_SHIFT;

    pte->us = (uint64_t) user;
    pte->rw = (uint64_t) write;
    pte->nx = (uint64_t) no_exec;

    pte->pwt = (uint64_t) pwt;
    pte->pcd = (uint64_t) pcd;
    pte->pat = (uint64_t) pat;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VMM functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void* kernel_physical_end;
extern void* kernel_physical_start;
#define KERNEL_PHYSICAL_START ((uint64_t)&kernel_physical_start)
#define KERNEL_PHYSICAL_END ((uint64_t)&kernel_physical_end)
#define KERNEL_VIRTUAL_START (0xFFFFFFFFC0100000)

_Atomic(vmm_handle_t*) CPU_LOCAL current_vmm_handle;

void vmm_init(tboot_info_t* info) {
    TRACE("Preparing vmm");

    // allocate the pml4 of the kernel
    // it must be below the 4gb mark because otherwise the smp trampoline would
    // not be able to load it correctly
    kernel_process.vmm_handle.pml4_physical = UINT32_MAX;
    pmm_allocate_pages(ALLOCATE_BELOW, 1, &kernel_process.vmm_handle.pml4_physical);
    memset((void*)kernel_process.vmm_handle.pml4_physical, 0, PAGE_SIZE);

    // enable features
    vmm_enable_cpu_features();

    // map the direct map
    TRACE("mapping direct map");
    for(int i = 0; i < info->mmap.count; i++) {
        tboot_mmap_entry_t* entry = &info->mmap.entries[i];

        // map everything except for bad memory
        if(entry->type != TBOOT_MEMORY_TYPE_BAD_MEMORY) {
            uintptr_t addr = entry->addr;
            size_t len = entry->len;

            // do not map the first page in the direct map
            // that will avoid bugs
            if (addr == 0) {
                if (len > PAGE_SIZE) {
                    len -= PAGE_SIZE;
                    addr += PAGE_SIZE;
                } else {
                    continue;
                }
            }

            vmm_map(&kernel_process.vmm_handle, addr, PHYSICAL_TO_DIRECT(addr), len, PAGE_SUPERVISOR_READWRITE, DEFAULT_CACHE);
        }
    }

    // TODO: map with proper permissions
    TRACE("mapping kernel");
    vmm_map(&kernel_process.vmm_handle, KERNEL_PHYSICAL_START, KERNEL_VIRTUAL_START, KERNEL_PHYSICAL_END - KERNEL_PHYSICAL_START, PAGE_SUPERVISOR_EXEC_READWRITE, DEFAULT_CACHE);

    TRACE("Switching to kernel page table");
    memory_base = DIRECT_MAPPING_BASE;
    vmm_set_handle(&kernel_process.vmm_handle);
}

void vmm_enable_cpu_features() {
    CPUID_EXTENDED_CPU_SIG_EDX excpusig_edx;
    cpuid(CPUID_EXTENDED_CPU_SIG, NULL, NULL, NULL, &excpusig_edx.raw);

    CPUID_VERSION_INFO_EDX versioninfo_edx;
    cpuid(CPUID_VERSION_INFO, NULL, NULL, NULL, &versioninfo_edx.raw);

    /**************************************************
     * Various features in cr0
     **************************************************/
    IA32_CR0 cr0 = __readcr0();
    cr0.CD = 0; // allow caching
    cr0.NW = 0; // allow write through
    cr0.WP = 1; // write protect enable
    __writecr0(cr0);

    /**************************************************
     * No execute - must be supported
     **************************************************/
    ASSERT(excpusig_edx.NX);
    MSR_IA32_EFER efer = __readmsr_efer();
    efer.NXE = 1;
    __writemsr_efer(efer);

    TRACE("CPU Features:");

    /***************************************************
     * 1GB pages
     **************************************************/
    if(excpusig_edx.Page1GB) {
        TRACE("\t* 1GB Pages");
        support_1gb = true;
    }

    /***************************************************
     * Global pages - optional
     **************************************************/
     if(versioninfo_edx.PGE) {
         TRACE("\t* Global pages");
         support_global = true;
         IA32_CR4 cr4 = __readcr4();
         cr4.PGE = 1;
         __writecr4(cr4);
     }
}

bool vmm_virtual_to_physical(vmm_handle_t* handle, uintptr_t virtual, uintptr_t* physical, page_type_t* type) {
    ASSERT(handle != NULL);
    ASSERT(virtual != 0);

    spinlock_acquire(&handle->lock);

    VA_ADDRESS va = { virtual };
    void* entry;
    page_type_t page_type;

    if(!get_entry_at_virtual_address(handle, virtual, &entry, &page_type)) {
        spinlock_release(&handle->lock);
        return false;
    }

    uintptr_t phys;
    switch(page_type) {
        case PAGE_TYPE_4KB: {
            PTE* pte = entry;
            phys = (pte->addr << PAGE_SHIFT) | va.four_kb.offset;
        } break;

        case PAGE_TYPE_2MB:
        case PAGE_TYPE_1GB:
        default:
            ASSERT(false);
    }

    if(physical != NULL) {
        *physical = phys;
    }

    if(type != NULL) {
        *type = page_type;
    }

    spinlock_release(&handle->lock);
    return true;
}

bool vmm_is_mapped(vmm_handle_t* handle, uintptr_t addr, size_t size) {
    ASSERT(handle != NULL);
    ASSERT(addr != 0);
    ASSERT(size > 0);

    size_t current_size = 0;

    while(current_size < size) {
        page_type_t type;
        if(!vmm_virtual_to_physical(handle, addr, NULL, &type)) {
            return false;
        }

        size_t page_size = page_size_by_type(type);
        current_size += page_size;
        addr += page_size;
    }

    return true;
}

void vmm_unmap(vmm_handle_t* handle, uintptr_t addr, size_t size) {
    ASSERT(handle != NULL);
    ASSERT(addr != 0);
    ASSERT(size > 0);

    spinlock_acquire(&handle->lock);

    size_t current_size = 0;

    page_type_t type;
    void* entry = NULL;

    while(current_size < size) {

        if(get_entry_at_virtual_address(handle, addr, &entry, &type)) {
            // if mapped then unmap
            switch(type) {
                case PAGE_TYPE_4KB: {
                    PTE* pte = entry;
                    pte->addr = 0;
                } break;

                case PAGE_TYPE_1GB:
                case PAGE_TYPE_2MB:
                default:
                    ASSERT(false);
            }

            // and go by the correct size
            size_t type_size = page_size_by_type(type);
            current_size += type_size;
            addr += type_size;
        }else {
            // if not mapped go to the next page
            current_size += page_size_by_type(type);
            addr += page_size_by_type(type);
        }
    }

    // TODO: cleanup the tables

    spinlock_release(&handle->lock);
}

void vmm_map(vmm_handle_t* handle, uintptr_t phys, uintptr_t virt, size_t size, page_perms_t perms, /*IA32_PAT_MEMTYPE*/ uint32_t cache) {
    ASSERT(handle != NULL);

    // allow to map zero page if the page has
    // a map zero permission
//    if(unlikely(virt == 0)) {
//        ASSERT((perms & PAGE_MAP_ZERO) != 0);
//    }

    ASSERT(virt % PAGE_SIZE == 0);
    ASSERT(phys % PAGE_SIZE == 0);

    spinlock_acquire(&handle->lock);

    uintptr_t current_va = virt;
    uintptr_t range_end_va = virt + size;
    uintptr_t current_physical = phys;


    // get the cache flags
    bool pwt_flag = false;
    bool pcd_flag = false;
    bool pat_flag = false;
//    if(!get_pat_flags_from_mem_type(cache, &pwt_flag, &pcd_flag, &pat_flag)) {
//        debug_log("[!] vmm_map: requested cache is not supported, using default (WT)\n");
//    }

    // get the permission bits
    bool user = (perms & PAGE_SUPERVISOR) == 0;
    bool write = (perms & PAGE_WRITE) != 0;
    bool no_exec = ((perms & PAGE_EXECUTE) == 0);

    while(current_va < range_end_va) {
        bool invlpg = false;
        set_pte(handle, current_va, current_physical, user, write, no_exec, pwt_flag, pcd_flag, pat_flag, &invlpg);

        if (invlpg) {
            __invlpg(current_va);
        }

        current_va += PAGE_SIZE;
        current_physical += PAGE_SIZE;
    }

    spinlock_release(&handle->lock);
}

void vmm_set_handle(vmm_handle_t* handle) {
    if (current_vmm_handle != handle) {
        current_vmm_handle = handle;
        __writecr3(handle->pml4_physical);
    }
}