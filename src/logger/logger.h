#ifndef TOMATKERNEL_LOGGER_H
#define TOMATKERNEL_LOGGER_H

#include <stdarg.h>
#include <stdbool.h>

#ifndef LOGGER_COUNT
    #define LOGGER_COUNT 10
#endif

#ifndef LOGGER_BUFFER_SIZE
    #define LOGGER_BUFFER_SIZE 1024
#endif

typedef struct logger {
    bool enabled;
    void(*write)(const char* str);
} logger_t;

/**
 * Will register a new logger
 *
 * @param logger - the logger, will be copied to an internal array
 */
void logger_register(const logger_t* logger);

void log_debug_full(const char* filename, const char* function, int line, const char* fmt, ...);
void log_info_full(const char* filename, const char* function, int line, const char* fmt, ...);
void log_warn_full(const char* filename, const char* function, int line, const char* fmt, ...);
void log_error_full(const char* filename, const char* function, int line, const char* fmt, ...);
void log_critical_full(const char* filename, const char* function, int line, const char* fmt, ...);

#define log_debug(fmt, ...) log_debug_full(__FILENAME__, __FUNCTION__, __LINE__, fmt, ## __VA_ARGS__)
#define log_info(fmt, ...) log_info_full(__FILENAME__, __FUNCTION__, __LINE__, fmt, ## __VA_ARGS__)
#define log_warn(fmt, ...) log_warn_full(__FILENAME__, __FUNCTION__, __LINE__, fmt, ## __VA_ARGS__)
#define log_error(fmt, ...) log_error_full(__FILENAME__, __FUNCTION__, __LINE__, fmt, ## __VA_ARGS__)
#define log_critical(fmt, ...) log_critical_full(__FILENAME__, __FUNCTION__, __LINE__, fmt, ## __VA_ARGS__)

#endif //TOMATKERNEL_LOGGER_H
