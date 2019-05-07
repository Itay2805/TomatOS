#ifndef TOMATKERNEL_SYSCALL_H
#define TOMATKERNEL_SYSCALL_H

#include <common/stdint.h>
#include "syscalls.h"

#define SYSCALL_INSTRUCTION "int $0x80"

////////////////////////////////////////////////////////////////////////////
// Syscalls
////////////////////////////////////////////////////////////////////////////
//
// Will handle syscalls in the os, basically just sets up syscall/sysret
// and uses that.
//
// also contains helper functions for doing syscalls
//
////////////////////////////////////////////////////////////////////////////

typedef error_t(*syscall_handler_f)(registers_t* regs);

extern syscall_handler_f syscalls[SYSCALL_COUNT];

void syscall_init();

#endif //TOMATKERNEL_SYSCALL_H
