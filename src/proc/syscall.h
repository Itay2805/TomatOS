#ifndef __PROC_SYSCALL_H__
#define __PROC_SYSCALL_H__

#include <util/except.h>
#include <stdbool.h>


typedef enum syscall {
    // misc stuff
    SYS_LOG             = 0x00,

    // process related syscalls
    SYS_PROC_SPAWN      = 0x10,
    SYS_PROC_KILL       = 0x11,
    SYS_PROC_SET_OPT    = 0x12,

    // thread related syscalls
    SYS_SPAWN_THREAD    = 0x20,
    SYS_THRD_KILL       = 0x21,
    SYS_THRD_SET_OPT    = 0x22,

    // handle related stuff
    SYS_CLOSE_HANDLE    = 0x30,

    // event and tpl related syscalls
    SYS_RAISE_TPL       = 0x40,
    SYS_RESTORE_TPL     = 0x41,
    SYS_CREATE_EVENT    = 0x42,
    SYS_SET_TIMER       = 0x43,
    SYS_WAIT_FOR_EVENT  = 0x44,
    SYS_CHECK_EVENT     = 0x45,
    SYS_SIGNAL_EVENT    = 0x46,

    // fs mounts
    SYS_VFS_OPEN        = 0x50,
    SYS_VFS_RESOLVE     = 0x51,
    SYS_VFS_MOUNT       = 0x52,
    SYS_VFS_UNMOUNT     = 0x53,
    SYS_VFS_MOUNTS      = 0x54,

    // fs related calls
    SYS_FS_OPEN         = 0x60,
    SYS_FS_IS_READONLY  = 0x61,

    // file related calls
    SYS_FILE_READ       = 0x70,
    SYS_FILE_SEEK       = 0x71,
    SYS_FILE_TELL       = 0x72,

    SYS_MAX
} syscall_t;

typedef struct syscall_context {
    uint64_t _saved_gprs[5];

    uint64_t arg6;
    uint64_t arg5;
    uint64_t arg4;
    uint64_t arg3;
    uint64_t arg2;
    uint64_t arg1;

    uintptr_t ret_error;
    uint64_t rip;
    IA32_RFLAGS rflags;

    union {
        syscall_t syscall;
        uint64_t ret_value;
    };
} __attribute__((packed)) syscall_context_t;

typedef err_t (*syscall_handler_t)(syscall_context_t* context);

/**
 * Helper function to verify a string is a proper usermode string
 *
 * @param str   [IN] The string to check
 */
err_t verify_string(const char* str);

/**
 * Helper function to verify a buffer is a proper usermode buffer
 *
 * @param buf       [IN] The buffer
 * @param len       [IN] The length of the buffer
 * @param writable  [IN] Do we want it to be writeable
 */
err_t verify_buffer(void* buf, size_t len);

/**
 * Initialize everything related to syscalls on this cpu
 */
void init_syscalls_for_cpu();

#endif //__PROC_SYSCALL_H__
