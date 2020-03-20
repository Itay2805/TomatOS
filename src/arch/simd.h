#ifndef __ARCH_SIMD_H__
#define __ARCH_SIMD_H__

#include <util/except.h>

/**
 * Initialize SIMD assuming the current CPU is the BSP
 */
void init_simd_for_bsp();

/**
 * Initialize SIMD for the current CPU
 *
 * @remark
 * This will fail if the cpu does nto support the same features as the BSP
 */
err_t init_simd_for_cpu();

/**
 * Create a new SIMD state and initialize it to a
 * default state
 *
 * @remark
 * This will return NULL if there is no FPU/SIMD support
 */
void* create_simd_state();

/**
 * Free an allocated SIMD state
 */
void free_simd_state(void* state);

/**
 * Save the current state into the given SIMD state
 */
extern void (*save_simd_state)(void* state);

/**
 * Restore the current state from the given SIMD state
 */
extern void (*restore_simd_state)(void* state);

#endif //__ARCH_SIMD_H__
