#ifndef TOMATKERNEL_ECHFS_SPEC_H
#define TOMATKERNEL_ECHFS_SPEC_H

#include <stddef.h>

typedef struct echfs_header {
    char _reserved0[4];
    char signature[8];
    uint64_t total_block_count;
    uint64_t main_directory_length;
    uint64_t bytes_per_block;
} __attribute__((packed)) echfs_header_t;

#define ECHFS_BLOCK_FREE                0x0000000000000000
#define ECHFS_BLOCK_RESERVED            0xFFFFFFFFFFFFFFF0
#define ECHFS_BLOCK_EOC                 0xFFFFFFFFFFFFFFFF

#define ECHFS_ENTRY_END_OF_DIRECTORY    0
#define ECHFS_ENTRY_ROOT                0xFFFFFFFFFFFFFFFF
#define ECHFS_ENTRY_DELETED             0xFFFFFFFFFFFFFFFE

#define ECHFS_TYPE_FILE                 0
#define ECHFS_TYPE_DIR                  1

typedef struct echfs_entry {
    uint64_t parent_id;
    uint8_t type;
    uint8_t name[201];
    uint64_t atime;
    uint64_t mtime;
    uint16_t perms;
    uint16_t owner_id;
    uint16_t group_id;
    uint64_t ctime;
    uint64_t payload;
    uint64_t file_size;
} __attribute__((packed)) echfs_entry_t;

#endif //TOMATKERNEL_ECHFS_SPEC_H
