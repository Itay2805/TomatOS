#include <error.h>
#include <memory/mm.h>
#include "stdlib.h"

void* malloc(size_t size) {
    error_t err = NO_ERROR;
    void* ptr = NULL;

    CHECK_AND_RETHROW(mm_allocate(size, &ptr));

cleanup:
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    error_t err = NO_ERROR;

    CHECK_AND_RETHROW(mm_reallocate(&ptr, size));

cleanup:
    return ptr;
}

void* calloc(size_t count, size_t size) {
    error_t err = NO_ERROR;
    void* ptr = NULL;

    CHECK_AND_RETHROW(mm_allocate(size * count, ptr));

cleanup:
    return ptr;
}

void free(void* ptr) {
    error_t err = NO_ERROR;

    if(ptr) {
        CHECK_AND_RETHROW(mm_free(ptr));
    }

cleanup:
    return;
}

