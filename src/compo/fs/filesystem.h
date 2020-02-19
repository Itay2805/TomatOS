#ifndef __COMPO_FS_FILESYSTEM_H__
#define __COMPO_FS_FILESYSTEM_H__

#include <util/except.h>
#include <stdbool.h>

typedef void* file_t;

struct component;

typedef enum seek_type {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
} seek_type_t;

typedef struct filesystem_interface {
    err_t (*open)(struct component* comp, const char* path, file_t* handle);
    err_t (*is_readonly)(struct component* comp, bool* ro);

    // file handle related
    err_t (*eof)(struct component* comp, file_t handle, bool* iseof);
    err_t (*seek)(struct component* comp, file_t handle, size_t offset, seek_type_t type);
    err_t (*tell)(struct component* comp, file_t handle, size_t* size);
    err_t (*read)(struct component* comp, file_t handle, void* buffer, size_t* count);
    err_t (*close)(struct component* comp, file_t handle);
} filesystem_interface_t;

#endif //__COMPO_FS_FILESYSTEM_H__
