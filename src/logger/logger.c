#include "logger.h"

#include <stb/stb_sprintf.h>
#include <drivers/rtc/rtc.h>
#include <locks/spinlock.h>

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

static logger_t* loggers[LOGGER_COUNT] = {0};
static spinlock_t logger_lock;
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

static char* level_names[] = {
        "DEBUG",
        "INFO",
        "WARNING",
        "ERROR",
        "CRITICAL"
};

static uint32_t colors[] = {
        0x9D9D9D,
        0x00A8A8,
        0xAA5500,
        0xAA0000,
        0x06989A
};

///////////////////////////////////////////////////////
// Logging logic
///////////////////////////////////////////////////////

static void print_level(logger_t* logger, logger_level_t level) {
    uint32_t fg = 0, bg = 0;
    if(logger->set_text_color) fg = logger->set_text_color(colors[level]);
    logger->write(level_names[level]);
    if(logger->set_text_color) logger->set_text_color(fg);
}

// [function][level] message (file:line)
static void log_per_logger(logger_t* logger, logger_level_t level, const char* function, const char* filename, int line, const char* fmt, va_list lst) {
    char buffer[LOGGER_BUFFER_SIZE];

    uint32_t fg = 0, bg = 0;
    if(logger->set_text_color) fg = logger->set_text_color(0xFFFFFF);
    if(logger->set_background_color) bg = logger->set_background_color(0x000000);

    logger->write("[");
    //logger->write(function);
    //logger->write("][");
    print_level(logger, level);
    logger->write("] ");

    // TODO: Use the callback instead
    stbsp_vsnprintf(buffer, LOGGER_BUFFER_SIZE, fmt, lst);
    logger->write(buffer);

    //logger->write(" (");
    //logger->write(filename);
    //logger->write(":");

    //stbsp_snprintf(buffer, LOGGER_BUFFER_SIZE, "%d", line);
    //logger->write(buffer);

    //logger->write(")\n");
    logger->write("\n");
    if(logger->set_text_color) logger->set_text_color(fg);
    if(logger->set_background_color) logger->set_background_color(bg);
}

void logger_log(logger_level_t level, const char* function, const char* filename, int line, const char* fmt, ...) {
    lock_preemption(&logger_lock);

    for(int i = 0; i < LOGGER_COUNT; i++) {
        if(loggers[i] != NULL && loggers[i]->enabled) {
            va_list lst;
            va_start(lst, fmt);
            log_per_logger(loggers[i], level, function, filename, line, fmt, lst);
            va_end(lst);
        }
    }

    unlock_preemption(&logger_lock);
}
