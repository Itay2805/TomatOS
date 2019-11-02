#include "stall.h"
#include "arch.h"
#include "debug.h"

static uintptr_t tsc_freq;

void stall_init(tboot_info_t* info) {
    ASSERT(info->tsc_freq >= 1000);
    tsc_freq = info->tsc_freq;
}

void stall(uint64_t milliseconds) {
    uintptr_t sleep_ticks = (milliseconds * (tsc_freq / 1000));

    uint64_t start = read_tsc();
    while(read_tsc() - start < sleep_ticks) {
        cpu_pause();
    }
}
