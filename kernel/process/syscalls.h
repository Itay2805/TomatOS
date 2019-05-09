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
    SYSCALL_INVOKE = 10,

    // resource provider related
    SYSCALL_PROVIDER_HANDLER_FINISHED = 11,

    // memory related syscalls
    SYSCALL_MEMORY_MAP = 12,
    SYSCALL_MEMORY_UNMAP = 13,

    // process related syscalls
    SYSCALL_PROCESS_START = 14,
    SYSCALL_PROCESS_KILL = 15,
    SYSCALL_THREAD_CREATE = 16,
    SYSCALL_THREAD_KILL = 17,

    SYSCALL_COUNT = 18,
} syscalls_t;

#endif