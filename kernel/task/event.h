#ifndef __TOMATOS_EVENT_H__
#define __TOMATOS_EVENT_H__

#include <evdns.h>
#include "task.h"

/**
 * This struct is a bit field where each bit represents a that there is
 * an event waiting at the given TPL
 */
extern uint32_t g_events_pending;

/**
 * Dispatch all the events related to this specific TPL
 *
 * @param tpl       [IN] The target TPL
 */
void disaptch_event_notifies(tpl_t tpl);

#endif //__TOMATOS_EVENT_H__
