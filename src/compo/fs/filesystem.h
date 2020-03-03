#ifndef __COMPO_FS_FILESYSTEM_H__
#define __COMPO_FS_FILESYSTEM_H__

#include <util/except.h>
#include <stdbool.h>
#include <compo/component.h>

typedef enum seek_type {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
} seek_type_t;

typedef struct file {
    err_t (*eof)(struct file* handle, bool* iseof);
    err_t (*seek)(struct file* handle, size_t offset, seek_type_t type);
    err_t (*tell)(struct file* handle, size_t* size);
    err_t (*read)(struct file* handle, void* buffer, size_t* count);
    err_t (*close)(struct file* handle);
}* file_t;

typedef struct filesystem {
    component_t component;

    err_t (*open)(struct filesystem* comp, const char* path, file_t* handle);
    err_t (*is_readonly)(struct filesystem* comp, bool* ro);
}* filesystem_t;

err_t file_read(file_t file, void* buf, size_t size);
err_t file_seek(file_t file, size_t offset, seek_type_t type);
err_t file_close(file_t file);

#endif //__COMPO_FS_FILESYSTEM_H__
