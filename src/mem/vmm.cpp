#include <util/cpp_runtime.h>
#include <arch/intrin.hpp>
#include <arch/paging.h>
#include <string.h>
#include <arch/cpuid.hpp>

#include "pmm.hpp"
#include "vmm.hpp"

namespace mem::vmm {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Kernel context
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    _Alignas(alignof(context)) uint8_t kernel_context_buffer[sizeof(context)];
    context *kernel = nullptr;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Context methods
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // gonna use this here for ease of use
    using namespace arch::paging;

    /*
     * This is the direct mapping base
     *
     * it starts at 0 for the initial direct map but is moved to
     * 0xFFFF7000_00000000 once we switch to the kernel's mapping
     */
    static uintptr_t memory_base = 0;

    /**
     * Will return the size of the page in bytes based on the type
     *
     * @param type [IN] the type
     *
     * @return the size in bytes
     */
    static size_t page_size_by_type(page_size type) {
        switch(type) {
            case SIZE_4KB: return 4096l;
            case SIZE_2MB: return 4096l * 512l;
            case SIZE_1GB: return 4096l * 512l * 512l;
            default: ASSERT(false);
        }
    }

    void context::set_pte(uintptr_t virt, uintptr_t phys, bool user, bool write, bool no_exec, bool pwt, bool pcd, bool pat) {
        VA_ADDRESS va = { virt };

        PML4E* pml4 = (PML4E*)(memory_base + pml4_physical);
        PML4E* pml4e = &pml4[va.one_gb.pml4e];
        if(!pml4e->present) {
            uintptr_t addr = mem::pmm::allocate_pages(mem::pmm::ALLOCATE_ANY, 1);
            memset((void*)(addr + memory_base), 0, ::arch::paging::page_size);

            pml4e->present = true;
            pml4e->addr = addr >> page_shift;

            pml4e->us = (uint64_t) user;
            pml4e->rw = (uint64_t) write;
            pml4e->nx = (uint64_t) no_exec;
        }else {
            pml4e->us = (uint64_t) (pml4e->us ? 1 : user);
            pml4e->rw = (uint64_t) (pml4e->rw ? 1 : write);
            pml4e->nx = (uint64_t) (pml4e->nx ? no_exec : 0);
        }

        PDPTE* pdpt = (PDPTE*)(memory_base + (pml4e->addr << page_shift));
        PDPTE* pdpte = &pdpt[va.one_gb.pdpte];
        if(!pdpte->present) {
            uintptr_t addr = mem::pmm::allocate_pages(mem::pmm::ALLOCATE_ANY, 1);
            memset((void*)(addr + memory_base), 0, ::arch::paging::page_size);

            pdpte->present = true;
            pdpte->addr = addr >> page_shift;

            pdpte->us = (uint64_t) user;
            pdpte->rw = (uint64_t) write;
            pdpte->nx = (uint64_t) no_exec;
        }else {
            pdpte->us = (uint64_t) (pdpte->us ? 1 : user);
            pdpte->rw = (uint64_t) (pdpte->rw ? 1 : write);
            pdpte->nx = (uint64_t) (pdpte->nx ? no_exec : 0);
        }

        // make sure not a huge page
        ASSERT(!pdpte->page_size);

        PDE* pd = (PDE*)(memory_base + (pdpte->addr << page_shift));
        PDE* pde = &pd[va.two_mb.pde];
        if(!pde->present) {
            uintptr_t addr = mem::pmm::allocate_pages(mem::pmm::ALLOCATE_ANY, 1);
            memset((void*)(addr + memory_base), 0, ::arch::paging::page_size);

            pde->present = true;
            pde->addr = addr >> page_shift;

            pde->us = (uint64_t) user;
            pde->rw = (uint64_t) write;
            pde->nx = (uint64_t) no_exec;
        }else {
            pde->us = (uint64_t) (pde->us ? 1 : user);
            pde->rw = (uint64_t) (pde->rw ? 1 : write);
            pde->nx = (uint64_t) (pde->nx ? no_exec : 0);
        }

        // make sure not a huge page
        ASSERT(!pde->page_size);

        PTE* pt = (PTE*)(memory_base + (pde->addr << page_shift));
        PTE* pte = &pt[va.four_kb.pte];
        ASSERT(!pte->present);

        pte->present = true;
        pte->addr = phys >> page_shift;

        pte->us = (uint64_t) user;
        pte->rw = (uint64_t) write;
        pte->nx = (uint64_t) no_exec;

        pte->pwt = (uint64_t) pwt;
        pte->pcd = (uint64_t) pcd;
        pte->pat = (uint64_t) pat;
    }

