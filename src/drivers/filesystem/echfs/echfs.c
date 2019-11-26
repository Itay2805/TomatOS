#include <util/defs.h>
#include <string.h>
#include <memory/mm.h>
#include "echfs.h"
#include "echfs_spec.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Echfs filesystem implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct echfs_filesystem {
    filesystem_t filesystem;

    uint64_t allocation_table_lba;
    uint64_t main_directory_lba;
} echfs_filesystem_t;

__attribute__((unused))
static filesystem_t* new_echfs_filesystem(partition_t* partition) {
    echfs_header_t header;
    ASSERT(!IS_ERROR(partition_read(partition, 0, &header, sizeof(echfs_header_t))));

    echfs_filesystem_t* fs = mm_allocate_pool(sizeof(echfs_filesystem_t));

    fs->allocation_table_lba = 16;
    fs->main_directory_lba = fs->allocation_table_lba + ((header.total_block_count * sizeof(uint64_t)) / header.bytes_per_block);

    return (filesystem_t*)fs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Echfs file implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct echfs_file {
    int dummy;
} echfs_file_t;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Echfs functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool check_echfs(partition_t* partition) {
    bool valid = false;

    echfs_header_t header;
    if(IS_ERROR(partition_read(partition, 0, &header, sizeof(echfs_header_t)))) goto cleanup;

    if(memcmp(header.signature, "_ECH_FS_", 8) != 0) goto cleanup;
    if(header.bytes_per_block != partition->parent->block_size) goto cleanup;

    valid = true;

cleanup:
    return valid;
}
