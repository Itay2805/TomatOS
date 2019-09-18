#include <stb/stb_ds.h>
#include <common/cpu/msr.h>
#include <memory/gdt.h>
#include "syscall.h"

/**
 * This is the syscall handler
 */
extern void handle_syscall_request();

syscall_handler_t* syscalls = NULL;

void common_syscall_handler(syscall_context_t context) {
    error_t err = NO_ERROR;

    CHECK(context.rax < arrlen(syscalls));
    CHECK_AND_RETHROW(syscalls[context.rax](&context));

cleanup:
    CATCH(err);
    context.rax = err;
}

error_t syscall_init() {
    arrsetlen(syscalls, SYSCALL_DYNAMIC_START);
    return NO_ERROR;
}

/**
 * Init the syscalls for the current cpu
 */
error_t syscall_per_cpu_init() {
    error_t err = NO_ERROR;

    /*
     * set all the syscall stuff
     */
    star_t star = {
        .syscall_cs_ss = (uint16_t) GDT_KERNEL_CODE,
        .sysret_cs_ss = (uint16_t) (GDT_USER_DATA - 8),
    };
    _wrmsr(IA32_STAR, star.raw);
    _wrmsr(IA32_LSTAR, (uintptr_t)handle_syscall_request);

//cleanup:
    return err;
}

error_t syscall_register_dynamic(syscall_handler_t handler, uint64_t* syscall_number) {
    error_t err = NO_ERROR;

    CHECK(handler != NULL);
    CHECK(syscall_number != NULL);

    if(*syscall_number == SYSCALL_AUTO) {
        *syscall_number = arrlen(syscalls);
        arrpush(syscalls, handler);
    }else {
        CHECK_TRACE(syscalls[*syscall_number] == 0, "Tried to allocate the same syscall twice");
        syscalls[*syscall_number] = handler;
    }

cleanup:
    return err;
}
error_t syscall_register(syscall_handler_t handler, uint64_t syscall_number) {
    error_t err = NO_ERROR;

    CHECK_AND_RETHROW(syscall_register_dynamic(handler, &syscall_number));

cleanup:
    return err;
}


