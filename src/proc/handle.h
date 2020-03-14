#ifndef __PROC_HANDLE_H__
#define __PROC_HANDLE_H__

#include <sync/spinlock.h>
#include <stdbool.h>
#include <compo/component.h>
#include <compo/fs/fs.h>

/******************************************************************************
 * Handles are used for managing userspace references of kernel objects
 ******************************************************************************/

typedef enum handle_type {
    HANDLE_COMPONENT,

    HANDLE_FILE,

    HANDLE_EVENT,
} handle_type_t;

typedef struct handle_data {
    spinlock_t lock;

    handle_type_t type;

    int refcount;

    // data for specific handles
    union {
        struct {
            component_t* val;
        } component;

        struct {
            file_t val;
        } file;

        struct {
            event_t val;
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

#endif //__PROC_HANDLE_H__
