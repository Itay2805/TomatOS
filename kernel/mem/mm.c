#include <sync/lock.h>
#include <stdbool.h>
#include <util/list.h>
#include <proc/process.h>
#include <util/string.h>
#include "tlsf.h"

#include "mm.h"
#include "pmm.h"

#define STACK_SIZE (PAGE_SIZE * 2)

static tlsf_t g_tlsf = NULL;
static ticket_lock_t g_tlsf_lock = INIT_LOCK();

static list_entry_t g_stack_free_list = INIT_LIST(g_stack_free_list);
static ticket_lock_t g_stack_lock = INIT_LOCK();
static void* g_stack_ptr = (void*)KERNEL_STACK_HEAP_BASE;

err_t mm_init() {
    err_t err = NO_ERROR;

    size_t size = ALIGN_UP(tlsf_size(), PAGE_SIZE);
    CHECK_AND_RETHROW(pmm_allocate_zero(size / PAGE_SIZE, &g_tlsf));
    g_tlsf = tlsf_create_with_pool(g_tlsf, size);
    CHECK(g_tlsf != NULL);

cleanup:
    return err;
}

static bool expand_tlsf(size_t size) {
    size_t allocsize = ALIGN_UP(tlsf_pool_overhead() + sizeof(size_t) * 5 + size, PAGE_SIZE);
    void* pool = NULL;
    if(IS_ERROR(pmm_allocate_zero(allocsize / PAGE_SIZE, &pool))) {
        return false;
    } else {
        return tlsf_add_pool(g_tlsf, pool, allocsize) != NULL;
    }
}

void* kalloc(size_t size) {
    ticket_lock(&g_tlsf_lock);
    void* res = tlsf_malloc(g_tlsf, size);

    // If we are out of resources then allocate
    // more space for tlsf
    if (res == NULL) {
        if (!expand_tlsf(size)) {
            goto cleanup;
        }
        res = tlsf_malloc(g_tlsf, size);
    }

    if (res != NULL) {
        memset(res, 0, size);
    }

cleanup:
    ticket_unlock(&g_tlsf_lock);
    return res;
}

void* krealloc(void* ptr, size_t size) {
    ticket_lock(&g_tlsf_lock);
    void* res = tlsf_realloc(g_tlsf, ptr, size);

    // If we are out of resources then allocate
    // more space for tlsf
    if (res == NULL) {
        if (!expand_tlsf(size)) {
            goto cleanup;
        }
        res = tlsf_realloc(g_tlsf, ptr, size);
    }

cleanup:
    ticket_unlock(&g_tlsf_lock);
    return res;
}

void kfree(void* ptr) {
    ticket_lock(&g_tlsf_lock);
    tlsf_free(g_tlsf, ptr);
    ticket_unlock(&g_tlsf_lock);
}

void* alloc_stack() {
    void* stack = NULL;

    ticket_lock(&g_stack_lock);
    if (list_is_empty(&g_stack_free_list)) {
        stack = g_stack_ptr;
        g_stack_ptr += SIZE_4KB + STACK_SIZE;

        directptr_t new_stack;
        if (!IS_ERROR(pmm_allocate_zero(1 + (STACK_SIZE / PAGE_SIZE), &new_stack))) {
            // map the stack
            for (int i = 0; i < (STACK_SIZE / PAGE_SIZE); i++) {
                vmm_map(&g_kernel.address_space, stack + i * PAGE_SIZE, DIRECT_TO_PHYSICAL(new_stack + i * PAGE_SIZE) + 0, MAP_WRITE);
            }
        }
    } else {
        list_entry_t* stack_entry = g_stack_free_list.next;
        list_del(stack);
        memset(stack_entry, 0, STACK_SIZE);
        stack = (void*)stack_entry;
    }
    ticket_unlock(&g_stack_lock);

    // set the return to exit
    stack += STACK_SIZE;
    stack -= sizeof(uintptr_t) * 2;

    return stack;
}

void free_stack(void* stk) {
    ticket_lock(&g_stack_lock);
    list_entry_t* entry = (stk + (sizeof(uintptr_t) * 2)) - STACK_SIZE;
    list_add(&g_stack_free_list, entry);
    ticket_unlock(&g_stack_lock);
}
