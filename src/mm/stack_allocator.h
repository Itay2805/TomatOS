#ifndef __MM_STACK_ALLOCATOR_H__
#define __MM_STACK_ALLOCATOR_H__

#include <stdint.h>

#define STACK_SIZE (1024u * 8u)

/**
 * Allocate a stack for a kernel thread
 */
uintptr_t allocate_stack();

/**
 * Free the stack
 */
void free_stack(uintptr_t addr);

#endif //__MM_STACK_ALLOCATOR_H__
