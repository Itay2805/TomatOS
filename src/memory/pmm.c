#include <common.h>
#include "pmm.h"

static uint64_t* addrs;
static size_t stack_len = 0;
static size_t stack_cap;

static const char* MMAP_TYPE[] = {
    [MULTIBOOT_MEMORY_AVAILABLE] = "Available",
    [MULTIBOOT_MEMORY_RESERVED] = "Reserved",
    [MULTIBOOT_MEMORY_ACPI_RECLAIMABLE] = "ACPI Reclaimable",
    [MULTIBOOT_MEMORY_NVS] = "Non-Volatile memory",
    [MULTIBOOT_MEMORY_BADRAM] = "Bad ram",
};

error_t pmm_early_init(multiboot_info_t* info) {
    error_t err = NO_ERROR;
    size_t top_address = 0;
    size_t total_available_size = 0;
    multiboot_memory_map_t* entries = (multiboot_memory_map_t*)(uintptr_t)info->mmap_addr;
    multiboot_memory_map_t* it;

    CHECK_TRACE(info->flags & MULTIBOOT_INFO_MEMORY, "mem lower/upper not found in multiboot info!");
    CHECK_TRACE(info->flags & MULTIBOOT_INFO_MEM_MAP, "mmap not found in multiboot info!");

    for(it = entries; (char*)it - (char*)entries < info->mmap_length; it++) {
        if(it->addr + it->len > top_address) {
            top_address = it->addr + it->len;
        }
        if(it->type == MULTIBOOT_MEMORY_AVAILABLE) {
            total_available_size += it->len;
        }
    }

    log_debug("Physical Memory: %lld/%lld", total_available_size, (info->mem_lower + info->mem_upper) * 1024);

    stack_cap = ALIGN_UP(total_available_size, KB(4))  / KB(4);
    size_t total_size_for_stack = (stack_cap) * sizeof(uint64_t);
    log_debug("PMM stack size %lld bytes (%lld entries)", total_size_for_stack, stack_cap);

    addrs = (uint64_t *) (ALIGN_UP(KERNEL_PHYSICAL_END, KB(4)) + KB(4));
    uintptr_t kernel_end = ALIGN_UP(ALIGN_UP(KERNEL_PHYSICAL_END, KB(4)) + KB(4) + total_size_for_stack, KB(4));

    log_debug("Memory map:");
    for(it = entries; (char*)it - (char*)entries < info->mmap_length; it++) {
        log_debug("\t0x%016p-0x%016p: %s", it->addr, it->addr + it->len, MMAP_TYPE[it->type]);
        if(it->type == MULTIBOOT_MEMORY_AVAILABLE) {
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
    addrs = (uint64_t*)(((uint64_t)addrs) + 0xFFFF800000000000);
    return NO_ERROR;
}

error_t pmm_allocate(uint64_t* addr) {
    error_t err = NO_ERROR;

    CHECK_ERROR(addr, ERROR_INVALID_ARGUMENT);

    CHECK(addrs);
    CHECK_ERROR_TRACE(stack_len > 0, ERROR_OUT_OF_MEMORY, "No more available physical address!");

    *addr = addrs[--stack_len];

cleanup:
    return err;
}

error_t pmm_free(uint64_t addr) {
    error_t err = NO_ERROR;

    CHECK(addrs);
    CHECK(stack_len < stack_cap);

    addrs[stack_len++] = addr;

cleanup:
    return err;
}
