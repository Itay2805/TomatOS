#ifndef TOMATKERNEL_FILESYSTEM_H
#define TOMATKERNEL_FILESYSTEM_H

#include <objects/object.h>
#include <util/error.h>
#include <util/list.h>
#include <util/sync.h>
#include <drivers/partition/partition.h>

typedef struct file_stat {
    size_t size;
    uint64_t modification_time;
    uint64_t creation_time;
} file_stat_t;

typedef struct filesystem_class {
    class_t _;

    error_t (*open)(void* _self, const char* path, void** filehandle);
    error_t (*stat)(void* _self, const char* path, file_stat_t* state);
} filesystem_class_t;

typedef struct file_class {
    class_t _;

    error_t (*read)(void* _self, size_t offset, void* buffer, size_t size);
    error_t (*close)(void* _self);
} file_class_t;

typedef struct filesystem {
    object_t super;

    // the partition the filesystem is in
    partition_t* parent;

    // all handles open for this filesystem
    lock_t files_lock;
    list_entry_t files;

    // the link in the filesystem list
    list_entry_t link;
} filesystem_t;

typedef struct file {
    object_t super;

    // the filesystem the file is on
    filesystem_t* parent;

    // the link in the open handles list
    list_entry_t* link;
} file_t;

const void* FileSystemClass();

const void* FileClass();

const void* FileSystem();

const void* File();

lock_t filesystems_lock;
list_entry_t filesystems;

error_t fs_open(void* _self, const char* path, void** filehandle);
error_t fs_stat(void* _self, const char* path, file_stat_t* state);

error_t file_read(void* _self, size_t offset, void* buffer, size_t size);
error_t file_close(void* _self);

#endif //TOMATKERNEL_FILESYSTEM_H
