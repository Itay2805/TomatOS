#include <stddef.h>
#include <arch/paging.h>
#include <util/list.h>
#include <proc/process.h>
#include "stack_allocator.h"
#include "vmm.h"
#include "pmm.h"
#include "mm.h"

typedef struct stack_block {
    list_entry_t link;
    uintptr_t base;
} stack_block_t;

static uintptr_t stack_allocation = STACK_ALLOCATION_BASE;
static list_entry_t free_blocks = INIT_LIST_ENTRY(free_blocks);
static list_entry_t used_blocks = INIT_LIST_ENTRY(used_blocks);
static spinlock_t lock = SPINLOCK_INIT;

static void acquire_alloc_lock() {
    spinlock_acquire(&lock);
}

static void release_alloc_lock() {
    spinlock_release(&lock);
}

uintptr_t allocate_stack() {
    acquire_alloc_lock();

    if (!is_list_empty(&free_blocks)) {
        // pop from the list
        stack_block_t* entry = CR(free_blocks.next, stack_block_t, link);
        remove_entry_list(&entry->link);

        // add to the used block list
        insert_tail_list(&used_blocks, &entry->link);

        release_alloc_lock();
        return entry->base + STACK_SIZE;
    }

    // setup the entry
    stack_block_t* block = mm_allocate(sizeof(stack_block_t));
    block->base = stack_allocation;
    insert_tail_list(&used_blocks, &block->link);

    // allocate pages
    for (int i = 0; i < SIZE_TO_PAGES(STACK_SIZE); i++) {
        // allocate and map the page
        uintptr_t base = 0;
        ASSERT(!IS_ERROR(pmm_allocate_pages(ALLOCATE_ANY, 1, &base)));
        vmm_map(&kernel_process.vmm_handle, base, stack_allocation, PAGE_SIZE, PAGE_SUPERVISOR_READWRITE, DEFAULT_CACHE);

        stack_allocation += PAGE_SIZE;
        ASSERT(stack_allocation < STACK_ALLOCATION_END);
    }

    // skip last page as guard
    stack_allocation += PAGE_SIZE;
    ASSERT(stack_allocation <= STACK_ALLOCATION_END);

    release_alloc_lock();

    return block->base + STACK_SIZE;
}

void free_stack(uintptr_t addr) {
    ASSERT(IS_STACK_PTR(addr));
    addr -= STACK_SIZE;

    acquire_alloc_lock();

    // search for the allocated block
    FOR_EACH_IN_LIST(used_blocks, link) {
        stack_block_t* block = CR(link, stack_block_t, link);

        // found it
        if (block->base == addr) {
            remove_entry_list(&block->link);
            insert_tail_list(&free_blocks, &block->link);
            break;
        }
    }

    release_alloc_lock();
}
