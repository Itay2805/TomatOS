#ifndef TOMATKERNEL_VMM_H
#define TOMATKERNEL_VMM_H

#include <util/error.h>
#include <util/arch.h>
#include <tboot.h>

#include <stdbool.h>
#include <stdint.h>
#include <util/sync.h>
#include <util/list.h>

#define DIRECT_MAPPING_BASE         0xFFFF800000000000ul
#define PHYSICAL_TO_DIRECT(addr)    ((typeof(addr))(((char*)(addr)) + DIRECT_MAPPING_BASE))
#define DIRECT_TO_PHYSICAL(addr)    ((typeof(addr))(((char*)(addr)) - DIRECT_MAPPING_BASE))
#define IS_KERNEL_PTR(ptr)          ((uintptr_t)(ptr) > DIRECT_MAPPING_BASE)

typedef enum page_perms {
    PAGE_WRITE = 1 << 0,
    PAGE_EXECUTE = 1 << 1,
    PAGE_SUPERVISOR = 1 << 2,
    PAGE_MAP_ZERO = 1 << 3,

    PAGE_READONLY = 0,
    PAGE_READWRITE = PAGE_WRITE,
    PAGE_EXEC_READ = PAGE_EXECUTE,
    PAGE_EXEC_READWRITE = PAGE_EXECUTE | PAGE_WRITE,
    PAGE_SUPERVISOR_READONLY = PAGE_SUPERVISOR,
    PAGE_SUPERVISOR_READWRITE = PAGE_SUPERVISOR | PAGE_WRITE,
    PAGE_SUPERVISOR_EXEC_READ = PAGE_SUPERVISOR | PAGE_EXECUTE,
    PAGE_SUPERVISOR_EXEC_READWRITE = PAGE_SUPERVISOR | PAGE_EXECUTE | PAGE_WRITE,
} page_perms_t;

#define DEFAULT_CACHE IA32_PAT_MEMTYPE_WB

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
    list_entry_t map;
    lock_t lock;
} vmm_handle_t;

/**
 * The kernel's vmm handle
 *
 * @remark
 * DO NOT USE AFTER THE THREADING HAS BEEN INITIALIZED
 * ONCE THREADING IS INITIALIZED USE kernel_process->vmm_handle
 */
extern vmm_handle_t kernel_handle;

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
 * @retval true - the page was not found
 * @retval false - the page was found
 */
bool vmm_virtual_to_physical(vmm_handle_t* handle, uintptr_t virtual, uintptr_t* physical, page_type_t* type);

/**
 * Will switch to the given address space
 *
 * TODO: Store reference to the handle in the per cpu storage
 */
void vmm_set_handle(vmm_handle_t* handle);

/**
 * Create a new page table to be used in user space
 *
 * This will make sure the higher half shared with this address space
 *
 * @param handle    [OUT] The out handle
 */
void vmm_create_page_table(vmm_handle_t* handle);

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
 * @retval true - the page was not found
 * @retval false - the page was found
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
 * @param size      [IN] The size to unmap
 */
void vmm_unmap(vmm_handle_t* handle, uintptr_t addr, size_t size);

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
void vmm_map(vmm_handle_t* handle, uintptr_t phys, uintptr_t virt, size_t size, page_perms_t perms, IA32_PAT_MEMTYPE cache);

/**
 * Allocate and map a page into *userspace*
 *
 * if the virt address is set to a value which is not 0, the address will be used as the base that will be
 * used to allocate
 *
 * @param handle    [IN]        Vmm handle
 * @param virt      [IN/OUT]    virtual address
 * @param size      [IN]        The size that is going to be allocated
 * @param perms     [IN]        Page permissions
 */
error_t vmm_allocate(vmm_handle_t* handle, uintptr_t* virt, size_t size, page_perms_t perms);

/**
 * Free the page and unmap it
 *
 * TODO: TLB shootdown and fun stuff
 *
 * @param handle    [IN]    The vmm handle
 * @param virt      [IN]    The virt base to free from
 * @param size      [IN]    The size to free and unmap
 */
error_t vmm_free(vmm_handle_t* handle, uintptr_t virt, size_t size);

#endif //TOMATKERNEL_VMM_H
