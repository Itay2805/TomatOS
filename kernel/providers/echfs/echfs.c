#include <common/klib.h>
#include <common/string.h>
#include "echfs.h"

static error_t echfs_read_header(resource_t resource, echfs_header_t* header) {
    error_t err = NO_ERROR;

    // read the header and check the signature
    CHECK(seek(resource, SEEK_START, 0));
    CHECK(read(resource, header, sizeof(header), NULL));

cleanup:
    return err;
}


static error_t echfs_find_file(resource_t resource, echfs_header_t* header, uint64_t parent_id, const char* name, echfs_directory_entry_t* output_entry) {
    error_t err = NO_ERROR;
    uint64_t start = header->bytes_per_sector * 16 + (header->total_block_count * sizeof(uint64_t));
    echfs_directory_entry_t entry = {0};
    bool found = false;

    CHECK(seek(resource, SEEK_START, start));
    for(int i = 0; i < header->main_dir_length; i++) {
        CHECK(read(resource, &entry, sizeof(entry), NULL));
        if(entry.parent_id == parent_id && strcmp(entry.name, name) == 0) {
            found = true;
            break;
        }
    }

    CHECK_ERROR(found, ERROR_NOT_FOUND);
    *output_entry = entry;

cleanup:
    return err;
}

error_t echfs_resolve_path(resource_t resource, const char* path, echfs_directory_entry_t* entry) {
    error_t err = NO_ERROR;
    echfs_header_t header;
    char* part = NULL;
    char buffer[218];

    CHECK_AND_RETHROW(echfs_read_header(resource, &header));

    uint64_t current_parent_id = ECHFS_DIR_ID_ROOT;
    echfs_directory_entry_t current_entry = {0};
    do {
        // get the next entry
        char* next = strchr(part++, '/');

        // copy the name to the buffer
        if(next == NULL) {
            // this is the last entry
            size_t len = strlen(part);
            CHECK(len <= 217);
            memcpy(buffer, part, len);
            buffer[len] = '\0';
        }else {
            CHECK(next - part <= 217);
            memcpy(buffer, part, next - part);
            buffer[next - part] = '\0';
        }

        // find the entry
        CHECK_AND_RETHROW(echfs_find_file(resource, &header, current_parent_id, buffer, &current_entry));

        if(next == NULL) {
            // last entry in the path should be a file
            CHECK_ERROR(current_entry.type == ECHFS_OBJECT_TYPE_FILE, ERROR_INVALID_PATH);
        }else {
            // any other entry in the path should be a folder
            CHECK_ERROR(current_entry.type == ECHFS_OBJECT_TYPE_DIR, ERROR_INVALID_PATH);
        }
    }
    while((part = strchr(path, '/')) != NULL);

    *entry = current_entry;

cleanup:
    return err;
}
