#ifndef __TOMATOS_KERNEL_ARCH_TIMING_H__
#define __TOMATOS_KERNEL_ARCH_TIMING_H__

#include <stdint.h>

/**
 * Stall in nanoseconds
 */
void ustall(uint64_t nanoseconds);

/**
 * Stall in microseconds
 */
void stall(uint64_t microseconds);

/**
 * Return the uptime in millisecs
 */
uint64_t uptime();

#endif //__TOMATOS_KERNEL_ARCH_TIMING_H__
