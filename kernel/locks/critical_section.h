#ifndef TOMATKERNEL_CRITICAL_SECTION_H
#define TOMATKERNEL_CRITICAL_SECTION_H

#include <common/stdint.h>

typedef uint64_t critical_section_t;

static inline critical_section_t critical_section_start() {
    uint64_t flags;
    asm volatile ("pushf\n\tcli\n\tpop %0" : "=r"(flags) : : "memory");
    return flags;
}

static inline void critical_section_end(critical_section_t flags) {
    asm ("push %0\n\tpopf" : : "rm"(flags) : "memory","cc");
}

#endif //TOMATKERNEL_CRITICAL_SECTION_H
