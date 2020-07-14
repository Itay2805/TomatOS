#ifndef __TOMATOS_KERNEL_PROC_EVENT_H__
#define __TOMATOS_KERNEL_PROC_EVENT_H__

#include <util/list.h>
#include "handle.h"

typedef struct event_data {
    /**
     * The event meta handle
     */
    handle_meta_t handle_meta;

    /**
     * The signalled flag
     */
    atomic_bool signaled;

    /**
     * The threads we need to activate
     */
    list_entry_t waiting_threads;
} event_data_t;

typedef event_data_t* event_t;

/**
 * Will create a new event
 *
 * @param event         [OUT] The created event
 */
err_t create_event(event_t* event);

/**
 * Will signal the event all wake all waiting
 * threads
 *
 * @param event         [IN] The event to signal
 */
err_t signal_event(event_t event);

/**
 * Wait for any of the events to fire
 *
 * @param events        [IN]    The events to wait for
 * @param event_count   [IN]    The amount of events to wait for
 * @param index         [OUT]   The index of the events to wait for
 */
err_t wait_for_events(event_t* events, size_t event_count, size_t* index);

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

#endif //__TOMATOS_KERNEL_PROC_EVENT_H__
