#ifndef TOMATOS_TASK_H
#define TOMATOS_TASK_H

#include <mem/mm.h>
#include <cont/list.h>
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

typedef void* task_handle_t;

#define task_resume(task) __builtin_coro_resume(task)
#define task_done(task) __builtin_coro_done(task)
#define task_destroy(task) __builtin_coro_destroy(task)
#define task_handle(task) (task_handle_t)__builtin_coro_promise(task, 0, true)

#define task(name, sig, ...) \
    IFE(__VA_ARGS__)( \
        task_t name sig; \
    ) \
    IFN(__VA_ARGS__)( \
        task_t name sig { \
            void* mem; \
            __builtin_coro_id(0, &((char[sizeof(task_t)]){}), NULL, NULL); \
            void* alloc = NULL; \
            if (__builtin_coro_alloc()) { \
                alloc = kalloc(__builtin_coro_size()); \
                if (alloc == NULL) { \
                    return NULL; \
                }\
            } \
            void* __coro_hdl = __builtin_coro_begin(alloc); \
            yield(); \
            { \
                __VA_ARGS__ \
            } \
        __coro_final: \
            switch (__builtin_coro_suspend(true)) { \
                case 0: __builtin_trap(); \
                case 1: goto __coro_cleanup; \
                default: goto __coro_suspend; \
            } \
        __coro_cleanup: \
            mem = __builtin_coro_free(__coro_hdl); \
            kfree(mem); \
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
