#ifndef __TOMATOS_KERNEL_ARCH_MM_H__
#define __TOMATOS_KERNEL_ARCH_MM_H__

#include <stddef.h>
#include <util/except.h>

/**
 * Initialize the kernel heap
 */
err_t mm_init();

/**
 * Allocate physicall cont memory
 */
void* kalloc(size_t size);

/**
 * Reallocate physically cont memory
 */
void* krealloc(void* ptr, size_t size);

/**
 * Free physically cont memory
 */
void kfree(void* ptr);

/**
 * Allocate a stack that can be used for a kernel
 * thread
 */
void* alloc_stack();

/**
 * Free a kernel thread task
 */
void free_stack(void* stk);

#endif //__TOMATOS_KERNEL_ARCH_MM_H__