#include <common.h>
#include <locks/spinlock.h>
#include "pmm.h"

static uint64_t* addrs;
static size_t stack_len = 0;
static size_t stack_cap;

/*
 * Global PMM lock
 */
static spinlock_t pmm_lock;

static const char* MMAP_TYPE[] = {
    [MMAP_AVAILABLE] = "Available",
    [MMAP_RESERVED] = "Reserved",
    [MMAP_ACPI_RELAIMABLE] = "ACPI Reclaimable",
    [MMAP_ACPI_NVS] = "Non-Volatile memory",
    [MMAP_BAD_MEMORY] = "Bad memory",
};

error_t pmm_early_init(boot_info_t* info) {
    error_t err = NO_ERROR;
    size_t top_address = 0;
    size_t total_available_size = 0;

    for(mmap_entry_t* it = info->mmap.entries; it < info->mmap.entries + info->mmap.count; it++) {
        if(it->addr + it->size > top_address) {
            top_address = it->addr + it->size;
        }
        if(it->type == MMAP_AVAILABLE) {
            total_available_size += it->size;
        }
    }

    log_debug("Physical Memory: %lld/%lld", total_available_size, info->total_mem);

    stack_cap = ALIGN_UP(total_available_size, KB(4))  / KB(4);
    size_t total_size_for_stack = (stack_cap) * sizeof(uint64_t);
    log_debug("PMM stack size %lld bytes (%lld entries)", total_size_for_stack, stack_cap);

    addrs = (uint64_t *) (ALIGN_UP(KERNEL_PHYSICAL_END, KB(4)) + KB(4));
    uintptr_t kernel_end = ALIGN_UP(ALIGN_UP(KERNEL_PHYSICAL_END, KB(4)) + KB(4) + total_size_for_stack, KB(4));

    log_debug("Memory map:");
    for(mmap_entry_t* it = info->mmap.entries; it < info->mmap.entries + info->mmap.count; it++) {
        log_debug("\t0x%016p-0x%016p: %s", it->addr, it->addr + it->size, MMAP_TYPE[it->type]);
        if(it->type == MMAP_AVAILABLE) {
            for(uint64_t addr = ALIGN_UP(it->addr, KB(4)); addr < ALIGN_DOWN(it->addr + it->size, KB(4)); addr += KB(4)) {
                // only use physical memory from the kernel upwards
                if(addr <= kernel_end)  continue;

                CHECK_AND_RETHROW(pmm_free(addr));
            }
        }
    }

    // reverse the stack, we do this because otherwise we will allocate high addresses
    // before even having that place mapped, this will make us allocate lower addresses first
    for(int i = 0; i < stack_len / 2; i++) {
        uint64_t tmp = addrs[stack_len - i];
        addrs[stack_len - i] = addrs[i];
    addrs[i] = tmp;
}

cleanup:
    return err;
}

error_t pmm_init() {
    addrs = (uint64_t*)(((uint64_t)addrs) + 0xFFFF800000000000);
    return NO_ERROR;
}

error_t pmm_allocate(uint64_t* addr) {
    error_t err = NO_ERROR;

    lock_preemption(&pmm_lock);

    CHECK_ERROR(addr, ERROR_INVALID_ARGUMENT);

    CHECK(addrs);
    CHECK_ERROR_TRACE(stack_len > 0, ERROR_OUT_OF_MEMORY, "No more available physical address!");

    *addr = addrs[--stack_len];

cleanup:
    unlock_preemption(&pmm_lock);
    return err;
}

error_t pmm_free(uint64_t addr) {
    error_t err = NO_ERROR;

    lock_preemption(&pmm_lock);

    CHECK(addrs);
    CHECK(stack_len < stack_cap);

    addrs[stack_len++] = addr;

cleanup:
    unlock_preemption(&pmm_lock);
    return err;
}
