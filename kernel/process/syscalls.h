#ifndef TOMATKERNEL_SYSCALLS
#define TOMATKERNEL_SYSCALLS

typedef enum syscalls {
    // trampoline related
    SYSCALL_PROVIDER_HANDLER_FINISHED = 0xF,

    // resource related syscalls
    SYSCALL_OPEN = 0x10,
    SYSCALL_CLOSE = 0x11,
    SYSCALL_READ = 0x12,
    SYSCALL_WRITE = 0x13,
    SYSCALL_SELECT = 0x14,
    SYSCALL_TELL = 0x15,
    SYSCALL_SEEK = 0x16,
    SYSCALL_POLL = 0x17,
    SYSCALL_WAIT = 0x18,
    SYSCALL_INVOKE = 0x19,
    SYSCALL_MKDIR = 0x1A,
    SYSCALL_READDIR = 0x1B,
    SYSCALL_OPEN_AT = 0x1C,
    SYSCALL_DUPLICATE = 0x1D,

    // memory related syscalls
    SYSCALL_MEMORY_MAP = 0x20,
    SYSCALL_MEMORY_UNMAP = 0x21,

    // process related syscalls
    SYSCALL_PROCESS_START = 0x30,
    SYSCALL_PROCESS_KILL = 0x31,
    SYSCALL_THREAD_CREATE = 0x32,
    SYSCALL_THREAD_KILL = 0x33,

    SYSCALL_COUNT,
} syscalls_t;

#endif