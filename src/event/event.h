#ifndef __EVENT_EVENT_H__
#define __EVENT_EVENT_H__

/**
 * Heavily inspired from the uefi event system
 */

#include <proc/syscall.h>
#include <util/except.h>
#include <util/list.h>

#include <stdatomic.h>
#include <stdint.h>

typedef enum tpl {
    // normal runtime tpl
    TPL_APPLICATION = 4,

    // normal callback tpl
    TPL_CALLBACK = 8,

    // the highest a user callback can be at, can preempt
    TPL_USER_HIGH = 12,

    // the tpl for the preemption
    TPL_PREEMPTION = 13,

    // for use in io related callbacks, will not preempt
    TPL_IO_CALLBACK = 16,

    // for the event timer callback
    TPL_TIMER = 30,

    // highest the kernel should use, no interrupts
    TPL_HIGH_LEVEL = 31
} tpl_t;

// forward declare for the notify function
struct event_data;

// the notify function
typedef err_t (*notify_function_t)(void* ctx, struct event_data* event);

// the underlying event data
typedef struct event_data {
    // notify function info
    tpl_t notify_tpl;
    notify_function_t notify_function;
    void* notify_ctx;

    // thread waiting for the event
    struct thread* thread;

    // the signal
    atomic_bool signaled;

    // this is for the notify list
    list_entry_t notify_link;
} event_data_t;

// a simpler form to use for events
typedef event_data_t* event_t;

void events_init();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The tpl is used to manage the priority of different events
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Raises a task's priority level and returns its previous level.
 *
 * @param new_tpl   [IN] The new task priority level.
 *
 * @return Previous task priority level
 */
tpl_t raise_tpl(tpl_t new_tpl);

/**
 * Restores a task's priority level to its previous value.
 *
 * @param old_tpl   [IN] The previous task priority level to restore.
 */
void restore_tpl(tpl_t old_tpl);

/**
 * Get the current task priority level
 */
tpl_t get_tpl();

/**
 * Will set the tpl
 *
 * NOTE: DO NOT USE THIS DIRECTLY, use raise_tpl/restore_tpl
 */
void set_tpl(tpl_t tpl);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Event related stuff
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Creates an event.
 *
 * @param notify_tpl        [IN]            The task priority level of event notifications
 * @param notify_function   [IN, OPTIONAL]  Pointer to the events notification function
 * @param notify_ctx        [IN, OPTIONAL]  Pointer to the notification functions context
 * @param event             [IN]            Pointer to the newly created event if the call succeeds; NULL otherwise
 *
 * @returns ERROR_OUT_OF_RESOURCES  If failed to allocate event
 */
err_t create_event(tpl_t notify_tpl, notify_function_t notify_function, void* notify_ctx, event_t* event);

/**
 * Signals the event.  Queues the event to be notified if needed.
 *
 * @param event             [IN] The event to signal.
 */
err_t signal_event(event_t event);

/**
 * Check the status of an event.
 *
 * @param event             [IN] The event to check.
 *
 * @returns ERROR_NOT_READY if not signaled
 */
err_t check_event(event_t event);

/**
 * Stops execution until an event is signaled.
 *
 * @param number_of_events  [IN]    The number of events in the UserEvents array
 * @param events            [IN]    An array of event_t
 * @param index             [OUT]   Pointer to the index of the event which satisfied the wait condition
 */
err_t wait_for_event(size_t number_of_events, event_t* events, size_t* index);

/**
 * Closes an event and frees the event structure.
 *
 * @param event             [IN] Event to close
 */
err_t close_event(event_t event);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Timer events
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum timer_type {
    /**
     * An event's timer settings is to be cancelled
     * and not trigger time is to be set
     */
    TIMER_CANCEL,

    /**
     *  An event is to be signaled periodically at a
     *  specified interval from the current time.
     */
    TIMER_PERIODIC,

    /**
     * An event is to be signaled once at a
     * specified interval from the current time.
     */
    TIMER_RELATIVE
} timer_type_t;

#define MS_TO_100NS(x) (x * 10000)

/**
 * Sets the type of timer and the trigger time for a timer event.
 *
 * @param event         [IN] The timer event that is to be signaled at the specified time.
 * @param type          [IN] The type of time that is specified in TriggerTime.
 * @param trigger_time  [IN] The number of 100ns until timer expires
 */
err_t set_timer(event_t event, timer_type_t type, uint64_t trigger_time);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Syscalls
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

err_t sys_raise_tpl(syscall_context_t* context);
err_t sys_restore_tpl(syscall_context_t* context);
err_t sys_create_event(syscall_context_t* context);
err_t sys_signal_event(syscall_context_t* context);
err_t sys_check_event(syscall_context_t* context);
err_t sys_wait_for_event(syscall_context_t* context);

#endif //__EVENT_EVENT_H__
