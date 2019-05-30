#ifndef VSPRINTF_H
#define VSPRINTF_H

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

// Taken from
// https://gitlab.com/qookei/quack/blob/master/kernel/vsnprintf.h

void vsnprintf(char *, size_t, const char *, va_list);

static inline void snprintf(char * buf, size_t len, const char * fmt, ...) {
    va_list va;
    va_start(va, fmt);
    vsnprintf(buf, len, fmt, va);
    va_end(va);
}

#endif
