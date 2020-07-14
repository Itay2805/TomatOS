#include <arch/cpu.h>
#include "critical.h"

void enter_critical(critical_t* crit) {
    crit->enable_int = are_interrupts_enabled();
    disable_interrupts();
}

void leave_critical(critical_t* crit) {
    if (crit->enable_int) {
        enable_interrupts();
    }
}
