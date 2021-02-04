#ifndef TOMATOS_TASK_H
#define TOMATOS_TASK_H

#include <cont/list.h>
#include <mem/mm.h>

#include "async.h"

/**
 * This defines some set priorities
 * for tasks
 */
typedef enum tpl {
    /**
     * TODO: this
     */
    TPL_APPLICATION = 4,

    /**
     * TODO: this
     */
    TPL_CALLBACK = 8,

    /**
     * TODO: this
     */
    TPL_NOTIFY = 16,

    /**
     * TODO: this
     */
    TPL_HIGH_LEVEL = 31,
} tpl_t;

/**
 * Raises a task's priority level and return its previous level
 *
 * @return          [IN] The new task priority level
 */
tpl_t raise_tpl(tpl_t new_tpl);

/**
 * Restore the task priority level to its previous value
 *
 * @param old_tpl   [IN] The previous task priority level
 */
void restore_tpl(tpl_t old_tpl);

typedef struct task {
    /**
     * The name of this task
     */
    char name[32];

    /**
     * The priority of this task
     */
    tpl_t tpl;

    /**
     * Link into the scheduler list
     */
    list_entry_t schedule_link;
} task_t;

/**
 * A handle to a task, not that useful but required for creating a new task
 */
typedef void* task_handle_t;

static inline void task_resume(task_t* task) {
    __builtin_coro_resume(__builtin_coro_promise(task, 0, true));
}

static inline void task_destroy(task_t* task) {
    __builtin_coro_destroy(__builtin_coro_promise(task, 0, true));
}

static inline bool task_done(task_t* task) {
    return __builtin_coro_done(__builtin_coro_promise(task, 0, true));
}

#define task(name, sig, ...) \
    IF(NOT(HAS_ARGS(__VA_ARGS__)))( \
        task_handle_t name sig; \
    ) \
    IF(HAS_ARGS(__VA_ARGS__))( \
        task_handle_t name sig { \
            void* __coro_mem;\
            task_t* __coro_task = __builtin_alloca(sizeof(task_t)); \
            __builtin_coro_id(0, __coro_task, NULL, NULL); \
            void* __coro_alloc = NULL; \
            if (__builtin_coro_alloc()) { \
                __coro_alloc = kalloc(__builtin_coro_size()); \
                if (__coro_alloc == NULL) { \
                    return NULL; \
                }\
            } \
            void* __coro_hdl = __builtin_coro_begin(__coro_alloc); \
            yield(); \
            { \
                EVAL1 FIRST(__VA_ARGS__) \
            } \
        __coro_final: \
            switch (__builtin_coro_suspend(true)) { \
                case 0: __builtin_trap(); \
                case 1: goto __coro_cleanup; \
                default: goto __coro_suspend; \
            } \
        __coro_cleanup:      \
            IF(HAS_SECOND(__VA_ARGS__))(SECOND(__VA_ARGS__)); \
            list_remove(&__coro_task->schedule_link); \
            __coro_mem = __builtin_coro_free(__coro_hdl); \
            kfree(__coro_mem); \
        __coro_suspend: \
            __builtin_coro_end(__coro_hdl, false); \
            return __coro_hdl; \
        } \
    )

/**
 * Create a task from a task handle, a task handle is created by calling the
 * task function for the first time.
 *
 * @param handle    [IN] The handle to the task
 * @param fmt       [IN] The name string (format)
 * @param ...       [IN] The parameters for the format
 */
task_t* create_task(task_handle_t handle, const char* name, ...);

#endif //TOMATOS_TASK_H
