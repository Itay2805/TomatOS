#include <sync/lock.h>
#include <util/list.h>
#include "pmm.h"

typedef struct memory_range {
    list_entry_t link;
    size_t page_count;
} memory_range_t;

static list_entry_t g_free_list = LIST_INIT(g_free_list);

static lock_t g_pmm_lock = {0};

err_t pmm_submit_range(directptr_t start, size_t page_count) {
    err_t err = NO_ERROR;

    CHECK_AND_RETHROW(pmm_free(start, page_count));

cleanup:
    return err;
}

err_t pmm_allocate(size_t page_count, directptr_t* base) {
    err_t err = NO_ERROR;

    acquire_lock(&g_pmm_lock);

    CHECK(base != NULL);

    memory_range_t* range = NULL;
    FOR_EACH_ENTRY(range, &g_free_list, link) {
        if (range->page_count >= page_count) {
            range->page_count -= page_count;
            *base = (directptr_t)range + range->page_count * PAGE_SIZE;
            if (range->page_count == 0) {
                list_del(&range->link);
                goto cleanup;
            }
        }
    }

    CHECK_FAIL_ERROR(ERROR_OUT_OF_RESOURCES);

cleanup:
    release_lock(&g_pmm_lock);
    return err;
}

err_t pmm_free(directptr_t page, size_t page_count) {
    acquire_lock(&g_pmm_lock);

    memory_range_t* range = NULL;
    FOR_EACH_ENTRY(range, &g_free_list, link) {
        if (range == page + page_count * PAGE_SIZE) {
            list_del(&range->link);
            memory_range_t* new_range = page;
            new_range->page_count = range->page_count + page_count;
            list_add(&g_free_list, &new_range->link);
            goto cleanup;
        } else {
            range->page_count += page_count;
            goto cleanup;
        }
    }

    range = page;
    range->page_count = page_count;
    list_add_tail(&g_free_list, &range->link);

cleanup:
    release_lock(&g_pmm_lock);
    return NO_ERROR;
}

