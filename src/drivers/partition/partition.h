#ifndef TOMATKERNEL_PARTITION_H
#define TOMATKERNEL_PARTITION_H

#include <objects/object.h>
#include <util/list.h>

typedef struct partition {
    object_t super;

    char name[255];

    uint64_t lba_start;
    uint64_t lba_end;

    list_entry_t link;
} partition_t;

const void* Partition();

#endif //TOMATKERNEL_PARTITION_H
