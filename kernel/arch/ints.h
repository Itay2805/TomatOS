#ifndef __TOMATOS_KERNEL_ARCH_INTS_H__
#define __TOMATOS_KERNEL_ARCH_INTS_H__

#include <proc/event.h>

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
    uint64_t rip;
#else
    #error Unknown achitecture
#endif
} system_context_t;

#endif //__TOMATOS_KERNEL_ARCH_INTS_H__
