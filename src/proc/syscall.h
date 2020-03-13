#ifndef __PROC_SYSCALL_H__
#define __PROC_SYSCALL_H__

#include <util/except.h>
#include <stdbool.h>


typedef enum syscall {
    // process related syscalls
    SYS_PROC_SPAWN,
    SYS_PROC_KILL,
    SYS_PROC_SET_OPT,

    // thread related syscalls
    SYS_THRD_SPAWN,
    SYS_THRD_KILL,
    SYS_THRD_SET_OPT,
    SYS_THRD_RAISE_TPL,
    SYS_THRD_RESTORE_TPL,

    // component related opts
    SYS_COMP_GET,
    SYS_COMP_GET_NEXT,
    SYS_COMP_GET_PRIMARY,
    SYS_COMP_SET_PRIMARY,

    // fs mounts
    SYS_VFS_RESOLVE,
    SYS_VFS_MOUNT,
    SYS_VFS_UNMOUNT,
    SYS_VFS_MOUNTS,

    // fs related calls
    SYS_FS_OPEN,
    SYS_FS_IS_READONLY,

    // file related calls
    SYS_FILE_READ,
    SYS_FILE_SEEK,
    SYS_FILE_TELL,
    SYS_FILE_CLOSE,

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
