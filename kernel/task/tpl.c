#include <stdbool.h>
#include <arch/cpu.h>
#include <util/except.h>

#include "task.h"
#include "event.h"

static void set_interrupt_state(bool enable) {
    if (!enable) {
        disable_interrupts();
    } else {
        enable_interrupts();
    }
}

static tpl_t CPU_LOCAL m_current_tpl;

tpl_t raise_tpl(tpl_t new_tpl) {
    // get the current value
    tpl_t old_tpl = m_current_tpl;

    // warn on an invalid tpl
    if (old_tpl > new_tpl) {
        UNLOCKED_WARN("raised to a lower tpl than the current one (old=%t new=%t)", old_tpl, new_tpl);
    }

    // disable interrupts if in high level
    if (new_tpl >= TPL_HIGH_LEVEL && old_tpl < TPL_HIGH_LEVEL) {
        set_interrupt_state(false);
    }

    // set the new value
    m_current_tpl = new_tpl;

    return old_tpl;
}

void restore_tpl(tpl_t new_tpl) {
    tpl_t old_tpl = m_current_tpl;

    // warn on an invalid tpl
    if (new_tpl > old_tpl) {
        UNLOCKED_WARN("\"restored to a higher tpl than the current one (old=%t new=%t)", old_tpl, new_tpl);
    }

    while (g_events_pending != 0) {
        tpl_t pending_tpl = __builtin_clz(g_events_pending);
        if (pending_tpl <= new_tpl) {
            break;
        }

        m_current_tpl = pending_tpl;
        if (m_current_tpl < TPL_HIGH_LEVEL) {
            set_interrupt_state(true);
        }
        disaptch_event_notifies(m_current_tpl);
    }

    m_current_tpl = new_tpl;

    if (m_current_tpl < TPL_HIGH_LEVEL) {
        set_interrupt_state(true);
    }
}
