//
// Created by Itay on 31/05/2019.
//

#include "logger.h"

#include <vsnprintf.h>
#include <drivers/rtc/rtc.h>

///////////////////////////////////////////////////////
// Log levels and loggers
///////////////////////////////////////////////////////

enum {
    LOG_LEVEL_CRITICAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_NOTICE,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
};

logger_t* loggers[LOGGER_COUNT] = {};

static int log_count = 0;

void logger_register(logger_t* logger) {
    for(int i = 0; i < LOGGER_COUNT; i++) {
        if(loggers[i] == NULL) {
            loggers[i] = logger;
            loggers[i]->enabled = true;
            break;
        }
    }
}

///////////////////////////////////////////////////////
// Display configurations
///////////////////////////////////////////////////////

static const char* LEVEL_NAMES[] = {
        "CRT ",
        "ERR ",
        "WRN ",
        "NOT ",
        "INF ",
        "DBG ",
};

static uint32_t FG_COLORS[] = {
        0x75507B,
        0xCC0000,
        0xC4A000,
        0x4E9A06,
        0xD3D7CF,
        0x06989A,
};

///////////////////////////////////////////////////////
// Logging logic
///////////////////////////////////////////////////////

static void write_to_loggers(int level, const char* str) {
    for(logger_t** logger = loggers; logger < loggers + LOGGER_COUNT; logger++) {
        if(*logger != NULL && (*logger)->enabled) {
            uint32_t fg = 0;
            if((*logger)->set_text_color) fg = (*logger)->set_text_color(FG_COLORS[level]);
            (*logger)->write(str);
            if((*logger)->set_text_color) (*logger)->set_text_color(fg);
        }
    }
}

/**
 * Will print the time and the count
 */
static void print_prefix(int level) {
    char buffer[30];
    time_t time = {0};
    int count = log_count++;

    rtc_time(&time);
    //snprintf(buffer, sizeof(buffer), "#%03d 20%02d-%02d-%02d %02d:%02d:%02d ", count, time.year, time.month, time.day, time.hours, time.minutes, time.seconds);
    snprintf(buffer, sizeof(buffer), "#%03d %02d:%02d:%02d ", count, time.hours, time.minutes, time.seconds);
    write_to_loggers(level, buffer);
}

static void do_log(const char* filename, const char* function, int line, int level, const char* str) {
    print_prefix(level);
    write_to_loggers(level, LEVEL_NAMES[level]);
    write_to_loggers(level, str);
    write_to_loggers(level, "\n");


}

void log_info_full(const char* filename, const char* function, int line, const char* fmt, ...) {
    char buffer[LOGGER_BUFFER_SIZE];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, LOGGER_BUFFER_SIZE, fmt, ap);
    do_log(filename, function, line, LOG_LEVEL_INFO, buffer);
    va_end(ap);
}

void log_notice_full(const char* filename, const char* function, int line, const char* fmt, ...) {
    char buffer[LOGGER_BUFFER_SIZE];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, LOGGER_BUFFER_SIZE, fmt, ap);
    do_log(filename, function, line, LOG_LEVEL_NOTICE, buffer);
    va_end(ap);
}

void log_debug_full(const char* filename, const char* function, int line, const char* fmt, ...) {
    char buffer[LOGGER_BUFFER_SIZE];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, LOGGER_BUFFER_SIZE, fmt, ap);
    do_log(filename, function, line, LOG_LEVEL_DEBUG, buffer);
    va_end(ap);
}

void log_warn_full(const char* filename, const char* function, int line, const char* fmt, ...) {
    char buffer[LOGGER_BUFFER_SIZE];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, LOGGER_BUFFER_SIZE, fmt, ap);
    do_log(filename, function, line, LOG_LEVEL_WARNING, buffer);
    va_end(ap);
}

void log_error_full(const char* filename, const char* function, int line, const char* fmt, ...) {
    char buffer[LOGGER_BUFFER_SIZE];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, LOGGER_BUFFER_SIZE, fmt, ap);
    do_log(filename, function, line, LOG_LEVEL_ERROR, buffer);
    va_end(ap);
}

void log_critical_full(const char* filename, const char* function, int line, const char* fmt, ...) {
    char buffer[LOGGER_BUFFER_SIZE];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, LOGGER_BUFFER_SIZE, fmt, ap);
    do_log(filename, function, line, LOG_LEVEL_CRITICAL, buffer);
    va_end(ap);
}
