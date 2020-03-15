#include <proc/process.h>
#include <proc/sched.h>
#include <proc/handle.h>
#include "fs.h"

err_t fs_open(filesystem_t fs, const char* path, file_t* handle) {
    err_t err = NO_ERROR;

    CHECK_ERROR(fs != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(path != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(handle != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(fs->component.type == COMPONENT_FILESYSTEM, ERROR_INVALID_HANDLE);

    CHECK_AND_RETHROW(fs->open(fs, path, handle));

cleanup:
    return err;
}

err_t fs_is_readonly(filesystem_t fs, bool* ro) {
    err_t err = NO_ERROR;

    CHECK_ERROR(fs != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(ro != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(fs->component.type == COMPONENT_FILESYSTEM, ERROR_INVALID_HANDLE);
    CHECK_AND_RETHROW(fs->is_readonly(fs, ro));

cleanup:
    return err;
}

err_t file_read(file_t file, void* buf, size_t size, size_t* read_size) {
    err_t err = NO_ERROR;

    CHECK_ERROR(file != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(buf != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(size > 0, ERROR_INVALID_PARAM);

    file_io_token_t token = {
        .buffer = buf,
        .buffer_size = size
    };

    CHECK_AND_RETHROW(file_read_ex(file, &token));

    if (read_size != NULL) {
        *read_size = token.buffer_size;
    } else {
        CHECK_ERROR(token.buffer_size == size, ERROR_EOF);
    }

cleanup:
    return err;
}

err_t file_read_ex(file_t file, file_io_token_t* io_token) {
    err_t err = NO_ERROR;
    bool should_close_event = false;

    CHECK_ERROR(file != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(io_token != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(io_token->buffer != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(io_token->buffer_size > 0, ERROR_INVALID_PARAM);

    // if the event is null then create a new event, call the file read
    // and then wait for the event to be performed
    if (io_token->event == NULL) {
        should_close_event = true;
        CHECK_AND_RETHROW(create_event(TPL_APPLICATION, NULL, NULL, &io_token->event));
        CHECK_AND_RETHROW(file->read(file, io_token));
        CHECK_AND_RETHROW(wait_for_event(1, &io_token->event, NULL));

    // otherwise simply call the file read
    } else {
        CHECK_AND_RETHROW(file->read(file, io_token));
    }

cleanup:
    // close the event if need to
    if (should_close_event && io_token->event) {
        WARN(!IS_ERROR(close_event(io_token->event)), "Failed to close io token event");
    }
    return err;
}

err_t file_seek(file_t file, size_t offset, seek_type_t type) {
    err_t err = NO_ERROR;

    CHECK_ERROR(file != NULL, ERROR_INVALID_PARAM);
    CHECK_AND_RETHROW(file->seek(file, offset, type));

cleanup:
    return err;
}

err_t file_tell(file_t file, size_t* offset) {
    err_t err = NO_ERROR;

    CHECK_ERROR(file != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(offset != NULL, ERROR_INVALID_PARAM);
    CHECK_AND_RETHROW(file->tell(file, offset));

cleanup:
    return err;
}

err_t file_close(file_t file) {
    err_t err = NO_ERROR;

    CHECK_ERROR(file != NULL, ERROR_INVALID_PARAM);
    CHECK_AND_RETHROW(file->close(file));

cleanup:
    return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Syscall wrappers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

err_t sys_fs_open(syscall_context_t* ctx) {
    err_t err = NO_ERROR;
    int out_handle = -1;
    file_t f = NULL;
    handle_t fs_handle = NULL;
    handle_t new_handle = NULL;

    //get parameters
    int fs = ctx->arg1;
    const char* path = (void*)ctx->arg2;
    CHECK_AND_RETHROW(verify_string(path));

    // get the handle
    CHECK_AND_RETHROW(get_handle(g_current_thread->parent, fs, &fs_handle));
    CHECK_AND_RETHROW(fs_handle->type == HANDLE_COMPONENT);

    // do it
    CHECK_AND_RETHROW(fs_open((filesystem_t)fs_handle->component.val, path, &f));

    // add the handle
    CHECK_AND_RETHROW(create_handle(&new_handle));
    new_handle->type = HANDLE_FILE;
    new_handle->file.val = f;
    f = NULL;
    CHECK_AND_RETHROW(add_handle(g_current_thread->parent, new_handle, &out_handle));

    // the return value
    ctx->ret_value = out_handle;

cleanup:
    if (fs_handle != NULL) {
        WARN(!IS_ERROR(close_handle(fs_handle)), "Failed to close fs handle");
    }

    if (new_handle != NULL) {
        WARN(!IS_ERROR(close_handle(new_handle)), "Failed to close file handle");
    }

    if (IS_ERROR(err)) {
        if (out_handle > 0) {
            WARN(!IS_ERROR(remove_handle(g_current_thread->parent, out_handle)), "Failed to remove handle");
        }

        if (f != NULL) {
            WARN(!IS_ERROR(file_close(f)), "Failed to close file");
        }
    }
    return err;
}

err_t sys_file_read(syscall_context_t* ctx) {
    err_t err = NO_ERROR;
    handle_t handle = NULL;

    // TODO: convert this to an io
    int user_handle = ctx->arg1;
    void* buffer = (void*)ctx->arg2;
    size_t size = ctx->arg3;
    CHECK_AND_RETHROW(verify_buffer(buffer, size));

    // get the handle
    CHECK_AND_RETHROW(get_handle(g_current_thread->parent, user_handle, &handle));
    spinlock_acquire(&handle->lock);
    CHECK_ERROR(handle->type == HANDLE_FILE, ERROR_INVALID_HANDLE);
    file_t file = handle->file.val;

    size_t ret = 0;
    CHECK_AND_RETHROW(file_read(file, buffer, size, &ret));

    ctx->ret_value = ret;

cleanup:
    if (handle != NULL) {
        spinlock_release(&handle->lock);
        WARN(!IS_ERROR(close_handle(handle)), "Failed to close handle");
    }

    return err;
}

err_t sys_file_seek(syscall_context_t* ctx) {
    err_t err = NO_ERROR;



cleanup:
    return err;
}

err_t sys_file_tell(syscall_context_t* ctx) {
    err_t err = NO_ERROR;
    size_t offset = 0;
    handle_t handle = NULL;

    int user_handle = ctx->arg1;

    CHECK_AND_RETHROW(get_handle(g_current_thread->parent, user_handle, &handle));
    CHECK_ERROR(handle->type == HANDLE_FILE, ERROR_INVALID_HANDLE);
    CHECK_AND_RETHROW(file_tell(handle->file.val, &offset));

    ctx->ret_value = offset;

cleanup:
    if (handle != NULL) {
        WARN(!IS_ERROR(close_handle(handle)), "Failed to close handle");
    }
    return err;
}
