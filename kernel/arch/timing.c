
#include <stdint.h>
#include <util/except.h>
#include <acpi/acpi.h>
#include "timing.h"

timer_implementation_t g_timer;

void ustall(uint64_t nanoseconds) {
    switch (g_timer) {
        case TIMER_ACPI: {
            acpi_ustall(nanoseconds);
        } break;

#ifdef __TOMATOS_AMD64__
        case TIMER_HPET: {

        } break;
#endif

        default: ASSERT_TRACE(0, "Unknown timer implementation");
    }
}

void stall(uint64_t microseconds) {
    switch (g_timer) {
        case TIMER_ACPI: {
            acpi_stall(microseconds);
        } break;

#ifdef __TOMATOS_AMD64__
        case TIMER_HPET: {

        } break;
#endif

        default: ASSERT_TRACE(0, "Unknown timer implementation");
    }
}
