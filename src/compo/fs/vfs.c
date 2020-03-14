#include <stdbool.h>
#include <proc/process.h>
#include <proc/sched.h>
#include <proc/handle.h>
#include "vfs.h"

err_t vfs_open(const char* path, file_t* handle) {
    err_t err = NO_ERROR;
    filesystem_t fs = NULL;

    CHECK_AND_RETHROW(vfs_resolve(path, &fs, &path));
    CHECK_AND_RETHROW(fs_open(fs, path, handle));

cleanup:
    if (fs != NULL) {
        WARN(!IS_ERROR(release_component(fs)), "Failed to release component");
    }
    return err;
}

err_t vfs_resolve(const char* path, filesystem_t* fs, const char** out_path) {
    err_t err = NO_ERROR;

    CHECK_ERROR(path != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(fs != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(out_path != NULL, ERROR_INVALID_PARAM);

    if (path[0] == '/') {
        // if starting with / then use the primary filesystem without routing
        // when setting the out path skip on the first /
        CHECK_AND_RETHROW(get_primary(COMPONENT_FILESYSTEM, (component_t**)fs));
        *out_path = path + 1;

    } else {
        // otherwise do proper routing
        // TODO: properly search for the correct mount
        ASSERT(false);
    }

cleanup:
    return err;
}

/**
 * Mount the given filesystem on the path
 *
 * @param path      [IN] The path to mount on
 * @param fs        [IN] The filesystem to mount
 */
err_t vfs_mount(const char* path, filesystem_t fs) {
    ASSERT(false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Syscall wrappers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

err_t sys_vfs_resolve(syscall_context_t* ctx) {
    err_t err = NO_ERROR;
    filesystem_t fs = NULL;
    handle_t handle = NULL;
    int out_handle = -1;

    // get arguments
    const char* path = (void*)ctx->arg1;
    const char** out_path = (void*)ctx->arg2;

    // verify arguments
    CHECK_AND_RETHROW(verify_string(path));
    CHECK_AND_RETHROW(verify_buffer(out_path, sizeof(*out_path), true));

    // will resolve the path
    CHECK_AND_RETHROW(vfs_resolve(path, &fs, out_path));

    // create the handle and add to the process
    CHECK_AND_RETHROW(create_handle(&handle));
    handle->type = HANDLE_COMPONENT;
    handle->component.val = (void*)fs;
    CHECK_AND_RETHROW(add_handle(g_current_thread->parent, handle, &out_handle));

    // will return the handle
    ctx->ret_value = out_handle;

cleanup:
    // forget it if we are not longer using it
    if (fs != NULL) {
        release_component(fs);
    }

    if (handle != NULL) {
        WARN(!IS_ERROR(close_handle(handle)), "Failed to close handle");
    }

    if (IS_ERROR(err)) {
        if (out_handle > 0) {
            WARN(!IS_ERROR(remove_handle(g_current_thread->parent, out_handle)), "Failed to remove handle from process");
        }
    }

    return err;
}

err_t sys_vfs_mount(syscall_context_t* ctx) {
    err_t err = NO_ERROR;

    CHECK_FAIL_ERROR(ERROR_NOT_READY);

cleanup:
    return err;
}
