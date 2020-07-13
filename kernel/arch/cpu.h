#ifndef __TOMATOS_KERNEL_ARCH_CPU_H__
#define __TOMATOS_KERNEL_ARCH_CPU_H__

#include <util/except.h>

void cpu_pause();

void memory_fence();

#ifdef __TOMATOS_AMD64__
    #define CPU_LOCAL __attribute__((address_space(256), section(".cpu_local_data")))
#else
    #error Invalid architecture
#endif

#endif //__TOMATOS_KERNEL_ARCH_CPU_H__
