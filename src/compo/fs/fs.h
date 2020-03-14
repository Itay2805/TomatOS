#ifndef __COMPO_FS_FILESYSTEM_H__
#define __COMPO_FS_FILESYSTEM_H__

#include <util/except.h>
#include <stdbool.h>
#include <compo/component.h>
#include <stdatomic.h>
#include <event/event.h>
#include <proc/syscall.h>

/**
 * These are used for the seek function
 */
typedef enum seek_type {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
} seek_type_t;

/**
 * This is used for async read/write
 */
typedef struct file_io_token {
    // the event, if null perform blocking io
    // otherwise perform non-blocking io
    event_t event;

    // the output error, if performs blocking io this will be
    // returned by the function
    err_t error;

    // the size of the buffer to read
    size_t buffer_size;

    // the buffer to read to
    void* buffer;
} file_io_token_t;

/**
 * This is the file interface
 *
 * @remark
 * Do not call these functions directly, since they might expect different
 * stuff than the common syscall
 */
typedef struct file {
    err_t (*eof)(struct file* handle, bool* iseof);
    err_t (*seek)(struct file* handle, size_t offset, seek_type_t type);
    err_t (*tell)(struct file* handle, size_t* size);
    err_t (*read)(struct file* handle, file_io_token_t* io_token);
    err_t (*close)(struct file* handle);
}* file_t;

/**
 * This is the filesystem component interface
 */
typedef struct filesystem {
    component_t component;

    err_t (*open)(struct filesystem* comp, const char* path, file_t* handle);
    err_t (*is_readonly)(struct filesystem* comp, bool* ro);
}* filesystem_t;

/**
 * Open a file from a certain filesystem
 *
 * @param fs        [IN]    The filesystem to open from
 * @param path      [IN]    The path to open
 * @param handle    [OUT]   The new handle
 */
err_t fs_open(filesystem_t fs, const char* path, file_t* handle);

/**
 * Check if the filesystem is read only
 *
 * @param fs        [IN]    The filesystem to check if is read only
 * @param ro        [OUT]   Is the filesystem read only
 */
err_t fs_is_readonly(filesystem_t fs, bool* ro);

/**
 * Read from an open file handle
 *
 * @remark
 * This will perform blocking io
 *
 * @param file          [IN]    The file we are reading
 * @param buf           [IN]    The buffer to read to
 * @param size          [IN]    The amount of bytes to read
 * @param read_size     [OUT]   The read size, if NULL will error on in-sufficient read and seek back
 */
err_t file_read(file_t file, void* buf, size_t size, size_t* read_size);

/**
 * A more advanced file read
 *
 * This can perform async io
 *
 * @param file          [IN] The file to read from
 * @param io_token      [IN] The io token
 */
err_t file_read_ex(file_t file, file_io_token_t* io_token);

/**
 * Seek into a file
 *
 * @param file      [IN] The file to seek
 * @param offset    [IN] The offset to seek
 * @param type      [IN] The seek type
 */
err_t file_seek(file_t file, size_t offset, seek_type_t type);

/**
 * Get the current offset in the file
 *
 * @param file      [IN]    The file to get the offset of
 * @param offset    [OUT]   The output offset
 */
err_t file_tell(file_t file, size_t* offset);

/**
 * Close the file
 *
 * @remark
 * Using the file after free can lead to serious problems!
 *
 * @param file  [IN] The file to close
 */
err_t file_close(file_t file);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Syscall wrappers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

err_t sys_fs_open(syscall_context_t* ctx);

#endif //__COMPO_FS_FILESYSTEM_H__
