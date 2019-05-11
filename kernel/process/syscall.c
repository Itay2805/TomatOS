#include <common/stdarg.h>

#include <interrupts/idt.h>

#include <graphics/term.h>

#include <process/process.h>

#include <memory/vmm.h>
#include <memory/gdt.h>

#include <cpu/control.h>
#include <cpu/rflags.h>
#include <cpu/msr.h>
#include <common/logging.h>

#include "scheduler.h"
#include "syscall.h"

syscall_handler_f syscalls[SYSCALL_COUNT];

extern void syscall_handler_stub();

void syscall_handler(registers_t regs) {
    error_t err = NO_ERROR;
    int syscall = (int)regs.rax;
    address_space_t addrspace;

    if(vmm_get() != kernel_address_space) {
        vmm_set(kernel_address_space);
    }

    if(syscall >= 0 && syscall < SYSCALL_COUNT && syscalls[syscall] != 0) {
        CHECK_AND_RETHROW(syscalls[syscall](&regs));
    }else {
        regs.rax = false;
        CHECK_FAIL_ERROR(ERROR_INVALID_SYSCALL);
    }

cleanup:
    // on error print the stack trace
    // and free the error
    addrspace = vmm_get();
    if(addrspace != kernel_address_space) vmm_set(kernel_address_space);
    if(IS_ERROR(err)) {
        KERNEL_STACK_TRACE(err);
        ERROR_FREE(err);
    }
    if(addrspace != kernel_address_space) vmm_set(addrspace);

    if(vmm_get() == kernel_address_space && running_thread->parent->address_space != kernel_address_space) {
        vmm_set(running_thread->parent->address_space);
    }
}

void syscall_init() {
#if 0
    term_write("[syscall_init] setting up the syscall/sysret msrs\n");
    
    uint64_t usercode = GDT_USER_CODE;
    usercode &= ~0b11;
    usercode -= 16;
    usercode |= 0b11;
    wrmsr(MSR_STAR, (usercode << 48u) | ((uint64_t)GDT_KERNEL_CODE << 32u));
    wrmsr(MSR_LSTAR, (uint64_t) syscall_handler_stub);
    wrmsr(MSR_SFMASK, ~(1 << 2u));

    term_write("[syscall_init] \tenabling the syscall feature\n");
    wrmsr(MSR_EFER, rdmsr(MSR_EFER) | EFER_SYSCALL);
#else
    LOG_WARN("syscall/sysret is currently disabled, using int 0x80");
    LOG_INFO("setting up int 0x80");

    idt_set_entry(0x80, syscall_handler_stub, IDT_INTERRUPT_GATE);
#endif
}
