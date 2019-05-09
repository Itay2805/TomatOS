#ifndef TOMATKERNEL_ECHFS_H
#define TOMATKERNEL_ECHFS_H

#include <common/stdint.h>

/**
 * The echfs header
 */
typedef struct echfs_header {
    char jmp[3];
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
    uint64_t payload;
    uint64_t size;
} echfs_directory_entry_t;

#endif //TOMATKERNEL_ECHFS_H
