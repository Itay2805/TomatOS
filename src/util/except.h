#ifndef __UTIL_EXCEPT_H__
#define __UTIL_EXCEPT_H__

#include <arch/intrin.h>
#include <stddef.h>
#include "trace.h"

#define PANIC_CR2_MAGIC 0xCAFEBABE

typedef enum err {
    NO_ERROR,
    ERROR_CHECK_FAILED,
    ERROR_NOT_READY,
    ERROR_INVALID_PARAM,
    ERROR_OUT_OF_RESOURCES,
    ERROR_NOT_FOUND,
    ERROR_INVALID_TPL,
} err_t;

#define IS_ERROR(err) (err != NO_ERROR)

const char* strerror(err_t err);
void panic(const char* panic_message);

#define CHECK_ERROR_LABEL_TRACE(expr, error, label, fmt, ...) \
    do { \
        if (!(expr)) { \
            err = error; \
            trace("[-] Check failed with error `%s` int function %s (%s:%d)\n", strerror(err), __func__, __FILENAME__, __LINE__); \
            if (fmt[0] != '\0') { \
                trace("[-] " fmt "\n", ## __VA_ARGS__); \
            } \
            goto label; \
        } \
    } while(0)

#define CHECK_ERROR_TRACE(expr, error, fmt, ...) CHECK_ERROR_LABEL_TRACE(expr, error, cleanup, fmt, ## __VA_ARGS__)
#define CHECK_ERROR(expr, error) CHECK_ERROR_LABEL_TRACE(expr, error, cleanup, "")
#define CHECK(expr) CHECK_ERROR(expr, ERROR_CHECK_FAILED)
#define CHECK_FAIL() CHECK(0)

#define CHECK_AND_RETHROW_LABEL(error, label) \
    do { \
        err = error; \
        if (IS_ERROR(err)) { \
            trace("[-] \trethrown at %s (%s:%d)\n", __func__, __FILENAME__, __LINE__); \
            goto label; \
        } \
    } while(0)

#define CHECK_AND_RETHROW(error) CHECK_AND_RETHROW_LABEL(error, cleanup)

#define WARN(expr, fmt, ...) \
    do { \
        if (!(expr)) { \
            trace("[!] Warning! " fmt " at %s (%s:%d) \n", ## __VA_ARGS__ , __func__, __FILENAME__, __LINE__); \
        } \
    } while(0)

#define ASSERT(expr) \
    do { \
        if (!(expr)) { \
            trace("GOT ASSERTION at %s (%s:%d)!!! " # expr "\n", __func__, __FILENAME__, __LINE__); \
            __writecr2(PANIC_CR2_MAGIC); \
            while(1) asm volatile("int $3"); \
        } \
    } while(0)

#define ASSERT_TRACE(expr, fmt, ...) \
    do { \
        if (!(expr)) { \
            trace("GOT ASSERTION at %s (%s:%d)!!! " fmt "\n", __func__, __FILENAME__, __LINE__, ## __VA_ARGS__); \
            __writecr2(PANIC_CR2_MAGIC); \
            while(1) asm volatile("int $3"); \
        } \
    } while(0);

#endif //__UTIL_EXCEPT_H__
