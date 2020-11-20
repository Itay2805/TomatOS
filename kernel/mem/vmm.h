#ifndef TOMATOS_VMM_H
#define TOMATOS_VMM_H

#include <arch/stivale2.h>
#include <util/defs.h>
#include <sync/lock.h>
#include <arch/cpu.h>
#include "mm.h"

err_t init_vmm(stivale2_struct_tag_memmap_t* memap);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Map and Unmap primitives
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct address_space {
    lock_t lock;
    directptr_t pml4;
} address_space_t;

err_t set_address_space(address_space_t* space);

typedef enum page_perms {
    MAP_READ   = 0u,
    MAP_WRITE  = BIT0,
    MAP_EXEC   = BIT1,
} page_perms_t;

err_t vmm_map(address_space_t* space, uintptr_t virt, physptr_t phys, size_t pages, page_perms_t perms);

err_t vmm_unmap(address_space_t* space, uintptr_t virt, size_t pages);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buffer handling
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Page fault handling
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * This is called from the arch specific code
 * whenever a page fault happens.
 *
 * @remark
 * Returning an error from here will cause the default
 * exception handler for the platform to fire. If no error
 * is returned then it is assumed that everything was fine
 * and the interrupt will return.
 *
 * @param addr      [IN] The fault address
 * @param params    [IN] The page fault parameters
 */
err_t vmm_handle_kernel_pagefault(uintptr_t addr, page_fault_params_t params);

#endif //TOMATOS_VMM_H
