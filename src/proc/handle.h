#ifndef __PROC_HANDLE_H__
#define __PROC_HANDLE_H__

#include <compo/component.h>
#include <sync/spinlock.h>

#include <stdbool.h>

/******************************************************************************
 * Handles are used for managing userspace references of kernel objects
 ******************************************************************************/

typedef enum handle_type {
    HANDLE_COMPONENT,
    HANDLE_FILE,
    HANDLE_EVENT,
    HANDLE_WORKSPACE,
    HANDLE_WINDOW,
} handle_type_t;

typedef struct handle_data {
    // handle lock
    spinlock_t lock;

    // the type of the handle
    handle_type_t type;

    // the ref count of the handle
    int refcount;

    // the value related to the handle
    void* val;

    // data for specific handles
    union {
        struct {
            bool periodic_timer;
        } event;
    };
} handle_data_t;

typedef handle_data_t* handle_t;

/**
 * This will wrap a user event so it can be used for
 * an external function
 *
 * @param in    [IN]    The handle of the event
 * @param out   [OUT]   The event to be used
 */
err_t wrap_event(handle_t handle, event_t* out);

/**
 * Create a new handle, the user has to be the one filling it
 *
 * @param handle    [OUT] The new handle
 */
err_t create_handle(handle_t* handle);

/**
 * Close the handle
 *
 * @param handle    [IN] The handle to close
 */
err_t close_handle(handle_t handle);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Syscalls
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

err_t sys_close_handle(syscall_context_t* ctx);

#endif //__PROC_HANDLE_H__
