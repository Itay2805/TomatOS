#ifndef TOMATKERNEL_VMM_H
#define TOMATKERNEL_VMM_H

#include <error.h>
#include <boot/multiboot.h>
#include <stddef.h>

#define PAGE_ATTR_EXECUTE    (1 << 0)
#define PAGE_ATTR_USER       (1 << 1)
#define PAGE_ATTR_WRITE      (1 << 2)

#define PHYSICAL_ADDRESS(addr) ((void*)((uintptr_t)addr + 0xFFFF800000000000))

// TODO: Add locks to all of these

typedef uint64_t address_space_t;

/**
 * Initialize the virtual memory manager
 */
error_t vmm_init(multiboot_info_t* info);

/**
 * Set the address space
 *
 * @remark
 * Make sure this mapping contains the currently running code mapped as executeable!
 */
void vmm_set(address_space_t address_space);

/**
 * Get the current address space
 */
address_space_t vmm_get();

/**
 * Map a virtual address to a physical address
 *
 * @param address_space     [IN] The address space we are operating on
 * @param virtual_address   [IN] The virtual page to map to
 * @param physical_address  [IN] The physical address to map
 * @param attributes        [IN] The attributes the virtual page will have
 *
 * @return NO_ERROR: Success
 * @return ERROR_OUT_OF_MEMORY: We ran out of memory
 */
error_t vmm_map(address_space_t address_space, void* virtual_address, void* physical_address, int attributes);

/**
 * Unmap the virtual address
 *
 * @remark
 * This will not free the physical page, but will leave it as is
 *
 * @param address_space     [IN] The address space we are operating on
 * @param virtual_address   [IN] The virtual page to unmap
 *
 * @return NO_ERROR: Success
 */
error_t vmm_unmap(address_space_t address_space, void* virtual_address);

/**
 * Allocate a new page, this will also allocate the physical page
 *
 * @param address_space     [IN] The address space to allocate in
 * @param virtual_address   [IN] The virtual address to allocate
 * @param attributes        [IN] The attributes of the new page
 */
error_t vmm_allocate(address_space_t address_space, void* virtual_address, int attributes);

/**
 * Free the virtual page including the physical page
 *
 * @param address_space     [IN] The address space to free from
 * @param virtual_address   [IN] The virtual address to free
 */
error_t vmm_free(address_space_t address_space, void* virtual_address);

/**
 * Allocate a buffer from one address space to another
 *
 * @param dst_addrspace     [IN] The address space we are going to write to
 * @param dst               [IN] The virtual address of the destination buffer in that address space
 * @param src_addrspace     [IN] The address space of the source buffer
 * @param src               [IN] The virtual address of the destination buffer in that address space
 * @param size              [IN] The size of the buffer we are going to copy
 */
error_t vmm_copy(address_space_t dst_addrspace, void* dst, address_space_t src_addrspace, void* src, size_t size);

#endif //TOMATKERNEL_VMM_H
