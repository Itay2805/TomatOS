#ifndef __TOMATOS_KERNEL_ARCH_TIMING_H__
#define __TOMATOS_KERNEL_ARCH_TIMING_H__

#include <stdint.h>

typedef enum timer_implementation {
    TIMER_ACPI,
#ifdef __TOMATOS_AMD64__
    TIMER_HPET,
#endif
} timer_implementation_t;

/**
 * The timer to use for timing
 */
extern timer_implementation_t g_timer;

/**
 * Stall in nanoseconds
 */
void ustall(uint64_t nanoseconds);

/**
 * Stall in microseconds
 */
void stall(uint64_t microseconds);

#endif //__TOMATOS_KERNEL_ARCH_TIMING_H__
