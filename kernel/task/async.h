#ifndef TOMATOS_ASYNC_H
#define TOMATOS_ASYNC_H


#include <util/cpp_magic.h>
#include <stdalign.h>
#include <stdbool.h>
#include <string.h>

/**
 * Returned from an async function, can be used to await/wait for it
 */
typedef void* future_t;

#define void_t

#define yield() \
    do { \
        switch (__builtin_coro_suspend(false)) { \
            case 0: break; \
            case 1: goto __coro_cleanup; \
            default: goto __coro_suspend; \
        } \
    } while (0)

#define ret(...) \
    do { \
        *(__coro_ret_t*)__builtin_coro_promise(__coro_hdl, 0, false) = \
        IF_HAS_ARGS(__VA_ARGS__)( \
            __VA_ARGS__; \
        )\
        IF_EMPTY(__VA_ARGS__)( \
            0; \
        )\
        goto __coro_final; \
    } while (0);

#define await(func, ...) \
    ({ \
        future_t t = func(__VA_ARGS__); \
        while (!__builtin_coro_done(t)) { \
            yield(); \
            __builtin_coro_resume(t); \
        } \
        promise_type(func) retval = *(promise_type(func)*)__builtin_coro_promise(t, 0, false); \
        __builtin_coro_destroy(t); \
        retval; \
    })

/**
 * The promise type of the given function
 */
#define promise_type(func) typeof(CAT3(__, func, _type)())

/**
 * Will wait (blocking) for the future to finish
 */
#define wait(func, ...) \
    ({ \
        future_t t = func(__VA_ARGS__); \
        while (!__builtin_coro_done(t)) { \
            __builtin_coro_resume(t); \
        } \
        promise_type(func) retval = *(promise_type(func)*)__builtin_coro_promise(t, 0, false); \
        __builtin_coro_destroy(t); \
        retval; \
    })

/**
 * Declare a new async function, async functions return a future which needs to be waited on either
 * in an async (inside another async function) or a sync (outside of an async function).
 */
#define async(ret, name, sig, ...) \
    IF_HAS_ARGS(CAT(ret, _t))( \
        ret CAT3(__, name, _type) (); \
    ) \
    IF_EMPTY(CAT(ret, _t)) \
    ( \
        int CAT3(__, name, _type) (); \
    ) \
    IF_EMPTY(__VA_ARGS__)( \
        future_t name sig; \
    ) \
    IF_HAS_ARGS(__VA_ARGS__)( \
        future_t name sig { \
            void* mem; \
            IF_HAS_ARGS(CAT(ret, _t))( \
                typedef ret __coro_ret_t; \
                __builtin_coro_id(0, &((char[sizeof(ret)]){}), NULL, NULL); \
            ) \
            IF_EMPTY(CAT(ret, _t)) \
            ( \
                typedef int __coro_ret_t; \
                __builtin_coro_id(0, &((char[sizeof(int)]){}), NULL, NULL); \
            ) \
            void* alloc = NULL; \
            if (__builtin_coro_alloc()) { \
                alloc = malloc(__builtin_coro_size()); \
            } \
            void* __coro_hdl = __builtin_coro_begin(alloc); \
            { \
                __VA_ARGS__ \
            } \
            IF_HAS_ARGS(CAT(ret, _t))( \
                __builtin_trap(); \
            ) \
        __coro_final: \
            switch (__builtin_coro_suspend(true)) { \
                case 0: __builtin_trap(); \
                case 1: goto __coro_cleanup; \
                default: goto __coro_suspend; \
            } \
        __coro_cleanup: \
            mem = __builtin_coro_free(__coro_hdl); \
            free(mem); \
        __coro_suspend: \
            __builtin_coro_end(__coro_hdl, false); \
            return __coro_hdl; \
        } \
    )
#endif //TOMATOS_ASYNC_H
