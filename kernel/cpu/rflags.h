//
// Created by Itay on 04/04/2019.
//

#ifndef TOMATKERNEL_RFLAGS_H
#define TOMATKERNEL_RFLAGS_H

////////////////////////////////////////////////////////////////////////////
// RFLAGS
////////////////////////////////////////////////////////////////////////////
//
// This contains the different bits of the rflags, just for easier use
//
////////////////////////////////////////////////////////////////////////////

#define FLAGS_CARRY     (1 << 0)
#define FLAGS_PARITY    (1 << 2)
#define FLAGS_ADJUST    (1 << 4)
#define FLAGS_ZERO (1 << 6)
#define FLAGS_SIGN (1 << 7)
#define FLAGS_TRAP (1 << 8)
#define FLAGS_INTERRUPT_ENABLE (1 << 9)
#define FLAGS_DIRECTION (1 << 10)
#define FLAGS_OVERFLOW (1 << 11)
#define FLAGS_IOPL_0 (0)
#define FLAGS_IOPL_1 (1 << 12)
#define FLAGS_IOPL_2 (1 << 13)
#define FLAGS_IOPL_3 ((1 << 12) | (1 << 13))
#define FLAGS_NESTED_TASK (1 << 14)

#define EFLAGS_RESUME (1 << 16)
#define EFLAGS_V8086 (1 << 17)
#define EFLAGS_ALIGNMENT_CHECK (1 << 18)
#define EFLAGS_VIRTUAL_INTERRUPT (1 << 19)
#define EFLAGS_VIRTUAL_INTERRUPT_PENDING (1 << 20)
#define EFLAGS_CPUID (1 << 21)

#define RFLAGS_DEFAULT (FLAGS_INTERRUPT_ENABLE | EFLAGS_CPUID | (1 << 1))
#define RFLAGS_DEFAULT_USER RFLAGS_DEFAULT

#endif //TOMATKERNEL_RFLAGS_H
