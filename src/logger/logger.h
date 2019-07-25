#ifndef TOMATKERNEL_LOGGER_H
#define TOMATKERNEL_LOGGER_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef LOGGER_COUNT
    #define LOGGER_COUNT 10
#endif

#ifndef LOGGER_BUFFER_SIZE
    #define LOGGER_BUFFER_SIZE 1024
#endif

typedef enum logger_level {
    LOGGER_LEVEL_DEBUG,
    LOGGER_LEVEL_INFO,
    LOGGER_LEVEL_WARN,
    LOGGER_LEVEL_ERROR,
    LOGGER_LEVEL_CRITICAL
} logger_level_t;

typedef struct logger {
    bool enabled;
    void(*write)(const char* str);
    uint32_t (*set_text_color)(uint32_t);
    uint32_t (*set_background_color)(uint32_t);
} logger_t;

/**
 * Will register a new logger
 *
 * @param logger - the logger, will be copied to an internal array
 */
void logger_register(logger_t* logger);

void logger_log(logger_level_t level, const char* function, const char* filename, int line, const char* fmt, ...);

#define log_debug(fmt, ...) logger_log(LOGGER_LEVEL_DEBUG, __FUNCTION__, __FILENAME__, __LINE__, fmt, ## __VA_ARGS__)
#define log_info(fmt, ...) logger_log(LOGGER_LEVEL_INFO, __FUNCTION__, __FILENAME__, __LINE__, fmt, ## __VA_ARGS__)
#define log_warn(fmt, ...) logger_log(LOGGER_LEVEL_WARN, __FUNCTION__, __FILENAME__, __LINE__, fmt, ## __VA_ARGS__)
#define log_error(fmt, ...) logger_log(LOGGER_LEVEL_ERROR, __FUNCTION__, __FILENAME__, __LINE__, fmt, ## __VA_ARGS__)
#define log_critical(fmt, ...) logger_log(LOGGER_LEVEL_CRITICAL, __FUNCTION__, __FILENAME__, __LINE__, fmt, ## __VA_ARGS__)

#endif //TOMATKERNEL_LOGGER_H
