#ifndef __TOMATOS_KERNEL_ARCH_CPU_H__
#define __TOMATOS_KERNEL_ARCH_CPU_H__

#include <util/except.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * CPU_LOCAL macro definition per-architecture
 */
#ifdef __TOMATOS_AMD64__
    /**
     * For x86_64 we are using gs-relatives to make
     * the cpu locals
     */
    #define CPU_LOCAL __attribute__((address_space(256), section(".cpu_local_data")))
#else
#error Invalid architecture
#endif

/**
 * The id of the current cpu
 */
extern size_t CPU_LOCAL g_cpu_id;

/**
 * Pause the cpu for a bit, used around spinlocks
 * and alike
 */
void cpu_pause();

/**
 * Makes the cpu go to sleep until an interrupt
 * arrives
 */
void cpu_sleep();

/**
 * Memory fence to serialize memory accesses
 */
void memory_fence();

/**
 * Disable interrupts on the current cpu
 */
void disable_interrupts();

/**
 * Enable interrupts on the current cpu
 */
void enable_interrupts();

/**
 * Checks if interrupts are enabled on the current cpu
 */
bool are_interrupts_enabled();

/**
 * Sets when the next scheduler tick
 * should fire
 */
void set_next_scheduler_tick(uint64_t ms);

#endif //__TOMATOS_KERNEL_ARCH_CPU_H__
