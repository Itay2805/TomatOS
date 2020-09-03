#include <arch/cpu.h>
#include <util/printf.h>
#include "critical.h"

void enter_critical(critical_t* crit) {
    crit->enable_int = are_interrupts_enabled();
}

void exit_critical(critical_t* crit) {
    if (crit->enable_int) {
        enable_interrupts();
    }
}
