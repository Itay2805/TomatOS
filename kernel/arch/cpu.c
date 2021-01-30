#include "intrin.h"
#include "cpu.h"

// declare globals
uintptr_t g_cpu_locals[256] = {0};
size_t g_cpu_count = 0;
size_t CPU_LOCAL g_cpu_id;
size_t CPU_LOCAL g_lapic_id;

void init_cpu_locals(size_t cpuid, size_t lapic_id) {
    // set the correct thing
    __wrmsr(MSR_IA32_GS_BASE, (uintptr_t)g_cpu_locals[cpuid]);

    // set some globals
    g_cpu_id = cpuid;
    g_lapic_id = lapic_id;
}

void cpu_pause() {
    asm volatile ("pause");
}

void cpu_sleep() {
    __hlt();
}

void memory_barrier() {
    asm volatile ("" ::: "memory");
}

void store_fence() {
    asm volatile ("sfence" ::: "memory");
}

void load_fence() {
    asm volatile ("lfence" ::: "memory");
}

void memory_fence() {
    asm volatile ("mfence" ::: "memory");
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

//size_t get_tsc_aux() {
//    size_t aux;
//    asm volatile ("rdtscp\n" : "=c" (aux) : : "eax", "edx");
//    return aux;
//}
//
//void set_tsc_aux(size_t id) {
//    __wrmsr(MSR_IA32_TSC_AUX, id);
//}
