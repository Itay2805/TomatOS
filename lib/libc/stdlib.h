#ifndef TOMATKERNEL_STDLIB_H
#define TOMATKERNEL_STDLIB_H

#include "stddef.h"

void* vmalloc(size_t size);
void* vrealloc(void *ptr, size_t size);
void* vcalloc(size_t count, size_t size);
void vfree(void *ptr);

/**
 * Will allocate a physically contiguous physical memory (will return a direct mapping address)
 *
 * @remark
 * The actual minimal allocation is page aligned
 */
void* pmalloc(size_t size);
void pfree(void* ptr, size_t size);

#endif //TOMATKERNEL_STDLIB_H
