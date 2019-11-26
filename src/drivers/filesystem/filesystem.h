#ifndef TOMATKERNEL_FILESYSTEM_H
#define TOMATKERNEL_FILESYSTEM_H

#include <util/error.h>
#include <util/list.h>
#include <util/sync.h>
#include <drivers/partition/partition.h>

typedef enum file_open_mode {
    FILE_READ = 1u << 0u,
    FILE_WRITE = 1u << 1u,
    FILE_APPEND = 1u << 2u,
} file_open_mode_t;

typedef struct file_stat {
    size_t size;
    uint64_t modification_time;
    uint64_t creation_time;
} file_stat_t;

struct file;

typedef struct filesystem {
    device_t device;

    // the partition the filesystem is in
    partition_t* parent;

    // all handles open for this filesystem
    lock_t files_lock;
    list_entry_t files;

    // the link in the filesystem list
    list_entry_t link;

    // functions
    error_t (*open)(struct filesystem* self, const char* file, file_open_mode_t mode, struct file** handle);
    error_t (*dtor)(struct filesystem* self);
} filesystem_t;

typedef struct file {
    device_t device;

    // the filesystem the file is on
    filesystem_t* parent;

    // the link in the open handles list
    list_entry_t* link;

    // the functions
    error_t (*read)(void* _self, size_t offset, void* buffer, size_t size);
    error_t (*close)(void* _self);
} file_t;

lock_t filesystems_lock;
list_entry_t filesystems;

error_t fs_open(filesystem_t* self, const char* file, file_open_mode_t mode, file_t** handle);

error_t file_read(file_t* self, size_t offset, void* buffer, size_t size);
error_t file_close(file_t* self);

#endif //TOMATKERNEL_FILESYSTEM_H
