#include <memory/mm.h>
#include <string.h>
#include "gpt.h"
#include "gpt_spec.h"

bool check_gpt(storage_device_t* storage) {
    size_t block_size = storage_get_block_size(storage);
    // TODO: storage size
    bool valid = false;

    // read the header
    void* buffer = mm_allocate_pages(storage_get_block_size(storage));
    GPT_TABLE_HEADER* header = buffer;
    storage_read_block(storage, 1, buffer);

    if(memcpy(&header->signature, "EFI PART", 8) != 0) goto cleanup;
    if(header->revision != 0x00010000) goto cleanup;
    if(header->header_size < 92) goto cleanup;
    if(header->header_size > block_size) goto cleanup;
    if(header->_reserved0 != 0) goto cleanup;
    // TODO: CRC32 check

    if(header->my_lba != 1) goto cleanup;
    if(header->first_usable_lba > header->last_usable_lba) goto cleanup;
    if(header->partition_entry_lba >= header->first_usable_lba) goto cleanup;
    if(header->partition_entry_lba <= header->last_usable_lba) goto cleanup;
    if(header->size_of_partition_entry % 128 != 0) goto cleanup;
    if(header->size_of_partition_entry != sizeof(GPT_PARTITION_ENTRY)) goto cleanup;

    if(header->number_of_partition_entries > UINTPTR_MAX / header->size_of_partition_entry) goto cleanup;
    if((header->number_of_partition_entries * header->size_of_partition_entry) % block_size != 0) goto cleanup;

    if(header->partition_entry_lba + 1 > header->first_usable_lba) goto cleanup;

    valid = true;

cleanup:
    mm_free_pages(buffer, block_size);
    return valid;
}
