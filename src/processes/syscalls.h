#ifndef TOMATKERNEL_SYSCALLS_H
#define TOMATKERNEL_SYSCALLS_H

//typedef enum process_opt {
//} process_opt_t;

typedef enum thread_opt {
    /**
     * will reallocate the stack to make it bigger
     *
     * @remark
     * The stack might be in a different address!!! so make sure you
     * are not referencing anything which is on the stack
     */
    THREAD_OPT_STACK_SIZE,

    /**
     * Set the FS base of the thread
     */
    THREAD_OPT_FS_BASE,
} thread_opt_t;

typedef enum syscalls {
    // memory related
    SYSCALL_MMAP                        = 0x10,
    SYSCALL_MUNMAP                      = 0x11,
    SYSCALL_MPROTECT                    = 0x12,

    // process related
    SYSCALL_PROCESS_CREATE              = 0x20,
    SYSCALL_PROCESS_START               = 0x21,
    SYSCALL_PROCESS_SET_OPT             = 0x22,
    SYSCALL_PROCESS_KILL                = 0x23,
    SYSCALL_PROCESS_FORK                = 0x24,

    // thread related
    SYSCALL_THREAD_CREATE               = 0x30,
    SYSCALL_THREAD_START                = 0x31,
    SYSCALL_THREAD_SET_OPT              = 0x32,
    SYSCALL_THREAD_JOIN                 = 0x33,
    SYSCALL_THREAD_KILL                 = 0x34,

    // socket related
    SYSCALL_SOCKET_CREATE               = 0x40,
    SYSCALL_SOCKET_BIND                 = 0x41,
    SYSCALL_SOCKET_AUTOBIND             = 0x42,
    SYSCALL_SOCKET_ACCEPT               = 0x43,
    SYSCALL_SOCKET_CONNET               = 0x44,
    SYSCALL_SOCKET_RECV                 = 0x45,
    SYSCALL_SOCKET_RECV_FROM            = 0x46,
    SYSCALL_SOCKET_SEND                 = 0x47,
    SYSCALL_SOCKET_SEND_TO              = 0x48,

    // object related
    SYSCALL_OBJECT_GET_BY_ID            = 0x50,
    SYSCALL_OBJECT_GET_PRIMARY          = 0x51,
    SYSCALL_OBJECT_SET_PRIMARY          = 0x52,
    SYSCALL_OBJECT_GET_NEXT             = 0x53,

    // window manager related
    SYSCALL_WINDOW_CREATE               = 0x60,
    SYSCALL_WINDOW_DELETE               = 0x61,
    SYSCALL_WINDOW_FRAMEBUFFFER         = 0x62,
    SYSCALL_WINDOW_EVENT_PULL           = 0x63,
    SYSCALL_WINDOW_EVENT_HANDLER        = 0x64,
    SYSCALL_WINDOW_EVENT_WAIT           = 0x65,

    // dynamic allocations starts from here
    SYSCALL_DYNAMIC_START               = 0x70,
} syscalls_t;



#endif //TOMATKERNEL_SYSCALLS_H
