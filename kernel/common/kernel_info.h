#ifndef COMMON_KERNEL_INFO_H
#define COMMON_KERNEL_INFO_H

////////////////////////////////////////////////////////////////////////////
// Kernel info
////////////////////////////////////////////////////////////////////////////
//
// Has some information about the kernel
//
////////////////////////////////////////////////////////////////////////////

#include "stdint.h"

extern void* kernel_start;
extern void* kernel_end;
extern void* kernel_stack;

#define KERNEL_START (uint64_t)(&kernel_start)
#define KERNEL_END (uint64_t)(&kernel_end)
#define KERNEL_STACK (void*)(&kernel_stack)

#endif