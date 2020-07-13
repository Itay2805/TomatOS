
#include <util/except.h>
#include "intrin.h"

extern void* g_bsp_cpu_local_buffer;

void cpu_pause() {
    asm volatile ("pause");
}

void memory_fence() {
    asm volatile ("" ::: "memory");
}

void init_cpu_local_for_bsp() {
    __wrmsr(MSR_IA32_GS_BASE, (uint64_t)g_bsp_cpu_local_buffer);
}


