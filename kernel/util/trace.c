#include <arch/io.h>
#include <debug/debug.h>
#include "trace.h"
#include "string.h"
#include "except.h"

//
// We initialize the trace lock to be at a high level to make sure
// nothing can interrupt while we print stuff
//
lock_t g_trace_lock = INIT_LOCK(TPL_HIGH_LEVEL);

//----------------------------------------------------------------------------------------------------------------------

const char* num_fmt(char* buf, uint64_t i, int base, int padding, char pad_with, int handle_signed, int upper, int len) {
    int neg = (signed)i < 0 && handle_signed;

    if (neg) {
        i = (unsigned)(-((signed)i));
    }

    char *ptr = buf + 64;
    *ptr = '\0';

    const char* digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";

    do {
        *--ptr = digits[i % base];
        if (padding) {
            padding--;
        }
        if (len > 0) {
            len--;
        }
    } while ((i /= base) != 0 && (len == -1 || len));

    while (padding) {
        *--ptr = pad_with;
        padding--;
    }

    if (neg) {
        *--ptr = '-';
    }

    return ptr;
}

size_t kvcprintf(printf_callback_t cb, void* ctx, const char* fmt, va_list ap) {
    size_t size = 0;

    while (*fmt) {
        if (*fmt != '%') {
            cb(*fmt, ctx);
            fmt++;
            size++;
            continue;
        }

        fmt++;

        // pad with zeros instead of spaces
        char pad_with = ' ';
        if (*fmt == '0') {
            pad_with = '0';
            fmt++;
        }

        // padding
        int padding = 0;
        while ('0' <= *fmt && *fmt <= '9') {
            padding *= 10;
            padding += *fmt++ - '0';
        }

        // l prefix for wide
        bool wide = false;
        while (*fmt == 'l') {
            wide = true;
            fmt++;
        }

        // for upper
        bool upper = *fmt == 'X' || *fmt == 'P';

        // for int/string args
        uint64_t i = 0;
        char num_buf[65] = {0};
        const char* s;

        // format char
        switch (*fmt) {
            //----------------------------------------------------------------------------------------------------------
            // Standard formats
            //----------------------------------------------------------------------------------------------------------

            case 'c': {
                i = va_arg(ap, int);
                cb(i, ctx);
                size++;
            } break;

            case 'd': {
                if (wide) {
                    i = va_arg(ap, long int);
                } else {
                    i = va_arg(ap, int);
                }

                s = num_fmt(num_buf, i, 10, padding, pad_with, true, false, -1);
                while (*s) {
                    cb(*s, ctx);
                    s++;
                    size++;
                }
            } break;

            case 'u': {
                if (wide) {
                    i = va_arg(ap, long int);
                } else {
                    i = va_arg(ap, int);
                }

                s = num_fmt(num_buf, i, 10, padding, pad_with, false, false, -1);
                while (*s) {
                    cb(*s, ctx);
                    s++;
                    size++;
                }
            } break;

            case 'x':
            case 'X': {
                if (wide) {
                    i = va_arg(ap, long int);
                } else {
                    i = va_arg(ap, int);
                }

                s = num_fmt(num_buf, i, 16, padding, pad_with, false, upper, wide ? 16 : 8);
                while (*s) {
                    cb(*s, ctx);
                    s++;
                    size++;
                }
            } break;

            case 'b': {
                if (wide) {
                    i = va_arg(ap, long int);
                } else {
                    i = va_arg(ap, int);
                }

                s = num_fmt(num_buf, i, 2, padding, pad_with, false, upper, wide ? 64 : 32);
                while (*s) {
                    cb(*s, ctx);
                    s++;
                    size++;
                }
            } break;

            case 'p':
            case 'P': {
                i = (uintptr_t)va_arg(ap, void*);

                s = num_fmt(num_buf, i, 16, padding, pad_with, false, upper, 16);
                while (*s) {
                    cb(*s, ctx);
                    s++;
                    size++;
                }
            } break;

            case 's': {
                s = va_arg(ap, char*);
                if (s == NULL) {
                    s = "<null>";
                }
                while (*s) {
                    cb(*s, ctx);
                    s++;
                    size++;
                }
            } break;

            case '%': {
                cb('%', ctx);
                size++;
            } break;

            //----------------------------------------------------------------------------------------------------------
            // custom formats
            //----------------------------------------------------------------------------------------------------------

            // print size
            case 'S': {
                if (wide) {
                    i = va_arg(ap, long int);
                } else {
                    i = va_arg(ap, int);
                }

                static char* sizes[] = { "B", "KB", "MB", "GB" };
                size_t div = 0;
                size_t rem = 0;

                while (i >= 1024 && div < ARRAY_LEN(sizes)) {
                    rem = (i % 1024);
                    div++;
                    i /= 1024;
                }

                // print the size
                s = num_fmt(num_buf, i, 10, 0, ' ', false, false, -1);
                while (*s) {
                    cb(*s, ctx);
                    s++;
                    size++;
                }

                if ((rem * 100) / 1024 != 0) {
                    cb('.', ctx);
                    size++;

                    s = num_fmt(num_buf, i, 10, 0, ' ', false, false, -1);
                    while (*s) {
                        cb(*s, ctx);
                        s++;
                        size++;
                    }
                }

                s = sizes[div];
                while (*s) {
                    cb(*s, ctx);
                    s++;
                    size++;
                }
            } break;

            // print error
            case 'R': {
                err_t err = va_arg(ap, err_t);
                switch (err) {
                    case NO_ERROR: s = "NO_ERROR"; break;
                    case ERROR_CHECK_FAILED: s = "ERROR_CHECK_FAILED"; break;
                    case ERROR_NOT_FOUND: s = "ERROR_NOT_FOUND"; break;
                    case ERROR_OUT_OF_RESOURCES: s = "ERROR_OUT_OF_RESOURCES"; break;
                    default: s = num_fmt(num_buf, err, 0, 0, ' ', false, false, -1); break;
                }
                while (*s) {
                    cb(*s, ctx);
                    s++;
                    size++;
                }
            } break;

            // print tpl
            case 't': {
                tpl_t tpl = va_arg(ap, tpl_t);
                switch (tpl) {
                    case TPL_APPLICATION: s = "TPL_APPLICATION"; break;
                    case TPL_NOTIFY: s = "TPL_NOTIFY"; break;
                    case TPL_HIGH_LEVEL: s = "TPL_HIGH_LEVEL"; break;
                    case TPL_CALLBACK: s = "TPL_CALLBACK"; break;
                    default: s = num_fmt(num_buf, tpl, 0, 0, ' ', false, false, -1); break;
                }
                while (*s) {
                    cb(*s, ctx);
                    s++;
                    size++;
                }
            } break;
        }

        fmt++;
    }

    return size;
}

//----------------------------------------------------------------------------------------------------------------------

typedef struct buffer_context {
    char* buffer;
    size_t size;
} buffer_context_t;

static void buffer_output_cb(char c, buffer_context_t* ctx) {
    if (ctx->size == 1) {
        // force terminator
        *ctx->buffer = '\0';
        ctx->size--;
    } else if (ctx->size > 1) {
        // place for more stuff
        *ctx->buffer = c;
        ctx->buffer++;
        ctx->size--;
    }
}

size_t kvsnprintf(char* buffer, size_t size, const char* fmt, va_list ap) {
    buffer_context_t ctx = {
        .buffer = buffer,
        .size = size,
    };
    return kvcprintf((printf_callback_t) buffer_output_cb, &ctx, fmt, ap);
}

size_t ksnprintf(char* buffer, size_t size, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    size_t retsize = kvsnprintf(buffer, size, fmt, ap);
    va_end(ap);
    return retsize;
}

//----------------------------------------------------------------------------------------------------------------------

static void debug_output_cb(char c, void* ctx) {
    debug_write_char(c);
}

size_t kprintf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    size_t size = kvcprintf(debug_output_cb, NULL, fmt, ap);
    va_end(ap);
    return size;
}
