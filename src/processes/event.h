#ifndef TOMATKERNEL_VENT_H
#define TOMATKERNEL_VENT_H

#include <assert.h>
#include "thread.h"

typedef volatile struct event {
    spinlock_t lock;
    thread_t** waiting_threads;
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
error_t event_wait(event_t* event);

/**
 * Will resume all the threads
 *
 * @param event     [IN] The event to signal
 */
error_t event_signal(event_t* event);

#endif //TOMATKERNEL_VENT_H
