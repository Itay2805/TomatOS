#include <error.h>
#include <memory/mm.h>
#include <memory/pmm.h>
#include <memory/vmm.h>
#include <common/common.h>
#include "stdlib.h"

void* vmalloc(size_t size) {
    error_t err = NO_ERROR;
    void* ptr = NULL;

    if(size == 0) {
        size = 1;
    }

    CHECK_AND_RETHROW(mm_allocate(size, &ptr));
    CHECK(ptr != NULL);

cleanup:
    if(err != NO_ERROR) {
        CATCH(err);
        ptr = NULL;
    }
    return ptr;
}

void* vrealloc(void *ptr, size_t size) {
    error_t err = NO_ERROR;

    CHECK_AND_RETHROW(mm_reallocate(&ptr, size));
    CHECK(ptr != NULL);

cleanup:
    if(err != NO_ERROR) {
        CATCH(err);
        ptr = NULL;
    }
    return ptr;
}

void* vcalloc(size_t count, size_t size) {
    error_t err = NO_ERROR;
    void* ptr = NULL;

    CHECK_AND_RETHROW(mm_allocate(size * count, &ptr));
    CHECK(ptr != NULL);

cleanup:
    if(err != NO_ERROR) {
        CATCH(err);
        ptr = NULL;
    }
    return ptr;
}

void vfree(void *ptr) {
    error_t err = NO_ERROR;

    if(ptr) {
        CHECK_AND_RETHROW(mm_free(ptr));
    }

cleanup:
    CATCH(err);
}

void* pmalloc(size_t size) {
    error_t err = NO_ERROR;
    uintptr_t phys = 0;

    // TODO: support more than a single page
    CHECK_AND_RETHROW(pmm_allocate(&phys, ALIGN_PAGE_UP(size) / KB(4)));

cleanup:
    if(err != NO_ERROR) {
        CATCH(err);
        return NULL;
    }
    return (void *) CONVERT_TO_DIRECT(phys);
}

void pfree(void* ptr, size_t size) {
    error_t err = NO_ERROR;

    CHECK((uintptr_t)ptr > DIRECT_MAPPING_BASE);
    CHECK_AND_RETHROW(pmm_free((uintptr_t)(ptr) - DIRECT_MAPPING_BASE, ALIGN_PAGE_UP(size) / KB(4)));

cleanup:
    CATCH(err);
}
