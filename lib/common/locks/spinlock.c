#include <cpu/atomic.h>
#include <interrupts/interrupts.h>
#include "spinlock.h"
#include "preemption.h"

void lock(spinlock_t* spinlock) {
    while(true) {
        if(!_xchgl(spinlock, 1)) {
            return;
        }
        while(spinlock->locked) _pause();
    }
}

void unlock(spinlock_t* spinlock) {
    _barrier();
    spinlock->locked = 0;
}

bool try_lock(spinlock_t* spinlock) {
    return (bool) _xchgl(spinlock, 1);
}

void lock_preemption(spinlock_t *spinlock1) {
    spinlock1->interrupts = prempsave();
    try_lock(spinlock1);
}

void unlock_preemption(spinlock_t *spinlock) {
    unlock(spinlock);
    prempstore(spinlock->interrupts);
}