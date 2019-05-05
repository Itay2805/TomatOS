#ifndef TOMATKERNEL_SYSCALLS
#define TOMATKERNEL_SYSCALLS

typedef enum syscalls {
    // resource related syscalls
    SYSCALL_OPEN = 1,
    SYSCALL_CLOSE = 2,
    SYSCALL_READ = 3,
    SYSCALL_WRITE = 4,
    SYSCALL_SELECT = 5,
    SYSCALL_POLL = 6,
    SYSCALL_WAIT = 7,

    // resource provider related
    SYSCALL_PROVIDER_HANDLER_FINISHED = 8,
    
    // process related syscalls
    SYSCALL_THREAD_CREATE,
    SYSCALL_THREAD_KILL,

    SYSCALL_COUNT,
} syscalls_t;

#endif