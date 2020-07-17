#ifndef __TOMATOS_KERNEL_ARCH_INTS_H__
#define __TOMATOS_KERNEL_ARCH_INTS_H__

#include <proc/event.h>
#include <arch/amd64/intrin.h>

#ifdef __TOMATOS_AMD64__
    #define INTERRUPT_COUNT 256
#else
    #error Unknown achitecture
#endif

/**
 * This has an event for every possible interrupt, allowing
 * for threads to wait for a certain interrupt
 */
extern event_t g_interrupt_events[INTERRUPT_COUNT];

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
#else
    #error Unknown achitecture
#endif
} system_context_t;

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
