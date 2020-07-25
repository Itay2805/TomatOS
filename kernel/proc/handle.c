#include <util/except.h>
#include <sync/lock.h>
#include <util/stb_ds.h>
#include <util/defs.h>
#include <stdbool.h>
#include <util/list.h>
#include "handle.h"
#include "process.h"

// TODO: for all of these we can technically assume that the global
//       will never be null, so we should put it as debug or verbose

static err_t create_handle_for_process(process_t* process, handle_meta_t* meta, handle_t* out_handle) {
    err_t err = NO_ERROR;

    CHECK(meta != NULL);
    CHECK(out_handle != NULL);
    CHECK(process != NULL);

    ticket_lock(&process->handles_lock);

    handle_t handle = process->next_handle++;
    hmput(process->handles, handle, meta);
    meta->refcount++;
    *out_handle = handle;

cleanup:
    if (process) {
        ticket_unlock(&process->handles_lock);
    }
    return err;
}

err_t create_handle(handle_meta_t* meta, handle_t* out_handle) {
    err_t err = NO_ERROR;

    CHECK_AND_RETHROW(create_handle_for_process(g_current_process, meta, out_handle));

cleanup:
    return err;
}

err_t copy_handle(handle_t target, handle_t in, handle_t* out) {
    err_t err = NO_ERROR;
    handle_meta_t* proc_meta = NULL;
    handle_meta_t* meta = NULL;
    process_t* proc = NULL;

    // get the process from the handle
    CHECK_AND_RETHROW(get_handle_meta(target, &proc_meta));
    CHECK_ERROR(proc_meta->kind == HANDLE_PROCESS, ERROR_INVALID_HANDLE);
    proc = CR(proc_meta, process_t, handle_meta);

    // get the meta of the in handle
    CHECK_AND_RETHROW(get_handle_meta(in, &meta));
    CHECK_AND_RETHROW(create_handle_for_process(proc, meta, out));

cleanup:
    if (proc_meta != NULL) {
        WARN(IS_ERROR(release_handle_meta(proc_meta)), "Failed to release process handle!");
    }
    if (meta != NULL) {
        WARN(IS_ERROR(release_handle_meta(meta)), "Failed to release handle!");
    }
    return err;
}

err_t get_handle_meta(handle_t handle, handle_meta_t** meta) {
    err_t err = NO_ERROR;
    bool unlock_lock = false;

    CHECK(meta != NULL);
    CHECK(g_current_process != NULL);

    if (handle <= 0) {
        switch (handle) {
            case THIS_PROCESS: {
                g_current_process->handle_meta.refcount++;
                *meta = (handle_meta_t*)g_current_process;
            } break;

            case THIS_THREAD: {
                g_current_thread->handle_meta.refcount++;
                *meta = (handle_meta_t*)g_current_thread;
            } break;

            default: CHECK_FAIL_ERROR(ERROR_INVALID_HANDLE);
        }
    } else {
        ticket_lock(&g_current_process->handles_lock);
        unlock_lock = true;

        // get the meta and make sure it is valid
        int index = hmgeti(g_current_process->handles, handle);
        CHECK_ERROR(index != -1, ERROR_INVALID_HANDLE);
        *meta = g_current_process->handles[index].value;

        // increment the refcount
        (*meta)->refcount++;
    }

cleanup:
    if (unlock_lock) {
        ticket_unlock(&g_current_process->handles_lock);
    }
    return err;
}

err_t release_handle_meta(handle_meta_t* meta) {
    err_t err = NO_ERROR;

    CHECK(meta != NULL);
    if (atomic_fetch_sub(&meta->refcount, 1) - 1 == 0) {
        CHECK(meta->dtor != NULL);
        CHECK_AND_RETHROW(meta->dtor(meta));
    }

cleanup:
    return err;
}

err_t close_handle(handle_t handle) {
    err_t err = NO_ERROR;
    handle_meta_t* meta = NULL;
    bool unlock_lock = false;

    CHECK(meta != NULL);
    CHECK(g_current_process != NULL);

    if (handle <= 0) {
        switch (handle) {
            default: CHECK_FAIL_ERROR(ERROR_INVALID_HANDLE);
        }
    } else {
        ticket_lock(&g_current_process->handles_lock);
        unlock_lock = true;

        // get the meta and make sure it is valid
        int index = hmgeti(g_current_process->handles, handle);
        CHECK_ERROR(index != -1, ERROR_INVALID_HANDLE);
        meta = g_current_process->handles[index].value;

        // decrement the refcount
        meta->refcount--;
        if (meta->refcount == 0) {
            hmdel(g_current_process->handles, handle);
        }

        ticket_unlock(&g_current_process->handles_lock);
        unlock_lock = false;

        if (meta->refcount == 0) {
            CHECK(meta->dtor != NULL);
            CHECK_AND_RETHROW(meta->dtor(meta));
        }
    }

cleanup:
    if (unlock_lock) {
        ticket_unlock(&g_current_process->handles_lock);
    }
    return err;
}