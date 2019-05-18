#include <graphics/term.h>
#include <drivers/rtc.h>
#include "logging.h"
#include "stdarg.h"
#include "mini-printf.h"

#include <drivers/qemu.h>

#define LOG_LVL_CRITICAL    0
#define LOG_LVL_ERROR       1
#define LOG_LVL_WARNING     2
#define LOG_LVL_NOTICE      3
#define LOG_LVL_INFO        4
#define LOG_LVL_DEBUG       5

#ifdef LOGGING_TERMINAL
#define WRITE_TERM term_write
    #define PRINT_TERM term_print
#else
#define WRITE_TERM(...)
#define PRINT_TERM(...)
#endif

#ifdef LOGGING_QEMU
#define WRITE_QEMU qemu_write
    #define PRINT_QEMU qemu_print
#else
#define WRITE_QEMU(...)
#define PRINT_QEMU(...)
#endif

#define LOG_PRINT(...) \
    do { \
        PRINT_TERM(__VA_ARGS__); \
        PRINT_QEMU(__VA_ARGS__); \
    } while(0)

#define LOG_WRITE(...) \
    do { \
        WRITE_TERM(__VA_ARGS__); \
        WRITE_QEMU(__VA_ARGS__); \
    } while(0)


static const char* LEVEL_NAMES[] = {
        "CRT",
        "ERR",
        "WRN",
        "NOT",
        "INF",
        "DBG",
};

static uint32_t FG_COLORS[] = {
        0x75507B,
        0xCC0000,
        0xC4A000,
        0x4E9A06,
        0xD3D7CF,
        0x06989A,
};

static int log_count = 0;

static void print_pad_two(int i) {
    if(i < 10) {
        LOG_PRINT("0%d", i);
    }else {
        LOG_PRINT("%d", i);
    }
}

static void print_time() {
    time_t time = {0};
    rtc_time(&time);
    LOG_WRITE("20");
    print_pad_two(time.year);
    LOG_WRITE("-");
    print_pad_two(time.month);
    LOG_WRITE("-");
    print_pad_two(time.day);
    LOG_WRITE(" ");
    print_pad_two(time.hours);
    LOG_WRITE(":");
    print_pad_two(time.minutes);
    LOG_WRITE(":");
    print_pad_two(time.seconds);
}

// TODO: abstract color
static void do_log(const char* func, int lvl, const char* fmt, va_list va) {
    uint32_t fg = term_get_text_color();
    uint32_t bg = term_get_background_color();
    term_set_text_color(FG_COLORS[lvl]);
    term_set_background_color(0);
    ++log_count;
    if(log_count < 10) {
        LOG_PRINT("#00%d ", log_count);
    }else if(log_count < 100) {
        LOG_PRINT("#0%d ", log_count);
    }else {
        LOG_PRINT("#%d ", log_count);
    }
    print_time();
    LOG_PRINT(" \x7F %s ", LEVEL_NAMES[lvl]);
    char buffer[1024];
    mini_vsnprintf(buffer, 1024u, fmt, va);
    LOG_WRITE(buffer);
    term_set_text_color(fg);
    term_set_background_color(bg);
    LOG_WRITE("\n");
}

void log_critical(const char* func, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    do_log(func, LOG_LVL_CRITICAL, fmt, va);
    va_end(va);
}

void log_debug(const char* func, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    do_log(func, LOG_LVL_DEBUG, fmt, va);
    va_end(va);
}

void log_warn(const char* func, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    do_log(func, LOG_LVL_WARNING, fmt, va);
    va_end(va);
}

void log_error(const char* func, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    do_log(func, LOG_LVL_ERROR, fmt, va);
    va_end(va);
}

void log_notice(const char* func, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    do_log(func, LOG_LVL_NOTICE, fmt, va);
    va_end(va);
}

void log_info(const char* func, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    do_log(func, LOG_LVL_INFO, fmt, va);
    va_end(va);
}