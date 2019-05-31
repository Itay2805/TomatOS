#ifndef TOMATKERNEL_VMM_H
#define TOMATKERNEL_VMM_H

#include <error.h>
#include <boot/multiboot.h>
#include <stddef.h>

#define VMM_ATTR_EXECUTE    (1 << 0)
#define VMM_ATTR_USER       (1 << 1)
#define VMM_ATTR_WRITE      (1 << 2)

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
 * @param address_space     [IN] The address space we are operating on
 * @param virtual_address   [IN] The virtual page to unmap
 *
 * @return NO_ERROR: Success
 */
error_t vmm_unmap(address_space_t address_space, void* virtual_address);

#endif //TOMATKERNEL_VMM_H
