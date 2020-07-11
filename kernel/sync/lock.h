#ifndef __TOMATOS_KERNEL_SYNC_LOCK_H__
#define __TOMATOS_KERNEL_SYNC_LOCK_H__

#include <stdatomic.h>

typedef struct lock {
    atomic_size_t now_serving;
    atomic_size_t next_ticket;
} lock_t;

void acquire_lock(lock_t* lock);

void release_lock(lock_t* lock);

#endif //__TOMATOS_KERNEL_SYNC_LOCK_H__
