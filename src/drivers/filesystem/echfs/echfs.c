#include <util/defs.h>
#include <string.h>
#include "echfs.h"
#include "echfs_spec.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Echfs filesystem implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* Echfs_filesystem_ctor(void* _self, va_list ap) {
    echfs_filesystem_t* self = super_ctor(EchfsFilesystem(), _self, ap);

    echfs_header_t header;
    ASSERT(!IS_ERROR(partition_read_block(self->super.parent, 0, &header, sizeof(echfs_header_t))));

    self->allocation_table_lba = 16;
    self->main_directory_lba = self->allocation_table_lba + ((header.total_block_count * sizeof(uint64_t)) / header.bytes_per_block);

    return self;
}

const void* EchfsFilesystem() {
    static const void* class = NULL;
    if(unlikely(class == NULL)) {
        class = new(FileSystemClass(),
                "EchfsFilesystem", FileSystem(), sizeof(echfs_filesystem_t),
                ctor, Echfs_filesystem_ctor,
                0);
    }
    return class;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Echfs file implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* Echfs_file_ctor(void* _self, va_list ap) {
    echfs_file_t* self = super_ctor(EchfsFile(), _self, ap);
    return self;
}

const void* EchfsFile() {
    static const void* class = NULL;
    if(unlikely(class == NULL)) {
        class = new(FileClass(),
                "EchfsFile", File(), sizeof(echfs_file_t),
                ctor, Echfs_file_ctor,
                0);
    }
    return class;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Echfs functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool check_echfs(partition_t* partition) {
    bool valid = false;

    echfs_header_t header;
    if(IS_ERROR(partition_read_block(partition, 0, &header, sizeof(echfs_header_t)))) goto cleanup;

    if(memcmp(header.signature, "_ECH_FS_", 8) != 0) goto cleanup;
    if(header.bytes_per_block != partition->parent->block_size) goto cleanup;

    valid = true;

cleanup:
    return valid;
}
