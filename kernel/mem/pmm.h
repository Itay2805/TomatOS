#ifndef TOMATOS_PMM_H
#define TOMATOS_PMM_H

#include <arch/stivale2.h>
#include <util/defs.h>
#include "mm.h"

/**
 * Initialize the pmm with
 */
err_t init_pmm();

/**
 * Allocate a single page
 */
directptr_t page_alloc();

/**
 * Free a single page
 */
void page_free(directptr_t ptr);

/**
 * Allocate physical memory, this will try to first allocate from high
 * memory and only if that is out it will allocate from low memory
 */
directptr_t palloc(size_t size);

/**
 * Allocate physical memory from low memory (<4GB) this is needed for
 * some drivers but should be avoided if not needed
 */
directptr_t palloc_low(size_t size);

/**
 * Like palloc but will first zero the memory
 */
directptr_t pallocz(size_t size);

/**
 * like palloc_low, but will first zero the memory
 */
directptr_t pallocz_low(size_t size);

/**
 * Free physical memory, works for both low and higher memory
 */
void pfree(directptr_t ptr, size_t size);

#endif //TOMATOS_PMM_H
