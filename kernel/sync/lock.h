#ifndef __TOMATOS_KERNEL_SYNC_LOCK_H__
#define __TOMATOS_KERNEL_SYNC_LOCK_H__

#include <stdatomic.h>

typedef struct lock {
    atomic_size_t now_serving;
    atomic_size_t next_ticket;
} ticket_lock_t;

#define INIT_LOCK() ((ticket_lock_t){0})

void ticket_lock(ticket_lock_t* lock);
void ticket_unlock(ticket_lock_t* lock);

#endif //__TOMATOS_KERNEL_SYNC_LOCK_H__
