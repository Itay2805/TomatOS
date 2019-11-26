#include <util/defs.h>
#include <memory/mm.h>

#include <string.h>
#include <util/arch.h>
#include <drivers/partition/partition.h>

#include "gpt_spec.h"
#include "gpt.h"

bool check_gpt(storage_device_t* storage) {
    error_t err = NO_ERROR;

    ASSERT(storage != NULL);

    // TODO: storage size
    bool valid = false;

    // TODO: locks

    // read the header
    GPT_TABLE_HEADER header = {};
    ASSERT(!IS_ERROR(storage_read(storage, storage->block_size, &header, sizeof(GPT_TABLE_HEADER))));

    if(header.signature != SIGNATURE_64('E','F','I',' ','P','A','R','T')) goto cleanup;
    if(header.revision != 0x00010000) goto cleanup;
    if(header.header_size < 92) goto cleanup;
    if(header.header_size > storage->block_size) goto cleanup;
    if(header._reserved0 != 0) goto cleanup;
    // TODO: CRC32 check

    if(header.my_lba != 1) goto cleanup;
    if(header.first_usable_lba > header.last_usable_lba) goto cleanup;
    if(header.partition_entry_lba >= header.first_usable_lba && header.partition_entry_lba <= header.last_usable_lba) goto cleanup;
    if(header.size_of_partition_entry % 128 != 0) goto cleanup;
    if(header.size_of_partition_entry != sizeof(GPT_PARTITION_ENTRY)) goto cleanup;

    if(header.number_of_partition_entries > UINTPTR_MAX / header.size_of_partition_entry) goto cleanup;
    if((header.number_of_partition_entries * header.size_of_partition_entry) % storage->block_size != 0) goto cleanup;

    if(header.partition_entry_lba + 1 > header.first_usable_lba) goto cleanup;

    valid = true;

cleanup:
    return valid && !IS_ERROR(err);
}

error_t gpt_parse(storage_device_t* storage) {
    error_t err = NO_ERROR;
    GPT_PARTITION_ENTRY* entries = NULL;
    GPT_TABLE_HEADER header = {};

    CHECK_ERROR(storage != NULL, ERROR_INVALID_PARAMETER);
    CHECK_AND_RETHROW(storage_read(storage, 1, &header, sizeof(GPT_TABLE_HEADER)));

    entries = mm_allocate_pages(SIZE_TO_PAGES(sizeof(GPT_PARTITION_ENTRY) * header.number_of_partition_entries));
    CHECK_AND_RETHROW(storage_read(storage, header.partition_entry_lba * storage->block_size, entries, sizeof(GPT_PARTITION_ENTRY) * header.number_of_partition_entries));

    // iterate over all the entries
    for(int i = 0; i < header.number_of_partition_entries; i++) {
        GPT_PARTITION_ENTRY* entry = &entries[i];

        // not an actual entry
        if(entry->unique_partition_guid.low == 0 && entry->unique_partition_guid.high == 0) {
            continue;
        }

        // skip the partition if has these attributes
        if(entry->attributes & (GPT_ATTR_DONT_MOUNT | GPT_ATTR_FOR_LEGACY)) {
            continue;
        }

        // turn the name
        char name[36] = {0};
        for(int j = 0; j < 36; j++) {
            name[j] = entry->partition_name[j] & 0xFF;
        }

        // create the partition and add it to the parent
        partition_t* part = mm_allocate_pool(sizeof(partition_t));
        part->device.type = DEVICE_PARTITION;
        strcpy(part->device.name, name);

        // log it nicely
        debug_log("[+] gpt: added partition `%s`\n", part->device.name);
    }

cleanup:
    if(entries != NULL) {
        mm_free_pages(entries, SIZE_TO_PAGES(storage->block_size));
    }

    return err;
}
