#include <proc/process.h>
#include <util/except.h>
#include <mem/pmm.h>
#include <stdbool.h>

#include "stivale.h"
#include "mem/vmm.h"
#include "intrin.h"


#define PM_PRESENT  (BIT0)
#define PM_WRITE    (BIT1)
#define PM_USER     (BIT2)
#define PM_GLOBAL   (BIT8)
#define PM_SIZE     (BIT7)
#define PM_XD       (BIT63)

/**
 * Are global pages supported
 */
static bool g_global_pages = false;

// TODO: use 2mb pages
#define HUGE_PAGE_SIZE SIZE_4KB

static err_t figure_features() {
    err_t err = NO_ERROR;
    IA32_CR4 cr4 = __readcr4();

    TRACE("CPU Paging Features");

    // TODO: no execute

    if (cr4.PGE) {
        TRACE("\t* Global pages");
        g_global_pages = true;
    }

//cleanup:
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
    CHECK_AND_RETHROW(figure_features());

    // initialize the kernel address space
    g_kernel.address_space.lock = INIT_LOCK();
    CHECK_AND_RETHROW(pmm_allocate_zero(1, &g_kernel.address_space.pml4));

    // all the kernel entries are preallocated, this makes sure the
    // address space is always in sync between all the kernel threads
    uint64_t* table = g_kernel.address_space.pml4;
    for (int i = 256; i < 512; i++) {
        CHECK_AND_RETHROW(pmm_allocate_zero(1, (directptr_t*)&table[i]));
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
    // TODO: don't rwx the kernel and modules
    TRACE("Mapping kernel");
    for (size_t i = 0; i < stivale->memory_map_entries; i++) {
        entry = &stivale->memory_map_addr[i];

        if (entry->type == 10) {
            bool is_kernel = true;

            // if this is a module direct map it
            stivale_module_t* module = stivale->modules;
            while (module != NULL) {
                if (entry->base == module->begin) {
                    is_kernel = false;
                    break;
                }
                module = module->next;
            }

            // otherwise it is the kernel and we want
            // to map it at the -2gb mark
            if (is_kernel) {
                for (physptr_t ptr = entry->base;
                     ptr < ALIGN_UP(entry->base + entry->length, PAGE_SIZE);
                     ptr += PAGE_SIZE) {
                    CHECK_AND_RETHROW(vmm_map(&g_kernel.address_space, (void*)(KERNEL_BASE + ptr), ptr, MAP_EXEC | MAP_WRITE));
                }
            }
        }
    }

    // set the kernel paging
    CHECK_AND_RETHROW(set_address_space(&g_kernel.address_space));

cleanup:
    return err;
}

err_t set_address_space(address_space_t* space) {
    err_t err = NO_ERROR;

    CHECK(space != NULL);
    __writecr3(DIRECT_TO_PHYSICAL(space->pml4));

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
            directptr_t ptr;
            CHECK_AND_RETHROW(pmm_allocate_zero(1, &ptr));
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

    CHECK(space != NULL);
    ticket_lock(&space->lock);



cleanup:
    if (space != NULL) {
        ticket_unlock(&space->lock);
    }
    return err;
}

