#ifndef TOMATKERNEL_CRITICAL_SECTION_H
#define TOMATKERNEL_CRITICAL_SECTION_H

#include <common/stdint.h>

typedef uint64_t critical_section_t;

#define CRITICAL_SECTION(...) \
    do { \
        uint64_t critical_section = critical_section_start(); \
        __VA_ARGS__; \
        critical_section_end(critical_section); \
    } while(0)

#define CRITICAL_SECTION_LABEL(label, ...) \
    do { \
        uint64_t critical_section = critical_section_start(); \
        __VA_ARGS__; \
        label: \
        critical_section_end(critical_section); \
        CHECK_AND_RETHROW(err); \
    } while(0)

static inline critical_section_t critical_section_start() {
    uint64_t flags;
    asm volatile ("pushf\n\tcli\n\tpop %0" : "=r"(flags) : : "memory");
    return flags;
}

static inline void critical_section_end(critical_section_t flags) {
    asm ("push %0\n\tpopf" : : "rm"(flags) : "memory","cc");
}

#endif //TOMATKERNEL_CRITICAL_SECTION_H
