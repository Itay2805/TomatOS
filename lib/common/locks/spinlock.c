#include <cpu/atomic.h>
#include <interrupts/interrupts.h>
#include "spinlock.h"
#include "preemption.h"

void do_lock(spinlock_t* spinlock, char* file, int line) {
    uint64_t start = 0xFFFFFF;
    while(true) {
        if(do_try_lock(spinlock, file, line)) {
            return;
        }
        while(spinlock->locked) {
            if(!(start--)) {
                log_warn("Probably have a deadlock at %s:%d", file, line);
            }
            _pause();
        }
    }
}

void do_unlock(spinlock_t* spinlock, char* file, int line) {
    _barrier();
    spinlock->locked = 0;
    spinlock->filename = file;
    spinlock->line = line;
}

bool do_try_lock(spinlock_t* spinlock, char* file, int line) {
    bool success = (bool) !_xchgl((void *) &spinlock->locked, 1);
    if(success) {
        spinlock->filename = file;
        spinlock->line = line;
    }
    return success;
}

void do_lock_preemption(spinlock_t *spinlock1, char* file, int line) {
    spinlock1->interrupts = prempsave();
    do_lock(spinlock1, file, line);
}

void do_unlock_preemption(spinlock_t *spinlock, char* file, int line) {
    if(spinlock->locked) {
        do_unlock(spinlock, file, line);
        prempstore(spinlock->interrupts);
    }else {
        log_warn("Tried to unlock and unlocked lock (%s:%d)", file, line);
    }
}