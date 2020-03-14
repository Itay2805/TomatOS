#ifndef __PROC_SYSCALL_H__
#define __PROC_SYSCALL_H__

#include <util/except.h>
#include <stdbool.h>


typedef enum syscall {
    // process related syscalls
    SYS_PROC_SPAWN = 0,
    SYS_PROC_KILL,
    SYS_PROC_SET_OPT,

    // thread related syscalls
    SYS_SPAWN_THREAD = 0x10,
    SYS_THRD_KILL,
    SYS_THRD_SET_OPT,

    // handle related stuff
    SYS_HNDL_CLOSE = 0x20,

    // event and tpl related syscalls
    SYS_RAISE_TPL = 0x30,
    SYS_RESTORE_TPL,
    SYS_CREATE_EVENT,
    SYS_SET_TIMER,
    SYS_WAIT_FOR_EVENT,
    SYS_CHECK_EVENT,
    SYS_SIGNAL_EVENT,

    // fs mounts
    SYS_VFS_RESOLVE = 0x40,
    SYS_VFS_MOUNT,
    SYS_VFS_UNMOUNT,
    SYS_VFS_MOUNTS,

    // fs related calls
    SYS_FS_OPEN = 0x50,
    SYS_FS_IS_READONLY,

    // file related calls
    SYS_FILE_READ = 0x60,
    SYS_FILE_SEEK,
    SYS_FILE_TELL,

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
err_t verify_buffer(void* buf, size_t len, bool writable);

/**
 * Initialize everything related to syscalls on this cpu
 */
void init_syscalls_for_cpu();

#endif //__PROC_SYSCALL_H__
