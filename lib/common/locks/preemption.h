#ifndef TOMATKERNEL_PREEMPTION_H
#define TOMATKERNEL_PREEMPTION_H

#include <stdint.h>

static inline uint64_t prempsave(void) {
    unsigned long flags;
    asm volatile ("pushf\n\tcli\n\tpop %0" : "=r"(flags) : : "memory");
    return flags;
}

static inline void prempstore(uint64_t flags) {
    asm ("push %0\n\tpopf" : : "rm"(flags) : "memory","cc");
}

#endif //TOMATKERNEL_PREEMPTION_H
