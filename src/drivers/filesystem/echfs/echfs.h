#ifndef TOMATKERNEL_ECHFS_H
#define TOMATKERNEL_ECHFS_H

#include <drivers/filesystem/filesystem.h>
#include <drivers/partition/partition.h>

bool check_echfs(partition_t* partition);

#endif //TOMATKERNEL_ECHFS_H
