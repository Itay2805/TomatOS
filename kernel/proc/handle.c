#include <util/except.h>
#include <sync/lock.h>
#include <util/stb_ds.h>
#include <util/defs.h>
#include <stdbool.h>
#include "handle.h"
#include "process.h"

static handle_meta_t** g_global_handles = NULL;
static ticket_lock_t g_global_handles_lock = INIT_LOCK();

#define LOCAL_HANDLE 0
#define GLOBAL_HANDLE 1
#define SPECIAL_HANDLE 2

err_t create_and_bind_global_handle(handle_meta_t* meta, handle_t* out_handle) {
    err_t err = NO_ERROR;

    CHECK(meta != NULL);
    CHECK(out_handle != NULL);

    ticket_lock(&g_global_handles_lock);



cleanup:
    ticket_unlock(&g_global_handles_lock);
    return err;
}

err_t get_handle_meta(handle_t handle, handle_meta_t** meta) {
    err_t err = NO_ERROR;
    bool unlock_global_handle = false;

    CHECK(meta != NULL);

    uint64_t handle_id = handle & ~(BIT63 | BIT62);
    uint8_t handle_type = handle >> 62;
    CHECK_ERROR(handle_id != 0, ERROR_INVALID_HANDLE);

    switch (handle_type) {
        case LOCAL_HANDLE: {
            CHECK_FAIL_TRACE("TODO");
        } break;

        case GLOBAL_HANDLE: {
            ticket_lock(&g_global_handles_lock);
            unlock_global_handle = true;

            // get the meta and make sure it is valid
            CHECK_ERROR(handle_id < arrlenu(g_global_handles), ERROR_INVALID_HANDLE);
            *meta = g_global_handles[handle_id];
            CHECK_ERROR(*meta != NULL, ERROR_INVALID_HANDLE);

            // increment the refcount
            (*meta)->refcount++;
        } break;

        case SPECIAL_HANDLE: {
            switch (handle_id) {
                case 1: {
                    g_current_process->handle_meta.refcount++;
                    *meta = (handle_meta_t*)g_current_process;
                } break;

                case 2: {
                    g_current_thread->handle_meta.refcount++;
                    *meta = (handle_meta_t*)g_current_thread;
                } break;

                default: CHECK_FAIL_ERROR(ERROR_INVALID_HANDLE);
            }
        } break;

        default: CHECK_FAIL_ERROR(ERROR_INVALID_HANDLE);
    }

cleanup:
    if (unlock_global_handle) {
        ticket_unlock(&g_global_handles_lock);
    }
    return err;
}

err_t release_handle_meta(handle_meta_t* meta) {
    err_t err = NO_ERROR;

    CHECK(meta != NULL);
    meta->refcount--;
    if (meta->refcount == 0) {
        CHECK(meta->dtor != NULL);
        CHECK_AND_RETHROW(meta->dtor(meta));
    }

cleanup:
    return err;
}

err_t close_handle(handle_t handle) {
    err_t err = NO_ERROR;
    handle_meta_t* meta = NULL;
    bool unlock_global_handle = false;

    CHECK(meta != NULL);

    uint64_t handle_id = handle & ~(BIT63 | BIT62);
    uint8_t handle_type = handle >> 62;
    CHECK_ERROR(handle_id != 0, ERROR_INVALID_HANDLE);

    switch (handle_type) {
        case LOCAL_HANDLE: {
            CHECK_FAIL_TRACE("TODO");
        } break;

        case GLOBAL_HANDLE: {
            ticket_lock(&g_global_handles_lock);
            unlock_global_handle = false;

            // get the meta and make sure it is valid
            CHECK_ERROR(handle_id < arrlenu(g_global_handles), ERROR_INVALID_HANDLE);
            meta = g_global_handles[handle_id];
            CHECK_ERROR(meta != NULL, ERROR_INVALID_HANDLE);

            // decrement the refcount
            meta->refcount--;
            if (meta->refcount == 0) {
                g_global_handles[handle_id] = NULL;
            }

            ticket_unlock(&g_global_handles_lock);
            unlock_global_handle = false;

            if (meta->refcount == 0) {
                CHECK(meta->dtor != NULL);
                CHECK_AND_RETHROW(meta->dtor(meta));
            }
        } break;

        case SPECIAL_HANDLE: {
            switch (handle_id) {
                // can't close own process/thread handles
                case 1: CHECK_FAIL_ERROR(ERROR_INVALID_HANDLE);
                case 2: CHECK_FAIL_ERROR(ERROR_INVALID_HANDLE);
                default: CHECK_FAIL_ERROR(ERROR_INVALID_HANDLE);
            }
        } break;

        default: CHECK_FAIL_ERROR(ERROR_INVALID_HANDLE);
    }

cleanup:
    if (unlock_global_handle) {
        ticket_unlock(&g_global_handles_lock);
    }
    return err;
}