#include <util/except.h>
#include <arch/cpu.h>
#include <stdbool.h>
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

void disable_interrupts() {
    asm volatile("cli" ::: "memory");
}

void enable_interrupts() {
    asm volatile("sli" ::: "memory");
}

bool are_interrupts_enabled() {
    IA32_RFLAGS flags;
    asm volatile (
        "pushfq\n"
        "pop %0"
        : "=r" (flags.raw)
    );
    return flags.IF == 1;
}
