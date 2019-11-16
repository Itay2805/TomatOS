#include <util/defs.h>
#include <string.h>
#include "echfs.h"
#include "echfs_spec.h"

static void* Echfs_ctor(void* _self, va_list ap) {
    echfs_filesystem_t* self = super_ctor(EchfsFilesystem(), _self, ap);
    return self;
}

const void* EchfsFilesystem() {
    static const void* class = NULL;
    if(unlikely(class == NULL)) {
        class = new(FileSystemClass(),
                "EchfsFilesystem", FileSystem(), sizeof(echfs_filesystem_t),
                ctor, Echfs_ctor,
                0);
    }
    return class;
}

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
