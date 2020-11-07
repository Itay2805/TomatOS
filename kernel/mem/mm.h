#ifndef TOMATOS_MM_H
#define TOMATOS_MM_H

#include <util/defs.h>

typedef void* directptr_t;
typedef uint64_t physptr_t;

#define MAX_PHYS_MEMORY_SIZE    (SIZE_256GB)

#ifdef __TOMATOS_AMD64__
    #define PAGE_SIZE           (SIZE_4KB)
    #define DIRECT_BASE         (0xFFFF800000000000ull)
    #define KERNEL_BASE         (0xffffffff80000000ull)
#else
#error Unknown ARCH
#endif

#define DIRECT_END              (0xFFFF800000000000ull + MAX_PHYS_MEMORY_SIZE)

#define MAX_PAGE_COUNT          (MAX_PHYS_MEMORY_SIZE / PAGE_SIZE)

#define PAGE_REFCOUNT_START     (DIRECT_END + SIZE_1GB)
#define PAGE_REFCOUNT_SIZE      (ALIGN_UP((MAX_PHYS_MEMORY_SIZE / PAGE_SIZE) * sizeof(uint16_t), PAGE_SIZE))
#define REFCOUNT_END            (PAGE_REFCOUNT_START + PAGE_REFCOUNT_SIZE)

_Static_assert(KERNEL_BASE > REFCOUNT_END, "kernel overlaps with virtual memory regions");

#define DIRECT_TO_PHYS(dir) ((physptr_t)((uint8_t*)(dir) - DIRECT_BASE))
#define PHYS_TO_DIRECT(dir) ((directptr_t)((uint8_t*)(dir) + DIRECT_BASE))

#endif //TOMATOS_MM_H
