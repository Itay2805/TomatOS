#ifndef TOMATKERNEL_SYSCALLS
#define TOMATKERNEL_SYSCALLS

typedef enum syscalls {
    // resource related syscalls
    SYSCALL_OPEN,
    SYSCALL_CLOSE,
    SYSCALL_READ,
    SYSCALL_WRITE,
    SYSCALL_SELECT,
    SYSCALL_POLL,
    SYSCALL_WAIT,
    
    // process related syscalls
    SYSCALL_THREAD_CREATE,
    SYSCALL_THREAD_KILL,

    // resource provider related
    SYSCALL_PROVIDER_HANDLER_FINISHED,

    SYSCALL_COUNT,
} syscalls_t;

#endif