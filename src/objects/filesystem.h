#ifndef TOMATKERNEL_FILESYSTEM_H
#define TOMATKERNEL_FILESYSTEM_H

#include <common/error.h>
#include "object.h"

typedef enum filesystem_syscalls {
    // file related
    FILESYSTEM_SYS_OPEN,
    FILESYSTEM_SYS_READ,
    FILESYSTEM_SYS_WRITE,
    FILESYSTEM_SYS_CLOSE,

    // directory related
    FILESYSTEM_SYS_MKDIR,
    FILESYSTEM_SYS_READDIR,

    // other
    FILESYSTEM_SYS_STAT,

    FILESYSTEM_SYS_MAX
} filesystem_syscalls_t;

typedef enum dir_ent_type {
    DIR_ENT_TYPE_FILE,
    DIR_ENT_TYPE_DIR,
} dir_ent_type_t;

typedef struct __attribute__((packed)) dir_ent {
    // entry name
    char name[256];

    // permissions
    uint8_t read : 1;
    uint8_t write : 1;
    uint8_t exec : 1;

    // specific for type
    dir_ent_type_t type;

    // only valid for file
    size_t size;
} dir_ent_t;

typedef struct filesystem_functions {
    error_t (*open)(object_t* obj, int* fd, const char* path);
    error_t (*read)(object_t* obj, int fd, void* buffer, size_t offset, size_t size);
    error_t (*write)(object_t* obj, int fd, void* buffer, size_t offset, size_t size);
    error_t (*close)(object_t* obj, int fd);

    error_t (*mkdir)(object_t* obj, const char* path);
    error_t (*readdir)(object_t* obj, int* dd, dir_ent_t* entry);

    error_t (*stat)(object_t* obj, const char* path, dir_ent_t* entry);
} filesystem_functions_t;

#endif //TOMATKERNEL_FILESYSTEM_H
