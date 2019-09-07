#include <error.h>
#include <memory/mm.h>
#include "stdlib.h"

void* kmalloc(size_t size) {
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

void* krealloc(void* ptr, size_t size) {
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

void* kcalloc(size_t count, size_t size) {
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

void kfree(void *ptr) {
    error_t err = NO_ERROR;

    if(ptr) {
        CHECK_AND_RETHROW(mm_free(ptr));
    }

cleanup:
    CATCH(err);
}

