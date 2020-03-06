#ifndef __COMPO_FS_VFS_H__
#define __COMPO_FS_VFS_H__

#include <util/except.h>
#include "fs.h"

/**
 * Opens a file from the VFS
 *
 * If the file is not found in any mount then it will
 * load the file from the primary filesystem
 *
 * @param path      [IN]    The path to open
 * @param handle    [OUT]   The handle of the opened file
 */
err_t vfs_open(const char* path, file_t* handle);

/**
 * Resolves the filesystem where the path leads to
 *
 * @param path      [IN]    The path to resolve
 * @param fs        [OUT]   The filesystem the path points to
 * @param out_path  [OUT]   The pointer to the rest of the path inside that filesystem
 */
err_t vfs_resolve(const char* path, filesystem_t* fs, const char** out_path);

/**
 * Mount the given filesystem on the path
 *
 * @param path      [IN] The path to mount on
 * @param fs        [IN] The filesystem to mount
 */
err_t vfs_mount(const char* path, filesystem_t fs);

#endif //__COMPO_FS_VFS_H__
