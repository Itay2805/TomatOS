#include <error.h>
#include <memory/mm.h>
#include "stdlib.h"

void* malloc(size_t size) {
    void* ptr = NULL;
    mm_allocate(size, ptr);
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    mm_reallocate(&ptr, size);
    return ptr;
}

void* calloc(size_t count, size_t size) {
    void* ptr = NULL;
    mm_allocate(size * count, ptr);
    return ptr;
}

void free(void* ptr) {
    if(ptr) {
        mm_free(ptr);
    }
}

