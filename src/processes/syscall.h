#ifndef TOMATKERNEL_SYSCALL_H
#define TOMATKERNEL_SYSCALL_H

#include <common/error.h>
#include "syscalls.h"

#define SYSCALL_AUTO ((uint64_t)-1)

/*
 * x86-64 System V syscall calling convention
 *
 * syscall num:
 * rax
 *
 * return value:
 * rax
 *
 * args:
 * rdi, rsi, rdx, r10, r8, r9
 *
 */

#define SYSRET(c) ((c)->rax)
#define SYSNUM(c) ((c)->rax)
#define SYSARG0(c) ((c)->rdi)
#define SYSARG1(c) ((c)->rsi)
#define SYSARG2(c) ((c)->rdx)
#define SYSARG3(c) ((c)->r10)
#define SYSARG4(c) ((c)->r8)
#define SYSARG5(c) ((c)->r9)

#define IS_USER_POINTER(ptr) ((uintptr_t)(ptr) < 0x0000800000000000ul)

typedef struct syscall_context {
    // the data segment to return to
    uint64_t ds;

    // general purpose registers
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    union {
        uint64_t rflags;
        uint64_t r11;
    };
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    union {
        uint64_t rip;
        uint64_t rcx;
    };
    uint64_t rbx;
    uint64_t rax;
} syscall_context_t;

typedef error_t (*syscall_handler_t)(syscall_context_t* context);

/**
 * This is the common syscall handler
 *
 * @param context
 */
void common_syscall_handler(syscall_context_t context);

/**
 * Syscall initialization
 */
error_t syscall_init();

/**
 * Init the syscalls for the current cpu
 */
error_t syscall_per_cpu_init();

/**
 * Register a new syscall
 *
 * @remark
 * if syscall_number is -1 then a syscall will be choosen dynamically
 *
 * @param handler           [IN]    The handler for the syscall
 * @param syscall_number    [OUT]   The syscall number
 */
error_t syscall_register_dynamic(syscall_handler_t handler, uint64_t* syscall_number);

error_t syscall_register(syscall_handler_t handler, uint64_t syscall_number);

#endif //TOMATKERNEL_SYSCALL_H
