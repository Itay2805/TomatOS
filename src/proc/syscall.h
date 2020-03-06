#ifndef __PROC_SYSCALL_H__
#define __PROC_SYSCALL_H__

#include <util/except.h>


enum syscall {
    // will allocate a syscall
    SYS_ALLOC = -1,

    // process related syscalls
    SYS_PROC_SPAWN,
    SYS_PROC_KILL,
    SYS_PROC_SET_OPT,

    // thread related syscalls
    SYS_THRD_SPAWN,
    SYS_THRD_KILL,
    SYS_THRD_SET_OPT,
    SYS_THRD_YIELD,

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
};

#endif //__PROC_SYSCALL_H__
