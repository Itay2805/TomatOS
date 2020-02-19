#ifndef __INTR_INTR_H__
#define __INTR_INTR_H__

#include <arch/intrin.h>

#include <util/except.h>
#include <util/list.h>

#include <stdint.h>

typedef struct interrupt_context {
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
    IA32_EFLAGS32 rflags;

    // the stack pointer and segment
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed)) interrupt_context_t;

enum {
    // magic interrupt value to let the kernel allocate an interrupt
    // vector
    INT_ALLOCATE = -1,

    // CPU Exceptions
    EXCEPTION_DIVIDE_ERROR = 0,
    EXCEPTION_DEBUG = 1,
    EXCEPTION_NMI = 2,
    EXCEPTION_BREAKPOINT = 3,
    EXCEPTION_OVERFLOW = 4,
    EXCEPTION_BOUND = 5,
    EXCEPTION_INVALID_OPCODE = 6,
    EXCEPTION_DOUBLE_FAULT = 7,
    EXCEPTION_INVALID_TSS = 8,
    EXCEPTION_SEG_NOT_PRESENT = 9,
    EXCEPTION_STACK_FAULT = 10,
    EXCEPTION_GP_FAULT = 11,
    EXCEPTION_PAGE_FAULT = 12,
    EXCEPTION_FP_ERROR = 13,
    EXCEPTION_ALIGNMENT_CHECK = 14,
    EXCEPTION_MACHINE_CHECK = 15,
    EXCEPTION_SIMD = 16,

    // IPIs
    IPI_YIELD = 0x20,

    // from here we can allocate it dynamically
    INT_ALLOCATION_BASE,
};

typedef struct interrupt {
    int vector;
    const char* name;
    void* user_param;
    err_t (*callback)(void* user_param, interrupt_context_t* ctx);
    list_entry_t link;
} interrupt_handler_t;

/**
 * Will initialize the interrupt management
 */
void interrupts_init();

/**
 * will hook on an interrupt
 *
 * you can use vector -1 to let it allocate an interrupt number
 *
 * @remark
 * The interrupt handler does not change the priority level of the
 * current thread but will block interrupts by default, so you can assume
 * nothing will interrupt you, you should also not enable interrupts while in
 * interrupt handler
 *
 * @remark
 * The interrupt must stay allocated as long as the interrupt
 * is registered
 *
 * @param interrupt [IN] The interrupt to register
 */
void interrupt_register(interrupt_handler_t* interrupt);

/**
 * Will free the interrupt at that vector
 *
 * This will allow to release the interrupt structure responding to
 * this interrupt
 */
void interrupt_free(interrupt_handler_t* inthandler);

#endif //__INTR_INTR_H__
