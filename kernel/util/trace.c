#include <arch/io.h>
#include <debug/debug.h>
#include "trace.h"
#include "string.h"

lock_t g_trace_lock = INIT_LOCK();

void trace_char(char c) {
    debug_write_char(c);
}

void trace_string(const char* c) {
    while (*c) {
        trace_char(*c++);
    }
}

static void trace_int(int64_t num) {
    char buf[21];

    if (num == 0) {
        trace_char('0');
        return;
    }

    if (num < 0) {
        num = -num;
        trace_char('-');
    }

    int i = 0;
    while (num != 0) {
        buf[i++] = "0123456789"[num % 10];
        num /= 10;
    }

    while (i) {
        trace_char(buf[--i]);
    }
}

void trace_int8(int8_t val) {
    trace_int(val);
}

void trace_int16(int16_t val) {
    trace_int(val);
}

void trace_int32(int32_t val) {
    trace_int(val);
}

void trace_int64(int64_t val) {
    trace_int(val);
}

static void trace_uint(uint64_t num) {
    char buf[65];

    if (num == 0) {
        trace_char('0');
        return;
    }

    int i = 0;
    while (num != 0) {
        buf[i++] = "0123456789"[num % 10];
        num /= 10;
    }

    while (i) {
        trace_char(buf[--i]);
    }
}

void trace_uint8(uint8_t val) {
    trace_uint(val);
}

void trace_uint16(uint16_t val) {
    trace_uint(val);
}

void trace_uint32(uint32_t val) {
    trace_uint(val);
}

void trace_uint64(uint64_t val) {
    trace_uint(val);
}

// boolean tracing
void trace_bool(bool val) {
    trace_string(val ? "true" : "false");
}

// error tracing
void trace_err(err_t err) {
    switch (err.value) {
        case 0: trace_string("NO_ERROR"); break;
        case 1: trace_string("ERROR_CHECK_FAILED"); break;
        case 2: trace_string("ERROR_NOT_FOUND"); break;
        case 3: trace_string("ERROR_OUT_OF_RESOURCES"); break;
        default: {
            trace_string("<error:");
            trace_uint32(err.value);
            trace_string(">");
        } break;
    }
}

// hex tracing
void trace_hex(trace_hex_t hex) {
    char buf[17];

    for (int i = (hex.bytes * 2) - 1; i >= 0; i--) {
        buf[i] = "0123456789ABCDEF"[hex.value % 16];
        hex.value /= 16;
    }

    buf[hex.bytes * 2] = 0;
    trace_string(buf);
}

// pointer tracing
void trace_ptr(void* ptr) {
    trace_string("0x");
    trace_hex(HEX((uintptr_t)ptr));
}


void trace_size(trace_size_t hex) {
    static const char* sizes[] = { "B", "KB", "MB", "GB" };
    size_t size = hex.val;
    size_t div = 0;
    size_t rem = 0;

    while (size >= 1024 && div < ARRAY_LEN(sizes)) {
        rem = (size % 1024);
        div++;
        size /= 1024;
    }

    PRINT(size, ".", (rem * 100) / 1024, sizes[div]);
}
