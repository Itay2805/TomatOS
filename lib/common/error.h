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

    ERROR_THREAD_CANCELED,
    ERROR_THREAD_RUNNING,

    ERROR_NOT_SUPPORTED,

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
            log_error("error `%s`: " fmt, error_names[err], ## __VA_ARGS__); \
            log_error("thrown (%s:%d), trace:", __FILENAME__, __LINE__); \
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

#define CHECK_FAIL() \
    CHECK(false)

#define CHECK_FAIL_ERROR(error) \
    CHECK_ERROR(false, error)

#define CHECK_FAIL_ERROR_TRACE(error, fmt, ...) \
    CHECK_ERROR_TRACE(false, error, fmt, ## __VA_ARGS__)

#define CHECK_AND_RETHROW_LABEL(error, label) \
    do { \
        err = (error); \
        if(err != NO_ERROR) { \
            log_error("\trethrown (%s:%d)", __FILENAME__, __LINE__); \
            goto label; \
        } \
    } while(0)

#define CHECK_AND_RETHROW(error) \
    CHECK_AND_RETHROW_LABEL(error, cleanup)

#define CATCH(err, ...) \
    do { \
        if((err) != NO_ERROR) { \
            log_warn("catched `%s` (%s:%d)", error_names[err], __FILENAME__, __LINE__); \
            __VA_ARGS__; \
        } \
    } while(0)

/*********************************************************
* Quite version of these functions
*********************************************************/

#define CHECK_ERROR_QUITE(cond, error) \
    do { \
        if(!(cond)) { \
            err = error; \
            goto cleanup; \
        } \
    } while(0)

#define CHECK_QUITE(cond) \
    CHECK_ERROR_QUITE(cond, ERROR_CHECK_FAILED)

#define CHECK_AND_RETHROW_LABEL_QUITE(error, label) \
    do { \
        err = (error); \
        if(err != NO_ERROR) { \
            goto label; \
        } \
    } while(0)

#define CHECK_AND_RETHROW_QUITE(error) \
    CHECK_AND_RETHROW_LABEL_QUITE(error, cleanup)


#endif //TOMATKERNEL_ERROR_H
