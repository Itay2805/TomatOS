#include <util/defs.h>
#include <driver/driver.h>
#include <driver/pci/pci_spec.h>
#include <mem/pmm.h>

typedef struct gpt_table_header {
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
} PACKED gpt_table_header_t;

#define GPT_ATTR_IMPORTANT (1u << 0u)
#define GPT_ATTR_DONT_MOUNT (1u << 1u)
#define GPT_ATTR_FOR_LEGACY (1u << 2u)

typedef struct gpt_entry {
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
} PACKED gpt_entry_t;

static err_t gpt_connect(driver_instance_t* interface) {
    TRACE("Connected to %s", interface->name);
    return NO_ERROR;
}

static bool gpt_check(driver_instance_t* interface) {
    bool result = false;
    gpt_table_header_t* header = pmalloc(interface->block.block_size);
    ASSERT(!IS_ERROR(interface->block.read_blocks(interface, 1, header, interface->block.block_size)));

    // Check the header
    if (header->signature != SIGNATURE_64("EFI PART")) goto cleanup;
    if (header->revision != 0x00010000) goto cleanup;
    if (header->header_size < 92) goto cleanup;
    if (header->header_size > interface->block.last_block * interface->block.block_size) goto cleanup;

    // TODO: sanity check header values

    result = true;

cleanup:
    pmfree(header, interface->block.block_size);
    return result;
}

static err_t gpt_entry(driver_bind_data_t* data) {
    TRACE("Got bind data");
    return true;
}

// this needs to be higher priority than mbr (because
// of protective mbr)
PRIORITY_DRIVER {
    .name = "GPT",
    .entry = gpt_entry,
    .interface = {
        .type = DRIVER_BLOCK,
    },
    .binds = (driver_bind_t[]) {
        {
            .type = BIND_INTERFACE,
            .interface = {
                .type = DRIVER_BLOCK,
                .connect = gpt_connect,
                .check = gpt_check,
            }
        },
        {BIND_END }
    }
};