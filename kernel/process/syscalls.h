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
    
    // process related syscalls
    SYSCALL_THREAD_CREATE,
    SYSCALL_THREAD_KILL,

    SYSCALL_COUNT,
} syscalls_t;

#endif