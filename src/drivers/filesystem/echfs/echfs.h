#ifndef TOMATKERNEL_ECHFS_H
#define TOMATKERNEL_ECHFS_H

#include <drivers/filesystem/filesystem.h>
#include <drivers/partition/partition.h>

typedef struct echfs_filesystem {
    filesystem_t super;

    uint64_t allocation_table_lba;
    uint64_t main_directory_lba;
} echfs_filesystem_t;

typedef struct echfs_file {
    file_t super;


} echfs_file_t;

const void* EchfsFilesystem();

const void* EchfsFile();

bool check_echfs(partition_t* partition);

#endif //TOMATKERNEL_ECHFS_H
