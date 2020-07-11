#ifndef __TOMATOS_KERNEL_ARCH_VMM_H__
#define __TOMATOS_KERNEL_ARCH_VMM_H__

#include <stdint.h>

#define USERSPACE_END               0x00007fffffffffffull
#define DIRECT_MAPPING_BASE         0xffff800000000000ul
#define DIRECT_MAPPING_END          0xffffffeefffffffful

#define PHYSICAL_TO_DIRECT(addr)    ((directptr_t)(((char*)(addr)) + DIRECT_MAPPING_BASE))
#define DIRECT_TO_PHYSICAL(addr)    ((physptr_t)(((char*)(addr)) - DIRECT_MAPPING_BASE))


typedef uint64_t physptr_t;
typedef void* directptr_t;

#define PAGE_SIZE SIZE_4KB

#endif //__TOMATOS_KERNEL_ARCH_VMM_H__
