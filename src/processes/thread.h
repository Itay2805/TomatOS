#ifndef TOMATKERNEL_THREAD_H
#define TOMATKERNEL_THREAD_H

#include <objects/objects.h>
#include "process.h"

typedef struct cpu_state {
    IA32_RFLAGS rflags;
    uint64_t gs_base;
    uint64_t rip;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
} cpu_state_t;

typedef struct thread {
    object_t object;

    // the parent of this thread
    process_t* parent;

    // Thread lock
    // TODO: Maybe switch to a read write lock
    lock_t lock;

    // Links to other threads in the same process
    list_entry_t scheduler_link;

    // the state of the thread
    __attribute__((aligned(16)))
    struct {
        cpu_state_t cpu;
    } state;

    // the allocated stack
    uintptr_t stack;
    size_t stack_size;

    // the stack used for the kernel stuff
    uintptr_t kernel_stack;
    size_t kernel_stack_size;
} thread_t;

/**
 * Initialize a thread to a default state and attach it to a process
 *
 * @remark
 * This will allocate a stack
 *
 * @param thread    [IN/OUT]    The thread to initialize
 * @param parent    [IN]        The parent of the process
 */
void thread_init(thread_t* thread, process_t* parent);

#endif //TOMATKERNEL_THREAD_H
