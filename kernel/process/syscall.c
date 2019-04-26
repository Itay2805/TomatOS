#include <common/stdarg.h>
#include <interrupts/idt.h>
#include <graphics/term.h>
#include <cpu/control.h>
#include <memory/vmm.h>
#include <memory/gdt.h>
#include "syscall.h"
#include "cpu/msr.h"
#include "cpu/rflags.h"

extern void syscall_handler_stub();

void syscall_handler(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f) {
    register uint64_t rax asm("rax");
    uint64_t syscall = rax;

    ((void)a);
    ((void)b);
    ((void)c);
    ((void)d);
    ((void)e);
    ((void)f);

    address_space_t addr = vmm_get();
    if(addr != kernel_address_space) vmm_set(kernel_address_space);
    term_print("[syscall_handler] got syscall 0x%x\n", (unsigned int) syscall);
    if(addr != kernel_address_space) vmm_set(addr);
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
    term_write("[syscall_init] setting up int 0x80\n");
    
    idt_set_entry(0x80, syscall_handler_stub, IDT_INTERRUPT_GATE);
#endif
}
