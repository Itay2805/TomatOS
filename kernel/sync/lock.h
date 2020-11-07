#ifndef TOMATOS_LOCK_H
#define TOMATOS_LOCK_H

#include <stdatomic.h>

typedef struct lock {
    atomic_size_t now_serving;
    atomic_size_t next_ticket;
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


#endif //TOMATOS_LOCK_H