    /**
     * Will get the bottom most entry of that virtual address
     *
     * basically this will take to account the 1gb and 2mb pages
     *
     * @param handle            [IN]    The handle to the vmm
     * @param virtual_address   [IN]    The virtual address to get the entry of
     * @param entry             [OUT]   The entry (pointer, direct mapped)
     * @param page_size         [OUT]   The page size (4kb,2mb or 1gb)
     *
     * @retval true - the page is mapped
     * @retval false - the page is not mapped
     */
    bool context::get_entry_at_virtual_address(void *virtual_address, void **entry, page_size *page_size) {
      VA_ADDRESS va = {(uintptr_t) virtual_address};
      PML4E* pml4 = (PML4E *) (memory_base + pml4_physical);

      ASSERT(page_size != nullptr);
      ASSERT(entry != nullptr);

      PML4E pml4e = pml4[va.one_gb.pml4e];
      if (!pml4e.present) {
        return false;
      }

      PDPTE* pdpt = (PDPTE *) (memory_base + (pml4e.addr << page_shift));
      PDPTE pdpte = pdpt[va.one_gb.pdpte];
      if (!pdpte.present) {
        *page_size = SIZE_1GB;
        return false;
      }

      if (pdpte.page_size) {
        *entry = &pdpt[va.one_gb.pdpte];
        *page_size = SIZE_1GB;
        return true;
      }

      PDE *pd = (PDE *) (memory_base + (pdpte.addr << page_shift));
      PDE pde = pd[va.two_mb.pde];
      if (!pde.present) {
        *page_size = SIZE_2MB;
        return false;
      }

      if (pde.page_size) {
        *entry = &pd[va.two_mb.pde];
        *page_size = SIZE_2MB;
        return true;
      }

      PTE *pt = (PTE *) (memory_base + (pde.addr << page_shift));
      PTE pte = pt[va.four_kb.pte];
      if (!pte.present) {
        *page_size = SIZE_4KB;
        return false;
      }

      *entry = &pt[va.four_kb.pte];
      *page_size = SIZE_4KB;
      return true;
    }

    context::context(bool kernel) {
        this->pml4_physical = pmm::allocate_pages(pmm::ALLOCATE_ANY, 1);
        memset((void*)pml4_physical, 0, ::arch::paging::page_size);

        if (!kernel) {
            // TODO: copy the kernel's mapping and stuff
        }
    }

    context::~context() {
        // make sure not deleting the kernel's mapping
        ASSERT(this != kernel);

        // TODO: free all the mappings
    }

    bool context::virtual_to_physical(void *virt, uintptr_t *out_phys, page_size *size) {
        ASSERT(virt != 0);
        util::spinlock_guard guard(this->lock);

        VA_ADDRESS va = { (uintptr_t)virt };
        void* entry;
        page_size page_size;

        if(!get_entry_at_virtual_address(virt, &entry, &page_size)) {
            return false;
        }

        uintptr_t phys;
        switch(page_size) {
            case SIZE_4KB: {
                PTE* pte = reinterpret_cast<PTE*>(entry);
                phys = (pte->addr << page_shift) | va.four_kb.offset;
            } break;

            case SIZE_2MB:
            case SIZE_1GB:
            default:
                ASSERT(false);
        }

        if(out_phys != nullptr) {
            *out_phys = phys;
        }

        if(size != nullptr) {
            *size = page_size;
        }

        return true;
    }

