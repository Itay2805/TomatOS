#include "intrin.h"

void cpu_pause() {
    asm volatile ("pause");
}

void cpu_sleep() {
    __hlt();
}

void memory_fence() {
    asm volatile ("" ::: "memory");
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

