#include "filesystem.h"

err_t file_read(file_t file, void* buf, size_t size) {
    err_t err = NO_ERROR;
    size_t read_size = size;

    CHECK_ERROR(file != NULL, ERROR_INVALID_PARAM);

    // allow 0 read because why not
    if (size != 0) {
        CHECK_AND_RETHROW(file->read(file, buf, &read_size));
        CHECK_ERROR(read_size == size, ERROR_OUT_OF_RESOURCES);
    }

cleanup:
    return err;
}

err_t file_seek(file_t file, size_t offset, seek_type_t type) {
    err_t err = NO_ERROR;

    CHECK_ERROR(file != NULL, ERROR_INVALID_PARAM);

    // allow 0 offset because why not
    if (offset != 0) {
        CHECK_AND_RETHROW(file->seek(file, offset, type));
    }

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
