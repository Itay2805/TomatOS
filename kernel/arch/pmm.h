#ifndef __TOMATOS_KERNEL_ARCH_PMM_H__
#define __TOMATOS_KERNEL_ARCH_PMM_H__

#include <util/except.h>
#include <util/defs.h>
#include "vmm.h"

err_t pmm_submit_range(directptr_t start, size_t page_count);

err_t pmm_allocate(size_t page_count, directptr_t* base);

err_t pmm_free(directptr_t page, size_t page_count);

#endif //__TOMATOS_KERNEL_ARCH_PMM_H__
