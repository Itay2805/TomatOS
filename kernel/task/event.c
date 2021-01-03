#include <sync/lock.h>
#include <util/except.h>
#include <arch/cpu.h>
#include "event.h"

typedef struct cpu_event_info {
    /**
     * Current task priority level
     */
    tpl_t current_tpl;

    /**
     * Protects the event queues
     */
     lock_t event_queue_lock;

     /**
      * A list of events to notify for each priority level
      */
      list_entry_t event_queue[TPL_HIGH_LEVEL + 1];

      /**
       * A bitmask of the event queues that are pending
       */
      uint32_t event_pending;
} cpu_event_info_t;

static cpu_event_info_t* m_event_info = NULL;

err_t init_events(int cpus) {
    err_t err = NO_ERROR;

    // allocate the per-cpu context
    m_event_info = kalloc(cpus * sizeof(cpu_event_info_t));
    CHECK_ERROR(m_event_info != NULL, ERROR_OUT_OF_RESOURCES);

    // setup the struct
    m_event_info->current_tpl = TPL_APPLICATION;
    for (int i = 0; i < ARRAY_LEN(m_event_info->event_queue); i++) {
        list_init(&m_event_info->event_queue[i]);
    }

cleanup:
    return err;
}

static void notify_event(event_t event) {
    // if the event is queued somewhere, remove it
    if (event->notify_link.next != NULL) {
        list_remove(&event->notify_link);
        event->notify_link.next = NULL;
    }

    // queue the event to the pending notification list
    cpu_event_info_t* event_info = &m_event_info[get_cpu_id()];
    list_push(&event_info->event_queue[event->notify_tpl], &event->notify_link);
    event_info->event_pending |= (1 << event->notify_tpl);
}

err_t create_event(event_type_t type, tpl_t notify_tpl, event_notify_t notify_function, void* notify_context, event_t* event) {
    err_t err = NO_ERROR;

    CHECK(event != NULL);

    // is this a notification event
    if (type & (EVENT_NOTIFY_WAIT | EVENT_NOTIFY_SIGNAL)) {
        CHECK(notify_function != NULL);
    }

    // allocate an initialize the new event
    ievent_t* new_event = kalloc(sizeof(ievent_t));
    CHECK_ERROR(new_event != NULL, ERROR_OUT_OF_RESOURCES);
    new_event->type = type;
    new_event->notify_tpl = notify_tpl;
    new_event->notify_func = notify_function;

    // set the out event
    *event = new_event;

    // TODO: event signal queue

cleanup:
    return err;
}

err_t signal_event(event_t event) {
    err_t err = NO_ERROR;

    CHECK(event != NULL);



cleanup:
    return err;
}
