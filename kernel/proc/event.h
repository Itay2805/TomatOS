#ifndef __TOMATOS_KERNEL_PROC_EVENT_H__
#define __TOMATOS_KERNEL_PROC_EVENT_H__

#include <util/list.h>
#include "process.h"
#include "handle.h"

typedef enum timer_type {
    /**
     * Will cancel the event timer
     */
    TIMER_CANCEL,

    /**
     * Will set a periodic timer
     */
    TIMER_PERIODIC,

    /**
     * Will set a one-shot relative timer
     */
    TIMER_RELATIVE
} timer_type_t;

/**
 * A single event
 */
typedef void* event_t;

/**
 * Will create a new event
 *
 * @param event         [OUT]           The created event
 */
err_t create_event(event_t* event);

/**
 * Will signal the event all wake all waiting
 * threads
 *
 * @param event         [IN]            The event to signal
 */
err_t signal_event(event_t event);

/**
 * Check if the event was signaled
 *
 * @param event         [IN]            The event to check
 */
err_t check_event(event_t event);

/**
 * Wait for any of the events to fire
 *
 * @param events        [IN]            The events to wait for
 * @param event_count   [IN]            The amount of events to wait for
 * @param index         [OUT]           The index of the events to wait for
 */
err_t wait_for_event(event_t* events, size_t event_count, size_t* index);

/**
 * Will close the event
 *
 * @remark
 * Internally this will simply call the release_handle_meta
 * on the event's handle_meta
 *
 * @param event         [IN] The event to close
 */
err_t close_event(event_t event);

/**
 * Sets a timer on the given event
 *
 * @remark
 * Only a single timer can be set for a given event
 *
 * @param timer         [IN] The event to set a timer for
 * @param type          [IN] The type of the timer
 * @param trigger_time  [IN] The trigger time for the timer (in microseconds)
 */
err_t set_Timer(event_t timer, timer_type_t type, uint64_t trigger_time);

#endif //__TOMATOS_KERNEL_PROC_EVENT_H__
