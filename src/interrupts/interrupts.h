#ifndef TOMATKERNEL_INTERRUPTS_H
#define TOMATKERNEL_INTERRUPTS_H

#include <stdint.h>
#include <util/list.h>

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
    IA32_RFLAGS rflags;

    // the stack pointer and segment
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed)) interrupt_context_t;

enum {
    // This is where the legacy interrupts start
    INT_LEGACY_BASE = 0x20,

    // IPIs
    IPI__TLB_SHOOTDOWN = 0x20 + 0xf,
    IPI_SCHEDULER_STARTUP,
    IPI_SCHEDULER_RESCHEDULE,

    // from here we can allocate it dynamically
    INT_ALLOCATION_BASE,
};

typedef struct interrupt {
    int vector;
    const char* name;
    void* user_param;
    error_t (*callback)(interrupt_context_t* ctx, void* user_param);
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

#endif //TOMATKERNEL_INTERRUPTS_H
