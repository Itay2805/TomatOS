#ifndef __TOMATOS_KERNEL_ARCH_VMM_H__
#define __TOMATOS_KERNEL_ARCH_VMM_H__

#include <util/except.h>
#include <util/defs.h>
#include <sync/lock.h>
#include <stdint.h>

#ifdef __TOMATOS_AMD64__
    #define PAGE_SIZE SIZE_4KB

    // all the lower half to userspace
    #define USERSPACE_END               0x00007fffffffffffull

    // unlimited direct map
    #define DIRECT_MAPPING_BASE         0xffff800000000000ull

    // 1GB for kernel thread stacks
    #define KERNEL_STACK_HEAP_BASE      0xffffe00000000000ull
    #define KERNEL_STACK_HEAP_END       0xffffe00040000000ull

    // 1GB for kernel heap
    // (used for kernel objects)
    #define KERNEL_HEAP_BASE            0xffffe00080000000ull
    #define KERNEL_HEAP_END             0xffffe000c0000000ull

    // the kernel base
    #define KERNEL_BASE                 ((void*)0xffffffff80000000ull)
#else
    #error Unknown target arch
#endif

/**
 * Convert between physical and directs pointers
 */
#define PHYSICAL_TO_DIRECT(addr)    ((directptr_t)(((char*)(addr)) + DIRECT_MAPPING_BASE))
#define DIRECT_TO_PHYSICAL(addr)    ((physptr_t)(((char*)(addr)) - DIRECT_MAPPING_BASE))

/**
 * Represents a physical address
 */
typedef uint64_t physptr_t;

/**
 * Represents a pointer in the direct map
 */
typedef void* directptr_t;

typedef struct address_space {
    ticket_lock_t lock;

    // arch specific stuff
#ifdef __TOMATOS_AMD64__
    directptr_t pml4;
#else
    #error Unknown target arch
#endif
} address_space_t;


typedef enum map_flag {
    MAP_WRITE = BIT0,
    MAP_EXEC = BIT1,
} map_flag_t;

err_t set_address_space(address_space_t* space);

err_t vmm_map(address_space_t* space, void* virtual, physptr_t physical, map_flag_t flag);

err_t vmm_unmap(address_space_t* space, void* virtual);

/**
 * Copy from userspace into kernel space
 *
 * @param dst       [IN] The buffer to write to
 * @param src       [IN] The buffer to read from
 * @param size      [IN] The size of the buffer to copy
 */
err_t copy_from_user(void* dst, void* src, size_t size);

/**
 * Copy from kernel to userspace
 *
 * @param dst       [IN] The buffer to write to
 * @param src       [IN] The buffer to read from
 * @param size      [IN] The size of the buffer to copy
 */
err_t copy_to_user(void* dst, void* src, size_t size);

#endif //__TOMATOS_KERNEL_ARCH_VMM_H__
