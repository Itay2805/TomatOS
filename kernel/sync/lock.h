#ifndef __TOMATOS_KERNEL_SYNC_LOCK_H__
#define __TOMATOS_KERNEL_SYNC_LOCK_H__

#include <stdatomic.h>
#include <stdbool.h>
#include "critical.h"

typedef struct lock {
    atomic_size_t now_serving;
    atomic_size_t next_ticket;
    critical_t critical;
} ticket_lock_t;

#define INIT_LOCK() ((ticket_lock_t){0})

/**
 * Will lock and wait for the ticket lock
 */
void ticket_lock(ticket_lock_t* lock);

/**
 * Will try to lock the ticket lock
 *
 * @return false if not locked, true if locked
 */
bool ticket_try_lock(ticket_lock_t* lock);

/**
 * Will unlock the ticket lock
 */
void ticket_unlock(ticket_lock_t* lock);

#endif //__TOMATOS_KERNEL_SYNC_LOCK_H__
