#ifndef TOMATOS_CPU_H
#define TOMATOS_CPU_H

#include <util/defs.h>

#ifdef __TOMATOS_AMD64__
    #include <arch/amd64/intrin.h>
#endif

typedef struct system_context {
#ifdef __TOMATOS_AMD64__
    uint64_t ds;
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t int_num;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    IA32_RFLAGS rflags;
    uint64_t rsp;
    uint64_t ss;

    // arch generic regs
    #define IP rip
    #define SP rsp
    #define ARG0 rdi
    #define ARG1 rsi
#else
    #error Unknown achitecture
#endif
} system_context_t;

/**
 * Initialize a new system context
 */
void init_context(system_context_t* target, bool kernel);

/**
 * Will save the current context to the current thread
 */
void save_context(system_context_t* curr);

/**
 * Will restore the context of the current thread to the current context
 */
void restore_context(system_context_t* curr);

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

#endif //TOMATOS_CPU_H
