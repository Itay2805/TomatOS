#include <common/cpu/atomic.h>
#include <helpers/hpet/hpet.h>
#include "event.h"

void event_signal(event_t* event) {
    lock(&event->lock);
    event->signaled = true;
    unlock(&event->lock);
}

bool event_wait(event_t* event, uint64_t timeout) {
    if(timeout == 0) timeout = UINT64_MAX;

    // wait for the signal
    uint64_t start = hpet_get_millis();
    while(!event->signaled && hpet_get_millis() - start < timeout) {
        _pause();
    }

    // clear the signal
    lock(&event->lock);
    bool signaled = event->signaled;
    event->signaled = false;
    unlock(&event->lock);

    return signaled;
}
