#ifndef TOMATKERNEL_SYSCALLS
#define TOMATKERNEL_SYSCALLS

typedef enum syscalls {
    // resource related syscalls
    SYSCALL_OPEN = 1,
    SYSCALL_CLOSE = 2,
    SYSCALL_READ = 3,
    SYSCALL_WRITE = 4,
    SYSCALL_SELECT = 5,
    SYSCALL_TELL = 6,
    SYSCALL_SEEK = 7,
    SYSCALL_POLL = 8,
    SYSCALL_WAIT = 9,

    // resource provider related
    SYSCALL_PROVIDER_HANDLER_FINISHED = 10,

    // memory related syscalls
    SYSCALL_MEMORY_MAP,
    SYSCALL_MEMORY_UNMAP,

    // process related syscalls
    SYSCALL_PROCESS_START,
    SYSCALL_PROCESS_KILL,
    SYSCALL_THREAD_CREATE,
    SYSCALL_THREAD_KILL,

    SYSCALL_COUNT,
} syscalls_t;

#endif