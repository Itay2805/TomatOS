#ifndef __SMP_PCPU_H__
#define __SMP_PCPU_H__

#include <stdint.h>

#define CPU_LOCAL __attribute__((address_space(256), section(".pcpu_data")))

// these are defined in the linker script
extern CPU_LOCAL uint64_t current_cpu_index;
extern CPU_LOCAL uint64_t kernel_stack;

/**
 * Initialize the per cpu storage on this cpu
 */
void pcpu_init_for_cpu();

/**
 * Initialize the pcpu data for the BSP
 *
 * This is done from a statically allocated section
 */
void pcpu_init_for_bsp();

/**
 * Get the per cpu storage base for the current cpu
 */
uintptr_t get_pcpu_base();

#endif //__SMP_PCPU_H__
