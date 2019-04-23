#ifndef TOMATKERNEL_MEMORY_PMM
#define TOMATKERNEL_MEMORY_PMM

////////////////////////////////////////////////////////////////////////////
// Physical Memory Manager
////////////////////////////////////////////////////////////////////////////
//
// The physical memory manager is in charge of allocating and freeing
// physical pages, in TomatKernel the PMM simply tracks everything using
// a simple Bitmap, starting at the end of the kernel, having the space
// needed exactly for our memory size.
//
////////////////////////////////////////////////////////////////////////////

#include "../common/multiboot.h"
#include "../common/stdint.h"

/**
 * This contains the physical address of the end of the kernel
 * after this address all the rest of memory should be available
 * 
 * this is used for defining areas for static data like bitmaps for
 * the pmm and vmm
 */
extern uint32_t end_of_kernel;

/**
 * Initialize the physical memory manager
 */
void pmm_init(multiboot_info_t* multiboot);

/**
 * Map the kernel (including bitmaps or other statically allocated stuff)
 */
void pmm_map_kernel(void);

/**
 * Set the given physical address as unavailable
 */
void pmm_map(void* phys);

/**
 * Allocate free pages, must be linearly in the amount of pages
 * to allocate
 * 
 * Note: The count is in pages and not bytes!
 */
void* pmm_allocate(size_t count);

/**
 * Set a physical page as free
 */
void pmm_free(void* phys);

#endif