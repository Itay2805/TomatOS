#include <stb_sprintf.h>
#include <arch/intrin.h>
#include <event/event.h>

#include <stdarg.h>
#include <sync/spinlock.h>

#include "serial.h"
#include "trace.h"

static spinlock_t trace_lock = SPINLOCK_INIT;

void trace(const char* fmt, ...) {
    char buffer[STB_SPRINTF_MIN] = {0};

    va_list list;
    va_start(list, fmt);
    size_t len = stbsp_vsnprintf(buffer, sizeof(buffer), fmt, list);
    va_end(list);

    spinlock_acquire_high_tpl(&trace_lock);

    // send to port 0xE9 (Qemu log port)
    for (int i = 0; i < len; i++) {
        __outbyte(0xE9, buffer[i]);
    }

    spinlock_release(&trace_lock);
}
