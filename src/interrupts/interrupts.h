#ifndef TOMATKERNEL_INTERRUPTS_H
#define TOMATKERNEL_INTERRUPTS_H

#include <stdbool.h>
#include <stdint.h>
#include <common.h>
#include <error.h>

/**
 * All of the registers pushed from the IDT
 */
typedef struct registers {
    // the data segment to return to
    uint64_t ds;

    // general purpose registers
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

    // the interrupt number
    uint64_t int_num;

    // error code (if any)
    uint64_t error_code;

    // the rip (set this to return to another place)
    uint64_t rip;

    // the code segment to return to
    uint64_t cs;

    // the rflags
    uint64_t rflags;

    // the stack pointer and segment
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed)) registers_t;

/**
 * A common interrupt handler
 *
 * Note: This is not the one called from the IDT, but rather one called
 *       from the IDT stubs
 */
typedef error_t(*interrupt_handler_f)(registers_t* registers);

/**
 * Enable interrupts
 */
#define _sti() asm volatile ("sti")

/**
 * Clear interrupts
 */
#define _cli() asm volatile ("cli")

/**
 * Halt until the next instruction
 */
#define _hlt() asm volatile ("hlt")

/**
 * Load the idt
 */
#define _lidt(addr) asm volatile("lidt %0" : : "m"(addr))

#endif //TOMATKERNEL_INTERRUPTS_H
