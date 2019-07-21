#ifndef TOMATKERNEL_EVENT_H
#define TOMATKERNEL_EVENT_H

#include "spinlock.h"

typedef struct event {
    spinlock_t lock;
    volatile bool signaled;
} event_t;

/**
 * Will signal the given event
 *
 * @remark
 * You should only wait for event where preemption is possible, or across CPUs
 *
 * @param event [IN] The event to signal
 */
void event_signal(event_t* event);

/**
 * Will wait for an event
 *
 * @remark
 * You should only wait for event where preemption is possible, or across CPUs
 *
 * @param event     [IN] The event to wait for
 * @param timeout   [IN] Timeout in millis, 0 for no timeout
 *
 * @return
 * True if signaled, false if timeout
 */
bool event_wait(event_t* event, uint64_t timeout);

#endif //TOMATKERNEL_EVENT_H
