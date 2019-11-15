#ifndef TOMATKERNEL_GPT_SPEC_H
#define TOMATKERNEL_GPT_SPEC_H

#include <stdint.h>

typedef struct GPT_TABLE_HEADER {
    // the head
    uint64_t signature;
    uint32_t revision;
    uint32_t header_size;
    uint32_t crc32;
    uint32_t _reserved0;

    // the partitioning info
    uint64_t my_lba;
    uint64_t alternate_lba;
    uint64_t first_usable_lba;
    uint64_t last_usable_lba;

    // the guid
    struct {
        uint64_t low;
        uint64_t high;
    } disk_guid;

    // entries related
    uint64_t partition_entry_lba;
    uint32_t number_of_partition_entries;
    uint32_t size_of_partition_entry;
    uint32_t partition_entry_array_crc32;
} __attribute__((packed)) GPT_TABLE_HEADER;

#define GPT_ATTR_IMPORTANT (1u << 0u)
#define GPT_ATTR_DONT_MOUNT (1u << 1u)
#define GPT_ATTR_FOR_LEGACY (1u << 2u)

typedef struct GPT_PARTITION_ENTRY {
    struct {
        uint64_t low;
        uint64_t high;
    } partition_type_guid;

    struct {
        uint64_t low;
        uint64_t high;
    } unique_partition_guid;

    uint64_t starting_lba;
    uint64_t ending_lba;

    uint64_t attributes;

    uint16_t partition_name[36];
} __attribute__((packed)) GPT_PARTITION_ENTRY;

#endif //TOMATKERNEL_GPT_SPEC_H
