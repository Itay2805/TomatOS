#include <sync/lock.h>
#include <stdbool.h>
#include "tlsf.h"

#include "mm.h"
#include "pmm.h"

static tlsf_t g_tlsf = NULL;
static ticket_lock_t g_tlsf_lock = INIT_LOCK();

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

