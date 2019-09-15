#ifndef TOMATKERNEL_SYSCALL_H
#define TOMATKERNEL_SYSCALL_H

#include <common/error.h>

#define SYSCALL_AUTO ((uint64_t)-1)

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

typedef enum syscalls {
    // memory related
    SYSCALL_MMAP                = 0x10,
    SYSCALL_MUNMAP              = 0x11,
    SYSCALL_MPROTECT            = 0x12,

    // process related
    SYSCALL_PROCESS_CREATE      = 0x20,
    SYSCALL_PROCESS_START       = 0x21,
    SYSCALL_PROCESS_SET_OPT     = 0x22,
    SYSCALL_PROCESS_KILL        = 0x23,

    // thread related
    SYSCALL_THREAD_CREATE       = 0x30,
    SYSCALL_THREAD_START        = 0x31,
    SYSCALL_THREAD_SET_OPT      = 0x32,
    SYSCALL_THREAD_JOIN         = 0x33,
    SYSCALL_THREAD_KILL         = 0x34,

    // socket related
    SYSCALL_SOCKET_CREATE       = 0x40,
    SYSCALL_SOCKET_BIND         = 0x41,
    SYSCALL_SOCKET_AUTOBIND     = 0x42,
    SYSCALL_SOCKET_ACCEPT       = 0x43,
    SYSCALL_SOCKET_CONNET       = 0x44,
    SYSCALL_SOCKET_RECV         = 0x45,
    SYSCALL_SOCKET_RECV_FROM    = 0x46,
    SYSCALL_SOCKET_SEND         = 0x47,
    SYSCALL_SOCKET_SEND_TO      = 0x48,

    // object related
    SYSCALL_OBJECT_GET_BY_ID    = 0x50,
    SYSCALL_OBJECT_GET_PRIMARY  = 0x51,
    SYSCALL_OBJECT_SET_PRIMARY  = 0x52,
    SYSCALL_OBJECT_GET_NEXT     = 0x53,

    // dynamic allocations starts from here
    SYSCALL_DYNAMIC_START = 0x60,
} syscalls_t;

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
error_t syscall_register(syscall_handler_t handler, uint64_t* syscall_number);

#endif //TOMATKERNEL_SYSCALL_H
