#ifndef __TOMATOS_KERNEL_ARCH_INTS_H__
#define __TOMATOS_KERNEL_ARCH_INTS_H__

#include <proc/event.h>
#include <arch/amd64/intrin.h>

#ifdef __TOMATOS_AMD64__
    #define INTERRUPT_COUNT (256 - 0x21)
#else
    #error Unknown achitecture
#endif

/**
 * This will cause the thread to wait for an interrupt
 */
void wait_for_interrupt(uint8_t vector);
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

    // system context regs
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
 * This will tell the kernel we want to route an irq
 * to the given vector
 *
 * @param irq       [IN]    The irq to register
 * @param vector    [OUT]   The vector in which the irq will fire
 */
err_t register_irq(uint8_t irq, uint8_t* vector);

/**
 * Will request an empty interrupt vector from
 * the kernel
 */
uint8_t allocate_vector();

#endif //__TOMATOS_KERNEL_ARCH_INTS_H__
