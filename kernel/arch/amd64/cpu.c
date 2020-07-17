#include <util/except.h>
#include <arch/cpu.h>
#include <stdbool.h>
#include <mem/mm.h>
#include "intrin.h"

extern char g_bsp_cpu_local_buffer[];
extern char __cpu_local_size[];

size_t CPU_LOCAL g_cpu_id;

void cpu_pause() {
    asm volatile ("pause");
}

void memory_fence() {
    asm volatile ("" ::: "memory");
}

void init_cpu_local_for_bsp() {
    __wrmsr(MSR_IA32_GS_BASE, (uint64_t)g_bsp_cpu_local_buffer);
}

err_t init_cpu_local() {
    err_t err = NO_ERROR;

    void* locals = kalloc((size_t)__cpu_local_size);
    CHECK_ERROR(locals != NULL, ERROR_OUT_OF_RESOURCES);
    __wrmsr(MSR_IA32_GS_BASE, (uint64_t)locals);

cleanup:
    return err;
}

void disable_interrupts() {
    asm volatile("cli" ::: "memory");
}

void enable_interrupts() {
    asm volatile("sti" ::: "memory");
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
