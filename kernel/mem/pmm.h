#ifndef TOMATOS_PMM_H
#define TOMATOS_PMM_H

#include <arch/stivale2.h>
#include <util/defs.h>
#include "mm.h"

/**
 * Initialize the pmm with
 * @param memap
 */
void init_pmm(stivale2_struct_tag_memmap_t* memap);

void pmm_reclaim_bootloader_memory(stivale2_struct_tag_memmap_t* memap);

__attribute__((assume_aligned(4096)))
directptr_t page_alloc();

void page_free(directptr_t page);

void page_ref(directptr_t page);

#endif //TOMATOS_PMM_H
