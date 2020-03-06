#include <stdbool.h>
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
