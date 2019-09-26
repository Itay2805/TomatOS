#include <common.h>
#include <locks/spinlock.h>
#include <tboot/tboot.h>
#include "pmm.h"
#include "vmm.h"

typedef struct pmm_slot {
    uintptr_t start;
    size_t length;
    bool valid;
} pmm_slot_t;

static pmm_slot_t* slots = NULL;
static size_t slot_index = 0;
static size_t slot_count = 0;

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

    log_info("Physical Memory: %lld bytes (top 0x%p)", total_available_size, top_address);
    slot_count = (ALIGN_PAGE_DOWN(total_available_size) / KB(4)) / 2;
    size_t total_size_for_stack = slot_count * sizeof(pmm_slot_t);
    log_info("PMM stack size %lld bytes (%lld entries)", total_size_for_stack, slot_count);

    slots = (pmm_slot_t *) (ALIGN_PAGE_UP(KERNEL_PHYSICAL_END)) + KB(4);
    uintptr_t kernel_end = ALIGN_PAGE_UP((uintptr_t)slots + total_size_for_stack);

    log_info("Memory map:");
    for(tboot_mmap_entry_t* it = info->mmap.entries; it < info->mmap.entries + info->mmap.count; it++) {
        log_info("\t0x%016p-0x%016p: %s", it->addr, it->addr + it->len, MMAP_TYPE[it->type]);
        if(it->type == TBOOT_MEMORY_TYPE_USABLE) {
            uintptr_t addr = ALIGN_UP(it->addr, KB(4));
            size_t count = ALIGN_DOWN(it->len, KB(4)) / KB(4);

            // if before the kernel ignore
            if(addr + (count * KB(4)) <= kernel_end) {
                continue;

            // align to the kernel start
            } else if(addr < kernel_end) {
                addr = kernel_end;
            }

            // set as free
            CHECK_AND_RETHROW(pmm_free(addr, count));
        }
    }

cleanup:
    return err;
}

error_t pmm_init() {
    slots = CONVERT_TO_DIRECT(slots);
    return NO_ERROR;
}

error_t pmm_allocate(uint64_t* addr, size_t count) {
    error_t err = NO_ERROR;

    lock_preemption(&pmm_lock);

    count *= KB(4);
    for(int i = 0; i < slot_index; i++) {
        pmm_slot_t* slot = &slots[i];

        // if not found go to the next one
        if(!slot->valid) {
            continue;

        // if does not have enough memory
        }else if(slot->length < count) {
            continue;

        // found a valid block!
        }else {
            slot->length -= count;
            uintptr_t new_addr = slot->start + slot->length;

            // if no more free place in this slot
            if(slot->length == 0) {
                slot->valid = false;
            }

            *addr = new_addr;
            goto cleanup;
        }
    }

    CHECK_FAIL_TRACE("Could not find enough contiguous physical memory");

cleanup:
    unlock_preemption(&pmm_lock);
    return err;
}

error_t pmm_free(uint64_t addr, size_t count) {
    error_t err = NO_ERROR;
    size_t free_len = count * KB(4);
    int free_i = -1;

    lock_preemption(&pmm_lock);

    for(int i = 0; i < slot_index; i++) {
        pmm_slot_t* slot = &slots[i];

        // if this is not a valid block we
        // can feel it later
        if(!slot->valid) {
            free_i = i;
            continue;
        }

        // add to the length of this slot if it is at its end
        if(slot->start + slot->length == addr) {
            slot->length += free_len;
            goto cleanup;

        // remove from the base of this block if ends at its start
        }else if(addr + free_len == slot->start) {
            slot->start -= free_len;
        }
    }

    if(free_i != -1) {
        slots[free_i].start = addr;
        slots[free_i].length = free_len;
        slots[free_i].valid = true;
    }else {
        CHECK_TRACE(slot_index < slot_count, "No more free slots!");

        // add a new entry
        slots[slot_index].start = addr;
        slots[slot_index].length = free_len;
        slots[slot_index].valid = true;
        slot_index++;
    }

cleanup:
    unlock_preemption(&pmm_lock);
    return err;
}
