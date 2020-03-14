#ifndef __MM_VMM_H__
#define __MM_VMM_H__

#include <sync/spinlock.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tboot.h>
#include <smp/pcpu.h>
#include <util/list.h>
#include "pmm.h"

#define USERSPACE_END               0x00007fffffffffffull

// ~140TB of tb for physical memory mappings
#define DIRECT_MAPPING_BASE         0xffff800000000000ul
#define DIRECT_MAPPING_END          0xffffffeefffffffful

// 64GB for stacks
#define STACK_ALLOCATION_BASE       0xffffffef00000000ul
#define STACK_ALLOCATION_END        0xfffffffefffffffful


#define PHYSICAL_TO_DIRECT(addr)    ((typeof(addr))(((char*)(addr)) + DIRECT_MAPPING_BASE))
#define DIRECT_TO_PHYSICAL(addr)    ((typeof(addr))(((char*)(addr)) - DIRECT_MAPPING_BASE))
#define IS_KERNEL_PTR(ptr)          ((uintptr_t)(ptr) > DIRECT_MAPPING_BASE)
#define IS_STACK_PTR(ptr)           ((uintptr_t)(ptr) > STACK_ALLOCATION_BASE && (uintptr_t)(ptr) < STACK_ALLOCATION_END)

typedef enum page_perms {
    PAGE_WRITE = 1u << 0u,
    PAGE_EXECUTE = 1u << 1u,
    PAGE_SUPERVISOR = 1u << 2u,
    PAGE_MAP_ZERO = 1u << 3u,

    PAGE_READONLY = 0,
    PAGE_READWRITE = PAGE_WRITE,
    PAGE_EXEC_READ = PAGE_EXECUTE,
    PAGE_EXEC_READWRITE = PAGE_EXECUTE | PAGE_WRITE,
    PAGE_SUPERVISOR_READONLY = PAGE_SUPERVISOR,
    PAGE_SUPERVISOR_READWRITE = PAGE_SUPERVISOR | PAGE_WRITE,
    PAGE_SUPERVISOR_EXEC_READ = PAGE_SUPERVISOR | PAGE_EXECUTE,
    PAGE_SUPERVISOR_EXEC_READWRITE = PAGE_SUPERVISOR | PAGE_EXECUTE | PAGE_WRITE,
} page_perms_t;

//#define DEFAULT_CACHE IA32_PAT_MEMTYPE_WB
#define DEFAULT_CACHE 0

typedef enum page_type {
    PAGE_TYPE_4KB,
    PAGE_TYPE_2MB,
    PAGE_TYPE_1GB,
} page_type_t;

/*
 * Represents a page table
 */
typedef struct vmm_handle {
    uintptr_t pml4_physical;
    spinlock_t lock;
    list_entry_t umem;
} vmm_handle_t;

/**
 * Will initialize the kernel memory mapping
 *
 * - maps the kernel
 * - maps a direct mapping for all the good memory handles
 */
void vmm_init(tboot_info_t* info);

/**
 * This will enable special memory related features
 * for the current cpu
 */
void vmm_enable_cpu_features();

/**
 * Will convert the given virtual address to its physical address
 *
 * TODO: Add get permissions
 *
 * @param handle    [IN]    The page table handle
 * @param virtual   [IN]    The virtual address
 * @param physical  [OUT]   The output physical address
 *
 * @retval true - the page was found
 * @retval false - the page was not found
 */
bool vmm_virtual_to_physical(vmm_handle_t* handle, uintptr_t virtual, uintptr_t* physical, page_type_t* type);

/**
 * Get the current vmm handle
 */
vmm_handle_t* vmm_get_handle();

/**
 * Will switch to the given address space
 */
void vmm_set_handle(vmm_handle_t* handle);

/**
 * Create a new page table to be used in user space
 *
 * This will make sure the higher half shared with this address space
 *
 * @param handle    [OUT] The out handle
 */
void vmm_create_address_space(vmm_handle_t* handle);

/**
 * Will destroy and free the vmm handle
 *
 * This will handle both freeing allocated pages and freeing the page
 * table itself
 *
 * @param handle    [IN] The handle to destroy
 */
void vmm_destroy_page_table(vmm_handle_t* handle);

/**
 * Will check if the range of addresses is virtually mapped
 *
 * @param handle    [IN] The handle to check
 * @param addr      [IN] The starting address to check
 * @param size      [IN] The range to check
 *
 * @retval true - the page is mapped
 * @retval false - the page is not mapped
 */
bool vmm_is_mapped(vmm_handle_t* handle, uintptr_t addr, size_t size);

/**
 * Will unmap the range of physical addresses
 *
 * @remark
 * This will not free the physical pages from the pmm! if you want to free them you gotta do it yourself!
 *
 * @param handle    [IN] The handle
 * @param addr      [IN] The virtual address to unmap
 * @param size      [IN] The amount of bytes to unmap
 */
void vmm_unmap(vmm_handle_t* handle, uintptr_t addr, size_t page_count);

/**
 * Will map the given range of addresses at the virtual address
 *
 * @remark
 * Thsi will not free the physical page nor actually free it if it was allocated with the vmm_allocate
 * function
 *
 * @param handle    [IN] The handle to the page map
 * @param phys      [IN] The physical address to map from
 * @param virt      [IN] The virtual address to start at
 * @param size      [IN] The amount of bytes to map
 * @param perms     [IN] The permissions of the range
 * @param cache     [IN] The cache policy for the range
 */
void vmm_map(vmm_handle_t* handle, uintptr_t phys, uintptr_t virt, size_t size, page_perms_t perms, uint32_t /* IA32_PAT_MEMTYPE */ cache);

/**
 * Will set the page perms to the new perms
 *
 * @param handle    [IN] The handle to the page map
 * @param virt      [IN] The virtual address to start from
 * @param size      [IN] The amount of bytes to change
 * @param perms     [IN] The new permissions
 */
void vmm_set_perms(vmm_handle_t* handle, uintptr_t virt, size_t size, page_perms_t perms);

/**
 * Allocate virtual memory for the user (mapped at lower memory)
 *
 * @remark
 * Using the PAGE_SUPERVISOR bit will cause an assert
 *
 * @param handle        [IN]    The VMM handle
 * @param type          [IN]    The allocation type
 * @param virt          [OUT]   The output virtual address, also used as base for ALLOCATE_BELOW and ALLOCATE_ADDRESS
 * @param page_count    [IN]    The amount of pages to allocate
 * @param perms         [IN]    The permissions
 */
err_t vmm_user_allocate(vmm_handle_t* handle, allocate_type_t type, uintptr_t* virt, size_t page_count, page_perms_t perms);

/**
 * Free memory allocated for userspace
 *
 * @param handle        [IN] The VMM handle
 * @param virt          [IN] The base to free from
 * @param page_count    [IN] The amount of pages to free
 */
err_t vmm_user_free(vmm_handle_t* handle, uintptr_t virt, size_t page_count);

/**
 * Create the userspace memory allocator for the given handle
 *
 * @param handle        [IN] The handle
 */
void vmm_create_user_mem(vmm_handle_t* handle);

/**
 * Destroy the userspace memory allocator of the handle
 *
 * @param handle        [IN] The handle
 */
void vmm_destroy_user_mem(vmm_handle_t* handle);

#endif //__MM_VMM_H__
