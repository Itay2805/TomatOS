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

typedef struct kernel_info {
    size_t size;
    uintptr_t start;
} kernel_info_t;

extern kernel_info_t kernel_info;

#endif