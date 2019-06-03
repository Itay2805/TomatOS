#ifndef TOMATKERNEL_SYSCALLS_H
#define TOMATKERNEL_SYSCALLS_H

typedef enum syscalls {
    //////////////////////////////////////////
    // Process syscalls
    //////////////////////////////////////////

    SYSCALL_PROCESS_SPAWN,
    SYSCALL_PROCESS_KILL,

    //////////////////////////////////////////
    // Thread syscalls
    //////////////////////////////////////////

    SYSCALL_THREAD_START,
    SYSCALL_THREAD_JOIN,
    SYSCALL_THREAD_EXIT,
    SYSCALL_THREAD_KILL,
    SYSCALL_THREAD_SELF,

    //////////////////////////////////////////
    // Signal syscalls
    //////////////////////////////////////////

    SYSCALL_SIGNAL_RAISE,
    SYSCALL_SIGNAL_HANDLE,

    //////////////////////////////////////////
    // A/IO syscalls
    //////////////////////////////////////////

    SYSCALL_IO_OPEN,
    SYSCALL_IO_OPEN_AT,
    SYSCALL_IO_CLOSE,
    SYSCALL_IO_DUP,

    SYSCALL_AIO_STAT,
    SYSCALL_AIO_READDIR,
    SYSCALL_AIO_MKDIR,
    SYSCALL_AIO_READ,
    SYSCALL_AIO_WRITE,
    SYSCALL_AIO_CANCEL,
    SYSCALL_AIO_SUSPEND,
    SYSCALL_AIO_STATUS,

    //////////////////////////////////////////
    // Memory syscalls
    //////////////////////////////////////////

    SYSCALL_MEM_MAP,
    SYSCALL_MEM_PROTECT,
    SYSCALL_MEM_UNMAP,

} syscalls_t;

#endif //TOMATKERNEL_SYSCALLS_H
