#include "fs.h"

err_t fs_open(filesystem_t fs, const char* path, file_t* handle) {
    err_t err = NO_ERROR;

    CHECK_ERROR(fs != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(path != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(handle != NULL, ERROR_INVALID_PARAM);
    CHECK_AND_RETHROW(fs->open(fs, path, handle));

cleanup:
    return err;
}

err_t fs_is_readonly(filesystem_t fs, bool* ro) {
    err_t err = NO_ERROR;

    CHECK_ERROR(fs != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(ro != NULL, ERROR_INVALID_PARAM);
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

err_t file_close(file_t file) {
    err_t err = NO_ERROR;

    CHECK_ERROR(file != NULL, ERROR_INVALID_PARAM);
    CHECK_AND_RETHROW(file->close(file));

cleanup:
    return err;
}
