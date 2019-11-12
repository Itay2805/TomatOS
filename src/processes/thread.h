#ifndef TOMATKERNEL_THREAD_H
#define TOMATKERNEL_THREAD_H

#include <objects/object.h>
#include <util/sync.h>
#include <util/arch.h>
#include <util/list.h>

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

typedef enum thread_state {
    // this is a running thread
    THREAD_RUNNING,

    // this is a thread that is waiting for something to
    // happen
    THREAD_WAITING,

    // this is a thread that is waiting to run
    THREAD_NORMAL,
} thread_state_t;

typedef struct thread {
    object_t _;

    struct process* parent;
    lock_t lock;

    thread_state_t state;
    cpu_state_t cpu_state;

    uintptr_t stack;
    size_t stack_size;

    uintptr_t kernel_stack;
    size_t kernel_stack_size;

    list_entry_t scheduler_link;
} thread_t;

const void* Thread();

#endif //TOMATKERNEL_THREAD_H
