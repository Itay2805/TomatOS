#include <util/string.h>
#include <util/list.h>
#include <sync/lock.h>
#include "pmm.h"

typedef struct memory_range {
    list_entry_t link;
    size_t page_count;
} memory_range_t;

static list_entry_t g_free_list = INIT_LIST(g_free_list);

static ticket_lock_t g_pmm_lock = {0};

err_t pmm_submit_range(directptr_t start, size_t page_count) {
    err_t err = NO_ERROR;

    CHECK_AND_RETHROW(pmm_free(start, page_count));

cleanup:
    return err;
}

err_t pmm_allocate(size_t page_count, directptr_t* base) {
    err_t err = NO_ERROR;

    ticket_lock(&g_pmm_lock);

    CHECK(base != NULL);

    memory_range_t* range = NULL;
    FOR_EACH_ENTRY(range, &g_free_list, link) {
        if (range->page_count >= page_count) {
            range->page_count -= page_count;
            *base = (directptr_t)range + range->page_count * PAGE_SIZE;
            if (range->page_count == 0) {
                list_del(&range->link);
            }
            goto cleanup;
        }
    }

    CHECK_FAIL_ERROR(ERROR_OUT_OF_RESOURCES);

cleanup:
    ticket_unlock(&g_pmm_lock);
    return err;
}

err_t pmm_allocate_zero(size_t page_count, directptr_t* base) {
    err_t err = NO_ERROR;

    CHECK_AND_RETHROW(pmm_allocate(page_count, base));
    memset(*base, 0, page_count * PAGE_SIZE);

cleanup:
    return err;
}

err_t pmm_free(directptr_t page, size_t page_count) {
    ticket_lock(&g_pmm_lock);

    memory_range_t* range = NULL;
    FOR_EACH_ENTRY(range, &g_free_list, link) {
        if (range == page + page_count * PAGE_SIZE) {
            list_del(&range->link);
            memory_range_t* new_range = page;
            new_range->page_count = range->page_count + page_count;
            list_add(&g_free_list, &new_range->link);
            goto cleanup;
        } else if(range + range->page_count * PAGE_SIZE == page) {
            range->page_count += page_count;
            goto cleanup;
        }
    }

    range = page;
    range->page_count = page_count;
    list_add_tail(&g_free_list, &range->link);

cleanup:
    ticket_unlock(&g_pmm_lock);
    return NO_ERROR;
}

