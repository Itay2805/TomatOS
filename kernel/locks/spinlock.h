#ifndef TOMATKERNEL_SPINLOCK_H
#define TOMATKERNEL_SPINLOCK_H

#include <common/stdbool.h>

typedef volatile int spinlock_t;

#ifdef __GNUC__

    static inline void spinlock_lock(volatile spinlock_t* spinlock) {
        while (__sync_val_compare_and_swap(spinlock, 0, 1));
        asm volatile("lfence" ::: "memory");
    }

    static inline void spinlock_unlock(volatile spinlock_t* spinlock) {
        *spinlock = 0;
        asm volatile("sfence" ::: "memory");
    }

#else
    #error Spinlock requires gnuc for atomic operations
#endif


#endif //TOMATKERNEL_SPINLOCK_H
