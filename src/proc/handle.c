#include <mm/mm.h>
#include "handle.h"
#include "sched.h"

static err_t wrapped_event_notify_function(handle_t handle, event_t event) {
    err_t err = NO_ERROR;

    // signal the user event and close the handle
    CHECK_AND_RETHROW(signal_event(handle->event.val));
    CHECK_AND_RETHROW(close_handle(handle));

cleanup:
    return err;
}

err_t wrap_event(handle_t handle, event_t* out) {
    err_t err = NO_ERROR;

    CHECK_ERROR(handle != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(out != NULL, ERROR_INVALID_PARAM);

    spinlock_acquire(&handle->lock);

    // make sure this is an event handle
    CHECK_ERROR(handle->type == HANDLE_EVENT, ERROR_INVALID_HANDLE);
    event_t event = handle->event.val;

    // increase the ref count and create the event
    handle->refcount++;
    CHECK_AND_RETHROW(create_event(event->notify_tpl, (void*)wrapped_event_notify_function, handle, out));

cleanup:
    if (handle != NULL) {
        spinlock_release(&handle->lock);
    }

    return err;
}

err_t create_handle(handle_t* out_handle) {
    err_t err = NO_ERROR;
    handle_data_t* data = NULL;

    CHECK_ERROR(out_handle != NULL, ERROR_INVALID_PARAM);

    data = mm_allocate(sizeof(handle_data_t));
    data->lock = SPINLOCK_INIT;
    data->refcount = 1;

    *out_handle = data;

cleanup:
    return err;
}

err_t close_handle(handle_t handle) {
    err_t err = NO_ERROR;

    CHECK_ERROR(handle != NULL, ERROR_INVALID_PARAM);

    spinlock_acquire(&handle->lock);

    // one less reference
    handle->refcount--;

    // if reached to zero references then delete it
    if (handle->refcount == 0) {
        switch (handle->type) {
            case HANDLE_COMPONENT: {
                // release the component handle
                CHECK_AND_RETHROW(release_component(handle->component.val));
            } break;

            case HANDLE_FILE: {
                // close the file
                TRACE("CLOSING FILE");
                CHECK_AND_RETHROW(file_close(handle->file.val));
            } break;

            case HANDLE_EVENT: {
                // if this is a periodic timer cancel it
                if (handle->event.periodic_timer) {
                    CHECK_AND_RETHROW(set_timer(handle->event.val, TIMER_CANCEL, 0));
                }

                // close the event
                CHECK_AND_RETHROW(close_event(handle->event.val));
            } break;
        }

        // free the handle
        mm_free(handle);
    }

cleanup:
    if (handle != NULL) {
        spinlock_release(&handle->lock);
    }

    return err;
}

err_t sys_close_handle(syscall_context_t* ctx) {
    err_t err = NO_ERROR;

    int handle = ctx->arg1;
    CHECK_AND_RETHROW(remove_handle(get_current_process(), handle));

cleanup:
    return err;
}
