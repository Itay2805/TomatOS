#ifndef TOMATKERNEL_ERROR_H
#define TOMATKERNEL_ERROR_H

#include "debug.h"

typedef enum error {
    NO_ERROR,
    ERROR_CHECK_FAILED,
    ERROR_INVALID_PARAMETER,
    ERROR_NOT_FOUND,
    ERROR_OUT_OF_RESOURCES,
    ERROR_NOT_IMPLEMENTED,
} error_t;

#define IS_ERROR(x) ((x) != NO_ERROR)

#define CHECK_ERROR_LABEL_DEBUG(expr, error, label, fmt, ...) \
    do { \
        if(!(expr)) { \
            err = (error); \
            debug_log("[-] " fmt "\n", ##__VA_ARGS__); \
            debug_log("[-] thrown at (%s:%d)\n", __FILENAME__, __LINE__); \
            goto label; \
        } \
    } while(0)

#define CHECK(expr)                 CHECK_ERROR_LABEL_DEBUG(expr, ERROR_CHECK_FAILED, cleanup, "check failed!")
#define CHECK_ERROR(expr, error)    CHECK_ERROR_LABEL_DEBUG(expr, error, cleanup, "check failed!")
#define CHECK_ERROR_DEBUG(expr, error, fmt, ...) CHECK_ERROR_LABEL_DEBUG(expr, error, cleanup, fmt, ## __VA_ARGS__)
#define CHECK_FAILED()              CHECK(false)
#define CHECK_FAILED_ERROR(error)   CHECK_ERROR(false, error)

#define CHECK_AND_RETHROW_LABEL(error, label) \
    do { \
        err = error; \
        if(IS_ERROR(err)) { \
            debug_log("[-] \trethrow at (%s:%d)\n", __FILENAME__, __LINE__); \
            goto label; \
        } \
    } while(0)

#define CHECK_AND_RETHROW(error)    CHECK_AND_RETHROW_LABEL(error, cleanup)

#endif //TOMATKERNEL_ERROR_H
