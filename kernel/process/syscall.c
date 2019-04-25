#include <memory/gdt.h>
#include <graphics/term.h>
#include <cpu/control.h>
#include <common/stdarg.h>
#include <memory/vmm.h>
#include "syscall.h"
#include "cpu/msr.h"
#include "cpu/rflags.h"

extern void syscall_handler_stub();

void syscall_handler(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f) {
    register uint64_t syscall asm("rax") = 0;

    ((void)a);
    ((void)b);
    ((void)c);
    ((void)d);
    ((void)e);
    ((void)f);

    address_space_t addr = vmm_get();
    if(addr != kernel_address_space) vmm_set(kernel_address_space);
    term_print("[syscall_handler] got syscall %x", (unsigned int) syscall);
    if(addr != kernel_address_space) vmm_set(addr);
}

void syscall_init() {
    term_write("[syscall_init] setting up the syscall/sysret msrs\n");
    wrmsr(MSR_STAR, (((uint64_t)GDT_USER_CODE - 16) << 48u) | ((uint64_t)GDT_KERNEL_CODE << 32u));
    wrmsr(MSR_LSTAR, (uint64_t) syscall_handler_stub);
    wrmsr(MSR_CSTAR, (uint64_t) syscall_handler_stub);
    wrmsr(MSR_SFMASK, FLAGS_INTERRUPT_ENABLE);

    term_write("[syscall_init] \tenabling the syscall feature\n");
    wrmsr(MSR_EFER, rdmsr(MSR_EFER) | EFER_SYSCALL);

}
