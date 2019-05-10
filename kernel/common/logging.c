#include <graphics/term.h>
#include <drivers/rtc.h>
#include "logging.h"
#include "stdarg.h"
#include "mini-printf.h"

#define LOG_LVL_CRITICAL    0
#define LOG_LVL_ERROR       1
#define LOG_LVL_WARNING     2
#define LOG_LVL_NOTICE      3
#define LOG_LVL_INFO        4
#define LOG_LVL_DEBUG       5

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
        term_print("0%d", i);
    }else {
        term_print("%d", i);
    }
}

static void print_time() {
    time_t time = {0};
    rtc_time(&time);
    term_write("20");
    print_pad_two(time.year);
    term_write("-");
    print_pad_two(time.month);
    term_write("-");
    print_pad_two(time.day);
    term_write(" ");
    print_pad_two(time.hours);
    term_write(":");
    print_pad_two(time.minutes);
    term_write(":");
    print_pad_two(time.seconds);
}

static void do_log(const char* func, int lvl, const char* fmt, va_list va) {
    uint32_t fg = term_get_text_color();
    uint32_t bg = term_get_background_color();
    term_set_text_color(FG_COLORS[lvl]);
    term_set_background_color(0);
    ++log_count;
    if(log_count < 10) {
        term_print("#00%d ", log_count);
    }else if(log_count < 100) {
        term_print("#0%d ", log_count);
    }else {
        term_print("#%d ", log_count);
    }
    print_time();
    term_print(" \x7F %s ", LEVEL_NAMES[lvl]);
    char buffer[1024];
    mini_vsnprintf(buffer, 1024u, fmt, va);
    term_write(buffer);
    term_set_text_color(fg);
    term_set_background_color(bg);
    term_write("\n");
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
