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

    ERROR_NOT_IMPLEMENTED,

    ERROR_COUNT
} error_t;

#ifndef __FILENAME__
    #define __FILENAME__ __FILE__
#endif

extern const char* error_names[ERROR_COUNT];

#define CHECK_ERROR_LOGGER_TRACE(cond, error, log_func, fmt, ...) \
    do { \
        if(!(cond)) { \
            err = error; \
            log_func("error `%s` thrown (%s:%d), trace:", error_names[err], __FILENAME__, __LINE__); \
            log_func(fmt, ## __VA_ARGS__); \
            goto cleanup; \
        } \
    } while(0)

#define CHECK_ERROR_TRACE(cond, error, fmt, ...) \
    CHECK_ERROR_LOGGER_TRACE(cond, error, log_error, fmt, ## __VA_ARGS__)

#define CHECK_TRACE(cond, fmt, ...) \
    CHECK_ERROR_TRACE(cond, ERROR_CHECK_FAILED, fmt, ## __VA_ARGS__)

#define CHECK_ERROR_LOG(cond, error, log_func) \
    do { \
        if(!(cond)) { \
            err = error; \
            log_func("error `%s` thrown (%s:%d)", error_names[err], __FILENAME__, __LINE__); \
            goto cleanup; \
        } \
    } while(0)

#define CHECK_ERROR(cond, error) \
    CHECK_ERROR_LOG(cond, error, log_error)

#define CHECK(cond) \
    CHECK_ERROR(cond, ERROR_CHECK_FAILED)

#define CHECK_FAIL() \
    CHECK(false)

#define CHECK_FAIL_ERROR(error) \
    CHECK_ERROR(false, error)

#define CHECK_FAIL_ERROR_TRACE(error, fmt, ...) \
    CHECK_ERROR_TRACE(false, error, fmt, ## __VA_ARGS__)

#define CHECK_AND_RETHROW_LOG_LABEL(error, log_func, label) \
    do { \
        err = (error); \
        if(err != NO_ERROR) { \
            log_func("\trethrown (%s:%d)", __FILENAME__, __LINE__); \
            goto label; \
        } \
    } while(0)

#define CHECK_AND_RETHROW_LABEL(error, label) \
    CHECK_AND_RETHROW_LOG_LABEL(error, log_error, label)

#define CHECK_AND_RETHROW_LOG(error) \
    CHECK_AND_RETHROW_LOG_LABEL(error, log_error)

#define CHECK_AND_RETHROW(error) \
    CHECK_AND_RETHROW_LOG_LABEL(error, log_error, cleanup)

#define CATCH(err) \
    do { \
        if((err) != NO_ERROR) { \
            log_warn("\tcatched (%s:%d)", __FILENAME__, __LINE__); \
        } \
    } while(0)

#endif //TOMATKERNEL_ERROR_H
