#ifndef __TOMATOS_KERNEL_ARCH_CPU_H__
#define __TOMATOS_KERNEL_ARCH_CPU_H__

#include <util/except.h>
#include <stdbool.h>

void cpu_pause();

void memory_fence();

void disable_interrupts();

void enable_interrupts();

bool are_interrupts_enabled();

#ifdef __TOMATOS_AMD64__
    #define CPU_LOCAL __attribute__((address_space(256), section(".cpu_local_data")))
#else
    #error Invalid architecture
#endif

extern size_t CPU_LOCAL g_cpu_id;

#endif //__TOMATOS_KERNEL_ARCH_CPU_H__
