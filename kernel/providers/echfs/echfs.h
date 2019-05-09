#ifndef TOMATKERNEL_ECHFS_H
#define TOMATKERNEL_ECHFS_H

#include <common/stdint.h>
#include <common/except.h>
#include <resource/resource.h>

/**
 * The echfs header
 */
typedef struct echfs_header {
    char jmp[4];
    char signature[8];
    uint64_t total_block_count;
    uint64_t main_dir_length;
    uint64_t bytes_per_sector;
} __attribute__((packed)) echfs_header_t;

/**
 * Allocation table values
 */
#define ECHFS_ATAB_FREE         0x0000000000000000
#define ECHFS_ATAB_RESERVED     0xFFFFFFFFFFFFFFF0
#define ECHFS_ATAB_END_OF_CHAIN 0xFFFFFFFFFFFFFFFF

#define ECHFS_DIR_ID_END_OF_DIR 0x0000000000000000
#define ECHFS_DIR_ID_ROOT       0xFFFFFFFFFFFFFFFF
#define ECHFS_DIR_ID_DELETED    0xFFFFFFFFFFFFFFFE

#define ECHFS_OBJECT_TYPE_FILE  0
#define ECHFS_OBJECT_TYPE_DIR  1

typedef struct echfs_directory_entry {
    uint64_t parent_id;
    uint8_t type;
    char name[218];
    uint8_t permissions;
    uint16_t owner;
    uint16_t group;
    uint64_t time;
    union {
        uint64_t dir_id;
        uint64_t data_start;
    };
    uint64_t size;
} __attribute__((packed)) echfs_directory_entry_t;

/**
 * Will resolve the path to return the directory entry
 *
 * @param resource  [IN]    The resource to read from
 * @param path      [IN]    The path to resolve
 * @param base      [OUT]   The directory entry
 */
error_t echfs_resolve_path(resource_t resource, const char* path, echfs_directory_entry_t* entry);

/**
 * Will read the directory
 *
 * @param resource  [IN]        The resource to read from
 * @param dir_id    [IN]        The parent id
 * @param pointer   [IN/OUT]    The pointer to start from
 * @param entry     [OUT]       The entry in the dir
 */
error_t echfs_read_dir(resource_t resource, uint64_t dir_id, uint64_t* pointer, echfs_directory_entry_t* entry);

error_t echfs_read_from_chain(resource_t resource, uint64_t current_block, char* buffer, size_t offset, size_t len, size_t* outLen);

#endif //TOMATKERNEL_ECHFS_H
