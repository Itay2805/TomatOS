#ifndef __TOMATOS_KERNEL_UTIL_EXCEPT_H__
#define __TOMATOS_KERNEL_UTIL_EXCEPT_H__

#include <util/printf.h>
#include <arch/cpu.h>

#ifndef __MODULE__
    #define __MODULE__ "tomatos"
#endif

#define PRINT(fmt, ...) printf(fmt, ## __VA_ARGS__)
#define TRACE(fmt, ...) PRINT("[*][cpu:%3d] " __MODULE__ ": " fmt "\n", g_cpu_id, ## __VA_ARGS__)
#define ERROR(fmt, ...) PRINT("[-][cpu:%3d] " __MODULE__ ": " fmt "\n", g_cpu_id, ## __VA_ARGS__)

typedef enum err {
    NO_ERROR,
    ERROR_CHECK_FAILED,
    ERROR_OUT_OF_RESOURCES,
    ERROR_INVALID_HANDLE,
    ERROR_NOT_READY,
    ERROR_NOT_FOUND,
} err_t;

#define IS_ERROR(err) (err != NO_ERROR)

const char* strerror(err_t err);

#define CHECK_ERROR_LABEL_TRACE(expr, error, label, fmt, ...) \
    do { \
        if (!(expr)) { \
            err = error; \
            ERROR(fmt, ## __VA_ARGS__); \
            ERROR("CHECK failed with error `%s` in function %s (%s:%d)", strerror(err), __func__, __FILENAME__, __LINE__); \
            goto label; \
        } \
    } while(0)

#define CHECK_ERROR_TRACE(expr, error, fmt, ...) CHECK_ERROR_LABEL_TRACE(expr, error, cleanup, fmt, ## __VA_ARGS__)
#define CHECK_LABEL_TRACE(expr, label, fmt, ...) CHECK_ERROR_LABEL_TRACE(expr, ERROR_CHECK_FAILED, label, fmt, ## __VA_ARGS__)
#define CHECK_TRACE(expr, fmt, ...) CHECK_ERROR_LABEL_TRACE(expr, ERROR_CHECK_FAILED, cleanup, fmt, ## __VA_ARGS__)

#define CHECK_ERROR_LABEL(expr, error, label) \
    do { \
        if (!(expr)) { \
            err = error; \
            ERROR("CHECK failed with error `%s` in function %s (%s:%d)", strerror(err), __func__, __FILENAME__, __LINE__); \
            goto label; \
        } \
    } while(0)

#define CHECK_ERROR(expr, error) CHECK_ERROR_LABEL(expr, error, cleanup)
#define CHECK_LABEL(expr, label) CHECK_ERROR_LABEL(expr, ERROR_CHECK_FAILED, label)
#define CHECK(expr) CHECK_ERROR_LABEL(expr, ERROR_CHECK_FAILED, cleanup)

#define CHECK_FAIL_ERROR_TRACE(error, fmt, ...) CHECK_ERROR_LABEL_TRACE(0, error, cleanup, fmt, ## __VA_ARGS__)
#define CHECK_FAIL_LABEL_TRACE(label, fmt, ...) CHECK_ERROR_LABEL_TRACE(0, ERROR_CHECK_FAILED, label, fmt, ## __VA_ARGS__)
#define CHECK_FAIL_TRACE(fmt, ...) CHECK_ERROR_LABEL_TRACE(0, ERROR_CHECK_FAILED, cleanup, fmt, ## __VA_ARGS__)

#define CHECK_FAIL_ERROR(error) CHECK_ERROR_LABEL(0, error, cleanup)
#define CHECK_FAIL_LABEL(label) CHECK_ERROR_LABEL(0, ERROR_CHECK_FAILED, label)
#define CHECK_FAIL() CHECK_ERROR_LABEL(0, ERROR_CHECK_FAILED, cleanup)

#define CHECK_AND_RETHROW_LABEL(error, label) \
    do { \
        err = error; \
        if (IS_ERROR(err)) { \
            ERROR("\trethrown at %s (%s:%d)", __func__, __FILENAME__, __LINE__); \
            goto label; \
        } \
    } while(0)

#define CHECK_AND_RETHROW(error) CHECK_AND_RETHROW_LABEL(error, cleanup)

#define WARN(expr, fmt, ...) \
    do { \
        if (!(expr)) { \
            PRINT("[!] Warning! " fmt " at %s (%s:%d) \n", ## __VA_ARGS__ , __func__, __FILENAME__, __LINE__); \
        } \
    } while(0)

// TODO: replace the trap with something else?

#define ASSERT(expr) \
    do { \
        if (!(expr)) { \
            ERROR("*************************"); \
            ERROR("    ASSERTION FAILED!    "); \
            ERROR("*************************"); \
            ERROR("%s:%d", __FILENAME__, __LINE__); \
            ERROR("Condition: `%s`", #expr); \
            while(1); \
        } \
    } while(0)

#define ASSERT_TRACE(expr, fmt, ...) \
    do { \
        if (!(expr)) { \
            ERROR("*************************"); \
            ERROR("    ASSERTION FAILED!    "); \
            ERROR("*************************"); \
            ERROR("%s:%d", __FILENAME__, __LINE__); \
            ERROR("Condition: `%s`", #expr); \
            ERROR(fmt, ## __VA_ARGS__); \
            while(1); \
        } \
    } while(0);

#endif //__TOMATOS_KERNEL_UTIL_EXCEPT_H__
