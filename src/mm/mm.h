#ifndef __MM_MM_H__
#define __MM_MM_H__

#include <stddef.h>

void mm_init();

void* mm_allocate(size_t size);
void* mm_reallocate(void *ptr, size_t size);
void mm_free(void *ptr);
void* mm_allocate_aligned();

#endif //__MM_MM_H__
