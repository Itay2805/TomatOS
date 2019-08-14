#ifndef TOMATKERNEL_STDLIB_H
#define TOMATKERNEL_STDLIB_H

#include "stddef.h"

void* kmalloc(size_t size);
void* krealloc(void* ptr, size_t size);
void* kcalloc(size_t count, size_t size);
void kfree(void *ptr);


#endif //TOMATKERNEL_STDLIB_H