    bool context::is_mapped(void* virt, size_t range) {
        ASSERT(virt != nullptr);
        ASSERT(range > 0);
        uintptr_t addr = (uintptr_t)virt;

        size_t current_size = 0;

        while(current_size < range) {
            page_size type;
            if(!virtual_to_physical((void*)addr, nullptr, &type)) {
                return false;
            }

            size_t page_size = page_size_by_type(type);
            current_size += page_size;
            addr += page_size;
        }

        return true;
    }

    void context::map(uintptr_t phys, void* virt, size_t size, permission perms, cache_policy cache) {
        // allow to map zero page if the page has
        // a map zero permission
        if(virt == nullptr) {
            ASSERT((perms & ZERO_PAGE) != 0);
        }

        ASSERT((uintptr_t)virt % ::arch::paging::page_size == 0);
        ASSERT(phys % ::arch::paging::page_size == 0);

        util::spinlock_guard guard(lock);

        uintptr_t current_va = (uintptr_t)virt;
        uintptr_t range_end_va = current_va + size;
        uintptr_t current_physical = phys;


        // get the cache flags
        bool pwt_flag = false;
        bool pcd_flag = false;
        bool pat_flag = false;
//        if(!get_pat_flags_from_mem_type(cache, &pwt_flag, &pcd_flag, &pat_flag)) {
//            debug_log("[!] vmm_map: requested cache is not supported, using default (WT)\n");
//        }

        // get the permission bits
        bool user = (perms & KERNEL) == 0;
        bool write = (perms & WRITE) != 0;
        bool no_exec = ((perms & EXECUTE) == 0);

        while(current_va < range_end_va) {
            set_pte(current_va, current_physical, user, write, no_exec, pwt_flag, pcd_flag, pat_flag);

            current_va += ::arch::paging::page_size;
            current_physical += ::arch::paging::page_size;
        }
    }

    void context::switch_to_context() {
        arch::write_cr3(this->pml4_physical);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Global functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    extern "C" void* kernel_physical_end;
    extern "C" void* kernel_physical_start;
    #define KERNEL_PHYSICAL_START ((uint64_t)&kernel_physical_start)
    #define KERNEL_PHYSICAL_END ((uint64_t)&kernel_physical_end)
    #define KERNEL_VIRTUAL_START ((void*)0xFFFFFFFFC0200000)

    void init(tboot_info_t *info) {
        debug_log("[*] Preparing vmm\n");

        // create the kernel context
        kernel = new (kernel_context_buffer) context(true);

        debug_log("[*] \tmapping direct map\n");
        for(int i = 0; i < info->mmap.count; i++) {
            tboot_mmap_entry_t* entry = &info->mmap.entries[i];

            if(entry->type != TBOOT_MEMORY_TYPE_BAD_MEMORY) {
                kernel->map(entry->addr, (void*)physical_to_direct(entry->addr), entry->len, (permission)(WRITE | KERNEL));
            }
        }

        debug_log("[*] \tmapping kernel\n");
        kernel->map(KERNEL_PHYSICAL_START, KERNEL_VIRTUAL_START, KERNEL_PHYSICAL_END - KERNEL_PHYSICAL_START, (permission)(EXECUTE | WRITE | KERNEL));

        enable_cpu_features();

        debug_log("[*] \tSwitching to kernel page table\n");
        memory_base = direct_mapping_base;
        kernel->switch_to_context();
    }

    void enable_cpu_features() {
        using namespace arch;

        // structures we may check
        CPUID_EX_FEATURES ex_features = { 0 };
        cpuid(cpuid_function::ex_features, 0, (uint32_t*)&ex_features);

        // structures we might change
        IA32_EFER efer = { .raw = read_msr(MSR_CODE_IA32_EFER) };

        /**************************************************
         * No execute - must be supported
         **************************************************/
         ASSERT(ex_features.nx);
         efer.nxe = 1;

         // commit all the changes
         write_msr(MSR_CODE_IA32_EFER, efer.raw);
    }

}
