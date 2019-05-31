#ifndef TOMATKERNEL_ERROR_H
#define TOMATKERNEL_ERROR_H

#include <logger/logger.h>

typedef enum error {
    NO_ERROR,
    ERROR_CHECK_FAILED,
    ERROR_INVALID_ARGUMENT,
    ERROR_INVALID_POINTER,

    ERROR_OUT_OF_MEMORY,
    ERROR_NOT_FOUND,

    ERROR_ALREADY_MAPPED,
    ERROR_NOT_MAPPED,

    ERROR_INVALID_SCHEME,
    ERROR_INVALID_DOMAIN,
    ERROR_INVALID_PORT,
    ERROR_INVALID_PATH,

    ERROR_SCHEME_EXISTS,
    ERROR_DOMAIN_EXISTS,
    ERROR_PORT_EXISTS,
    ERROR_PATH_EXISTS,

    ERROR_NOT_READABLE,
    ERROR_NOT_WRITEABLE,
    ERROR_NOT_SEEKABLE,
    ERROR_NOT_DIRECTORY,
    ERROR_NOT_POLLABLE,

    ERROR_INVALID_RESOURCE,

    ERROR_COUNT
} error_t;

#ifndef __FILENAME__
    #define __FILENAME__ __FILE__
#endif

extern const char* error_names[ERROR_COUNT];

#define CHECK_ERROR_TRACE(cond, error, fmt, ...) \
    do { \
        if(!(cond)) { \
            err = error; \
            log_error("error `%s` thrown (%s:%d), trace:", error_names[err], __FILENAME__, __LINE__); \
            log_error(fmt, ## __VA_ARGS__); \
            goto cleanup; \
        } \
    } while(0)

#define CHECK_TRACE(cond, fmt, ...) \
    CHECK_ERROR_TRACE(cond, ERROR_CHECK_FAILED, fmt, ## __VA_ARGS__)

#define CHECK_ERROR(cond, error) \
    do { \
        if(!(cond)) { \
            err = error; \
            log_error("error `%s` thrown (%s:%d)", error_names[err], __FILENAME__, __LINE__); \
            goto cleanup; \
        } \
    } while(0)

#define CHECK(cond) \
    CHECK_ERROR(cond, ERROR_CHECK_FAILED)

#define CHECK_AND_RETHROW(error) \
    do { \
        err = (error); \
        if(err != NO_ERROR) { \
            log_error("\trethrown (%s:%d)", __FILENAME__, __LINE__); \
            goto cleanup; \
        } \
    } while(0)

#endif //TOMATKERNEL_ERROR_H
