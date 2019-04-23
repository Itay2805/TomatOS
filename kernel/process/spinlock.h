//
// Created by Itay on 23/04/2019.
//

#ifndef TOMATKERNEL_SPINLOCK_H
#define TOMATKERNEL_SPINLOCK_H

#include <common/stdbool.h>

typedef struct spinlock {
    int locked;
} spinlock_t;

#ifdef __GNUC__
    static inline void spinlock_lock(spinlock_t* spinlock) {
        int cmp = 0;
        int xchng = 1;
        while(!__atomic_compare_exchange(&spinlock->locked, &cmp, &xchng, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));
    }

    static inline void spinlock_unlock(spinlock_t* spinlock) {
        __atomic_store_n(&spinlock->locked, 0, __ATOMIC_SEQ_CST);
    }
#else
    #error Spinlock requires gnuc for atomic operations
#endif


#endif //TOMATKERNEL_SPINLOCK_H
