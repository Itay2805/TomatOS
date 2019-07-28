#include <common.h>
#include <locks/spinlock.h>
#include <drivers/portio.h>
#include <tboot/tboot.h>
#include "pmm.h"
#include "vmm.h"

static uint64_t* addrs;
static size_t stack_len = 0;
static size_t stack_cap;

/*
 * Global PMM lock
 */
static spinlock_t pmm_lock;

static const char* MMAP_TYPE[] = {
    [TBOOT_MEMORY_TYPE_USABLE] = "Available",
    [TBOOT_MEMORY_TYPE_RESERVED] = "Reserved",
    [TBOOT_MEMORY_TYPE_ACPI_RECLAIM] = "ACPI Reclaimable",
    [TBOOT_MEMORY_TYPE_ACPI_NVS] = "ACPI NVM",
    [TBOOT_MEMORY_TYPE_BAD_MEMORY] = "Bad memory",
};

error_t pmm_early_init(tboot_info_t* info) {
    error_t err = NO_ERROR;
    size_t top_address = 0;
    size_t total_available_size = 0;

    CHECK(info->mmap.entries != NULL);
    CHECK(info->mmap.count > 0);

    for(tboot_mmap_entry_t* it = info->mmap.entries; it < info->mmap.entries + info->mmap.count; it++) {
        if(it->addr + it->len > top_address) {
            top_address = it->addr + it->len;
        }
        if(it->type == TBOOT_MEMORY_TYPE_USABLE) {
            total_available_size += it->len;
        }
    }

    log_info("Physical Memory: %lld/%lld", total_available_size, top_address);

    stack_cap = ALIGN_UP(total_available_size, KB(4))  / KB(4);
    size_t total_size_for_stack = (stack_cap) * sizeof(uint64_t);
    log_info("PMM stack size %lld bytes (%lld entries)", total_size_for_stack, stack_cap);

    addrs = (uint64_t *) (ALIGN_UP(KERNEL_PHYSICAL_END, KB(4)) + KB(4));
    uintptr_t kernel_end = ALIGN_UP(ALIGN_UP(KERNEL_PHYSICAL_END, KB(4)) + KB(4) + total_size_for_stack, KB(4));

    log_info("Memory map:");
    for(tboot_mmap_entry_t* it = info->mmap.entries; it < info->mmap.entries + info->mmap.count; it++) {
        log_info("\t0x%016p-0x%016p: %s", it->addr, it->addr + it->len, MMAP_TYPE[it->type]);
        if(it->type == TBOOT_MEMORY_TYPE_USABLE) {
            for(uint64_t addr = ALIGN_UP(it->addr, KB(4)); addr < ALIGN_DOWN(it->addr + it->len, KB(4)); addr += KB(4)) {
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
    addrs = (uint64_t*)(((uint64_t)addrs) + DIRECT_MAPPING_BASE);
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
