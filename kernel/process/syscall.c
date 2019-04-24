#include <memory/gdt.h>
#include <graphics/term.h>
#include "syscall.h"
#include "cpu/msr.h"
#include "cpu/rflags.h"

static void syscall_handler_stub() {}

void syscall_handler() {
}

void syscall_init() {
    term_write("[syscall_init] setting up the syscall/sysret msrs\n");
    wrmsr(MSR_STAR, (((uint64_t)GDT_USER_CODE - 16) << 48u) | ((uint64_t)GDT_KERNEL_CODE << 32u));
    wrmsr(MSR_LSTAR, (uint64_t) syscall_handler_stub);
    wrmsr(MSR_CSTAR, (uint64_t) syscall_handler_stub);
    wrmsr(MSR_SFMASK, FLAGS_INTERRUPT_ENABLE);

    term_write("[syscall_init] enabling the syscall feature\n");

}
