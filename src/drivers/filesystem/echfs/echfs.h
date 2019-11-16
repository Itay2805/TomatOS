#ifndef TOMATKERNEL_ECHFS_H
#define TOMATKERNEL_ECHFS_H

#include <drivers/filesystem/filesystem.h>
#include <drivers/partition/partition.h>

typedef struct echfs_filesystem {
    filesystem_t super;
} echfs_filesystem_t;

const void* EchfsFilesystem();

bool check_echfs(partition_t* partition);

#endif //TOMATKERNEL_ECHFS_H
