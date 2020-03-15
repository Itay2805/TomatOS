#include <arch/paging.h>
#include <arch/msr.h>
#include <arch/cpuid.h>

#include <compo/fs/vfs.h>

#include <util/def.h>
#include <mm/gdt.h>
#include <string.h>

#include "process.h"
#include "thread.h"
#include "syscall.h"
#include "sched.h"

static err_t sys_log(syscall_context_t *ctx) {
    err_t err = NO_ERROR;

    const char *str = (char *) ctx->arg1;
    CHECK_AND_RETHROW(verify_string(str));

    TRACE("syslog: pid=%d, tid=%d: %s", g_current_thread->parent->pid, g_current_thread->tid, str);

cleanup:
    return err;
}

static syscall_handler_t handlers[SYS_MAX] = {
        [SYS_LOG] = sys_log,

        // process related syscalls
        [SYS_PROC_SPAWN] = NULL,
        [SYS_PROC_KILL] = NULL,
        [SYS_PROC_SET_OPT] = NULL,

        // thread related syscalls
        [SYS_SPAWN_THREAD] = sys_spawn_thread,
        [SYS_THRD_KILL] = NULL,
        [SYS_THRD_SET_OPT] = sys_thrd_set_opt,

        // handle related stuff
        [SYS_CLOSE_HANDLE] = sys_close_handle,

        // component related opts
        [SYS_RAISE_TPL] = sys_raise_tpl,
        [SYS_RESTORE_TPL] = sys_restore_tpl,
        [SYS_CREATE_EVENT] = sys_create_event,
        [SYS_SET_TIMER] = NULL,
        [SYS_WAIT_FOR_EVENT] = sys_wait_for_event,
        [SYS_CHECK_EVENT] = sys_check_event,
        [SYS_SIGNAL_EVENT] = sys_signal_event,

        // fs mounts
        [SYS_VFS_RESOLVE] = sys_vfs_resolve,
        [SYS_VFS_MOUNT] = sys_vfs_mount,
        [SYS_VFS_UNMOUNT] = NULL,
        [SYS_VFS_MOUNTS] = NULL,

        // fs related calls
        [SYS_FS_OPEN] = sys_fs_open,
        [SYS_FS_IS_READONLY] = NULL,

        // file related calls
        [SYS_FILE_READ] = NULL,
        [SYS_FILE_SEEK] = NULL,
        [SYS_FILE_TELL] = NULL,
};

void syscall_common_handler(syscall_context_t ctx) {
    err_t err = NO_ERROR;

    // check that the syscall number is valid
    CHECK_ERROR(ctx.syscall < ARRAY_LEN(handlers), ERROR_NOT_FOUND);

    // call the handler
    CHECK_AND_RETHROW(handlers[ctx.syscall](&ctx));

cleanup:
    WARN(!IS_ERROR(err), "Got error in syscall, ignoring");
    ctx.ret_error = err;
}

err_t verify_string(const char *str) {
    err_t err = NO_ERROR;

    // verify the base pointer is good
    // worst case we fall in the non-canonical area
    CHECK_ERROR((uintptr_t) str < USERSPACE_END, ERROR_INVALID_POINTER);

cleanup:
    return err;
}

err_t verify_buffer(void *buf, size_t len) {
    err_t err = NO_ERROR;

    // verify these are good
    CHECK_ERROR((uintptr_t) buf < USERSPACE_END, ERROR_INVALID_POINTER);
    CHECK_ERROR((uintptr_t) buf + len < USERSPACE_END, ERROR_INVALID_POINTER);

cleanup:
    return err;
}

extern void syscall_entry();

void init_syscalls_for_cpu() {
    // make sure the cpu supports syscall/sysret
    CPUID_EXTENDED_CPU_SIG_EDX extended_cpu_sig_edx = {0};
    cpuid(CPUID_EXTENDED_CPU_SIG, NULL, NULL, NULL, &extended_cpu_sig_edx.raw);
    ASSERT(extended_cpu_sig_edx.SYSCALL_SYSRET);

    // enable syscall/sysret in efer
    IA32_EFER efer = __read_efer();
    efer.SCE = 1;
    __write_efer(efer);

    // setup the values for the syscall
    uint64_t usercode = GDT_USER_CODE;
    usercode &= ~0b11;
    usercode -= 16;
    usercode |= 0b11;

    // set the flags to keep
    IA32_RFLAGS rflags = {
            .always_one = 1,
            .IF = 1,
    };

    __writemsr(MSR_IA32_STAR, (usercode << 48u) | (GDT_KERNEL_CODE << 32u));
    __writemsr(MSR_IA32_LSTAR, (uint64_t) syscall_entry);
    __writemsr(MSR_IA32_FMASK, ~rflags.raw & 0xFFFFFFFF);
}
