#ifndef TOMATOS_CPU_H
#define TOMATOS_CPU_H

#include <util/defs.h>

#include <arch/amd64/intrin.h>

/**
 * The system context is saved into this struct on exceptions
 * so it can be nicely logged
 */
typedef struct system_context {
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
} system_context_t;

/**
 * The page fault params are passed to the page fault handler function
 * for handling it or crashing on it
 */
typedef union page_fault_params {
    struct {
        uint32_t present : 1;
        uint32_t write : 1;
        uint32_t user : 1;
        uint32_t reserved_write : 1;
        uint32_t instruction_fetch : 1;
    };
    uint32_t raw;
} page_fault_params_t;

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
void memory_barrier();

/**
 * TODO: This
 */
void store_fence();

/**
 * TODO: This
 */
void load_fence();

/**
 * TODO: This
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

/**
 * Get the cpu id, used to index into cpu specific cpu info
 */
size_t get_cpu_id();

/**
 * Used to set the id on boot
 */
void set_cpu_id(size_t id);

#endif //TOMATOS_CPU_H
