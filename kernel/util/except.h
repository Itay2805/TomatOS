#ifndef TOMATOS_EXCEPT_H
#define TOMATOS_EXCEPT_H

#include "trace.h"

#define is_empty(...) ( sizeof( (char[]){#__VA_ARGS__} ) == 1 )

#define CHECK_ERROR_LABEL(check, error, label, ...) \
    do { \
        if (!(check)) { \
            err = error; \
            if (!is_empty(__VA_ARGS__)) { \
                ERROR(__VA_ARGS__); \
            } \
            ERROR("Check failed with error ", err, " in function ", __FUNCTION__," (", __FILE__, ":", __LINE__, ")"); \
            goto label; \
        } \
    } while(0)

#define CHECK_ERROR(check, error, ...)              CHECK_ERROR_LABEL(check, error, cleanup, ## __VA_ARGS__)
#define CHECK_LABEL(check, label, ...)              CHECK_ERROR_LABEL(check, ERROR_CHECK_FAILED, label, ## __VA_ARGS__)
#define CHECK(check, ...)                           CHECK_ERROR_LABEL(check, ERROR_CHECK_FAILED, cleanup, ## __VA_ARGS__)

#define CHECK_FAIL(...)                             CHECK_ERROR_LABEL(0, ERROR_CHECK_FAILED, cleanup, ## __VA_ARGS__)
#define CHECK_FAIL_ERROR(error, ...)                CHECK_ERROR_LABEL(0, error, cleanup, ## __VA_ARGS__)
#define CHECK_FAIL_LABEL(label, ...)                CHECK_ERROR_LABEL(0, ERROR_CHECK_FAILED, label, ## __VA_ARGS__)
#define CHECK_FAIL_ERROR_LABEL(error, label, ...)   CHECK_ERROR_LABEL(0, error, label, ## __VA_ARGS__)

#define CHECK_AND_RETHROW_LABEL(error, label) \
    do { \
        err = error; \
        if (IS_ERROR(err)) { \
            ERROR("\trethrown at ", __FUNCTION__," (", __FILE__, ":", __LINE__, ")"); \
            goto label; \
        } \
    } while(0)

#define CHECK_AND_RETHROW(error) CHECK_AND_RETHROW_LABEL(error, cleanup)

#define WARN_ON(check, ...)     \
    do {                        \
        if (check) {            \
            WARN(__VA_ARGS__);  \
        }                       \
    } while(0)

#define ASSERT(expr, ...) \
    do { \
        if (!(expr)) { \
            ERROR("*************************"); \
            ERROR("    ASSERTION FAILED!    "); \
            ERROR("*************************"); \
            ERROR(__FUNCTION__, " (", __FILENAME__, ":", __LINE__, ")"); \
            ERROR("Condition: `", #expr, "`"); \
            if (!is_empty(__VA_ARGS__)) { \
                ERROR(__VA_ARGS__); \
            } \
            ERROR("Stack trace:"); \
            __builtin_trap(); \
        } \
    } while(0);

// TODO:             debug_trace_own_stack();

#define DEBUG_ASSERT(expr, ...) ASSERT(expr, ## __VA_ARGS__)

#endif //TOMATOS_EXCEPT_H
