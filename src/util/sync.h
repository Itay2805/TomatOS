#ifndef TOMATKERNEL_LOCK_H
#define TOMATKERNEL_LOCK_H

#include <libc/stdbool.h>
#include "list.h"

typedef volatile uint32_t lock_t;

void acquire_lock(lock_t *lock);
bool acquire_lock_or_fail(lock_t *lock);
void release_lock(lock_t* lock);

uint32_t interlocked_increment(volatile uint32_t* value);
uint32_t interlocked_decrement(volatile uint32_t* value);
uint32_t interlocked_compare_exchange(volatile uint32_t* value, uint32_t comapre, uint32_t exchange);

typedef struct event {
    lock_t lock;
    list_entry_t threads;
} event_t;

/**
 * Will wait for an event, this will suspend the thread until the haven't has fired.
 *
 * @remark
 * This MUST run from a thread context
 *
 * @param event     [IN] The event to wait for
 * @param thread    [IN] The thread to run
 */
void event_wait(event_t* event);

/**
 * Will resume all the threads
 *
 * @param event     [IN] The event to signal
 */
void event_signal(event_t* event);

#endif //TOMATKERNEL_LOCK_H
