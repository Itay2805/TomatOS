#ifndef TOMATKERNEL_RWLOCK_H
#define TOMATKERNEL_RWLOCK_H

#include "spinlock.h"

typedef struct {
    spinlock_t lock;
    volatile bool writing;
    volatile int read_count;
    volatile int write_wait;
} rwlock_t;

static void rwlock_read_lock(rwlock_t* rwlock) {

    while(true) {
        spinlock_lock(&rwlock->lock);
        // are we on writing?
        if(rwlock->writing) {
            //  if so we can not lock
        }else {
            // if someone wants to 
            // write lets not lock
            if(rwlock->write_wait == 0) {
                rwlock->read_count++;
            } 
        }
        spinlock_unlock(&rwlock->lock);
    }
}

static void rwlock_read_unlock(rwlock_t* rwlock) {
    spinlock_lock(&rwlock->lock);
    rwlock->read_count--;
    spinlock_unlock(&rwlock->lock);
}

static void rwlock_write_lock(rwlock_t* rwlock) {
    spinlock_lock(&rwlock->lock);
    rwlock->write_wait++;
    spinlock_unlock(&rwlock->lock);
    
    while(true) {
        spinlock_lock(&rwlock->lock);
        // are we on writing?
        if(rwlock->writing) {
            //  if so we can not lock
        }else {
            rwlock->write_wait--;
            rwlock->writing = true;
        }
        spinlock_unlock(&rwlock->lock);
    }
}

static void rwlock_write_unlock(rwlock_t* rwlock) {
    spinlock_lock(&rwlock->lock);
    rwlock->writing = false;
    spinlock_unlock(&rwlock->lock);
}

#endif