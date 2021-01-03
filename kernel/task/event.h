#ifndef TOMATOS_EVENT_H
#define TOMATOS_EVENT_H

#include "sched.h"

typedef enum event_type {
    /**
     *
     */
    EVENT               = 0x00000000,

    /**
     * TODO: this
     */
    EVENT_TIMER         = 0x80000000,

    /**
     * TODO: this
     */
    EVENT_NOTIFY_WAIT   = 0x00000100,

    /**
     * TODO: this
     */
    EVENT_NOTIFY_SIGNAL = 0x00000200,
} event_type_t;

typedef struct ievent* event_t;

/**
 * TODO: this
 */
typedef err_t (*event_notify_t)(event_t event, void* ctx);

/**
 * Timer event information
 */
typedef struct timer_event_info {
    list_entry_t link;
    uint64_t trigger_time;
    uint64_t period;
} timer_event_info_t;

/**
 * The event information
 */
typedef struct ievent {
    //------------------------------------
    // Event metadata
    //------------------------------------
    event_type_t type;
    uint32_t signal_count;
    list_entry_t signal_link;

    //------------------------------------
    // Related to notification
    //------------------------------------
    tpl_t notify_tpl;
    event_notify_t notify_func;
    void* notify_context;
    list_entry_t notify_link;

    //------------------------------------
    // The timer event
    //------------------------------------
    timer_event_info_t timer;
} ievent_t;

/**
 * Create a new event
 *
 * @param type              [IN]            The type of the event to create
 * @param notify_tpl        [IN]            The priority of notification of the event
 * @param notify_function   [IN,OPTIONAL]   The notification function to call
 * @param notify_context    [IN,OPTIONAL]   A parameter to the notification function
 * @param event             [OUT]           The new event
 */
err_t create_event(event_type_t type, tpl_t notify_tpl, event_notify_t notify_function, void* notify_context, event_t* event);

/**
 * Signal the event
 *
 * @param event             [IN] The event to signal
 */
err_t signal_event(event_t event);

#endif //TOMATOS_EVENT_H
