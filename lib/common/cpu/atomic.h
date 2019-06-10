#ifndef TOMATKERNEL_ATOMIC_H
#define TOMATKERNEL_ATOMIC_H

#include <stdint.h>

static inline uint32_t _xchgl(void* ptr, uint32_t x) {
    asm volatile("xchgl %0, %1" : "=r"(x) : "m"(*(volatile unsigned*)ptr), "0"(x) : "memory");
    return x;
}

#define _pause()        asm volatile("pause" ::: "memory")

#define _barrier()      asm volatile("":::"memory")

#endif //TOMATKERNEL_ATOMIC_H
