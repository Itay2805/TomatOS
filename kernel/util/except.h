#ifndef TOMATOS_EXCEPT_H
#define TOMATOS_EXCEPT_H

#include <debug/debug.h>
#include <arch/cpu.h>
#include "trace.h"

//----------------------------------------------------------------------------------------------------------------------
// Error definitions
//----------------------------------------------------------------------------------------------------------------------


typedef enum err {
    /**
     * There was no error, everything is good
     */
    NO_ERROR,

    /**
     * Some check failed, basically an internal error
     */
    ERROR_CHECK_FAILED,

    /**
     * The requested item was not found
     */
    ERROR_NOT_FOUND,

    /**
     * The function ran out of resources to continue
     */
    ERROR_OUT_OF_RESOURCES,
} err_t;

/**
 * Check if there was an error
 */
#define IS_ERROR(err) (err != NO_ERROR)

//----------------------------------------------------------------------------------------------------------------------
// A check that fails if the expression returns false
//----------------------------------------------------------------------------------------------------------------------

#define CHECK_ERROR_LABEL(check, error, label, ...) \
    do { \
        if (!(check)) { \
            err = error; \
            IF_HAS_ARGS(__VA_ARGS__)(ERROR(__VA_ARGS__)); \
            ERROR("Check failed with error %R in function %s (%s:%d)", err, __FUNCTION__, __FILE__, __LINE__); \
            goto label; \
        } \
    } while(0)

#define CHECK_ERROR(check, error, ...)              CHECK_ERROR_LABEL(check, error, cleanup, ## __VA_ARGS__)
#define CHECK_LABEL(check, label, ...)              CHECK_ERROR_LABEL(check, ERROR_CHECK_FAILED, label, ## __VA_ARGS__)
#define CHECK(check, ...)                           CHECK_ERROR_LABEL(check, ERROR_CHECK_FAILED, cleanup, ## __VA_ARGS__)

//----------------------------------------------------------------------------------------------------------------------
// A check that fails without a condition
//----------------------------------------------------------------------------------------------------------------------

#define CHECK_FAIL(...)                             CHECK_ERROR_LABEL(0, ERROR_CHECK_FAILED, cleanup, ## __VA_ARGS__)
#define CHECK_FAIL_ERROR(error, ...)                CHECK_ERROR_LABEL(0, error, cleanup, ## __VA_ARGS__)
#define CHECK_FAIL_LABEL(label, ...)                CHECK_ERROR_LABEL(0, ERROR_CHECK_FAILED, label, ## __VA_ARGS__)
#define CHECK_FAIL_ERROR_LABEL(error, label, ...)   CHECK_ERROR_LABEL(0, error, label, ## __VA_ARGS__)

//----------------------------------------------------------------------------------------------------------------------
// A check that fails if an error was returned, used around functions returning an error
//----------------------------------------------------------------------------------------------------------------------

#define CHECK_AND_RETHROW_LABEL(error, label) \
    do { \
        err = error; \
        if (IS_ERROR(err)) { \
            ERROR("\trethrown at %s (%s:%d)", __FUNCTION__, __FILE__, __LINE__); \
            goto label; \
        } \
    } while(0)

#define CHECK_AND_RETHROW(error) CHECK_AND_RETHROW_LABEL(error, cleanup)

//----------------------------------------------------------------------------------------------------------------------
// Misc utilities
//----------------------------------------------------------------------------------------------------------------------

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
            ERROR("%s (%s:%d)", __FUNCTION__, __FILE__, __LINE__); \
            ERROR("Condition: `%s`", #expr); \
            IF_HAS_ARGS(__VA_ARGS__)(ERROR(__VA_ARGS__)); \
            ERROR("Stack trace:");\
            debug_trace_me(); \
            while(1) cpu_sleep(); \
        } \
    } while(0);

#define DEBUG_ASSERT(expr, ...) ASSERT(expr, ## __VA_ARGS__)

#endif //TOMATOS_EXCEPT_H
