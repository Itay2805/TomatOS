#include <driver/pci/pci_spec.h>
#include <driver/driver.h>
#include <util/string.h>
#include <util/defs.h>
#include <mem/pmm.h>
#include <mem/mm.h>

#undef __MODULE__
#define __MODULE__ "mbr"

typedef struct mbr_entry {
    uint8_t boot_indicator;
    uint8_t start_head;
    uint8_t start_sector;
    uint8_t start_track;
    uint8_t os_indicator;
    uint8_t end_head;
    uint8_t end_sector;
    uint8_t end_track;
    uint32_t starting_lba;
    uint32_t size_in_lba;
} PACKED mbr_entry_t;

typedef struct mbr {
    uint8_t code[440];
    uint32_t uid;
    uint16_t unknown;
    mbr_entry_t entries[4];
    uint16_t signature;
} PACKED mbr_t;

typedef struct mbr_partition {
    driver_instance_t instance;
    size_t base;
    driver_instance_t* block;
} mbr_partition_t;

static err_t do_read(driver_instance_t* instance, size_t lba, void* buffer, size_t buffer_size) {
    err_t err = NO_ERROR;
    mbr_partition_t* part = CR(instance, mbr_partition_t, instance);

    CHECK(lba <= part->instance.block.last_block);
    CHECK_AND_RETHROW(part->block->block.read_blocks(part->block, part->base + lba, buffer, buffer_size));

cleanup:
    return err;
}

static err_t do_write(driver_instance_t* instance, size_t lba, void* buffer, size_t buffer_size) {
    err_t err = NO_ERROR;
    mbr_partition_t* part = CR(instance, mbr_partition_t, instance);
    CHECK_AND_RETHROW(part->block->block.write_blocks(part->block, part->base + lba, buffer, buffer_size));

cleanup:
    return err;
}

static err_t delete_mbr(handle_meta_t* meta) {
    err_t err = NO_ERROR;

    mbr_partition_t* part = CR(meta, mbr_partition_t, instance.meta);

    // release the handle
    CHECK_AND_RETHROW(release_handle_meta(&part->block->meta));

cleanup:
    kfree(part);
    return err;
}

static err_t mbr_connect(driver_instance_t* interface) {
    err_t err = NO_ERROR;
    mbr_t* mbr = pmalloc(interface->block.block_size);

    CHECK_AND_RETHROW(interface->block.read_blocks(interface, 0, mbr, interface->block.block_size));

    // make sure there are actually partitions
    for (int i = 0; i < 4; i++) {
        if (mbr->entries[i].os_indicator == 0 || mbr->entries[i].size_in_lba == 0) {
            continue;
        }

        // take reference to the block
        interface->meta.refcount++;

        // create the object
        mbr_partition_t* new_part = kalloc(sizeof(mbr_partition_t));
        new_part->instance.meta.refcount = 1;
        new_part->instance.meta.dtor = delete_mbr;
        new_part->instance.meta.kind = HANDLE_DRIVER;
        new_part->instance.type = DRIVER_BLOCK;
        new_part->instance.block.last_block = mbr->entries[i].size_in_lba;
        new_part->instance.block.block_size = interface->block.block_size;
        new_part->instance.block.read_blocks = do_read;
        new_part->instance.block.write_blocks = do_write;
        new_part->instance.block.flush_blocks = interface->block.flush_blocks;
        snprintf(new_part->instance.name, sizeof(new_part->instance.name), "mbr #%d", i);
        new_part->block = interface;
        new_part->base = mbr->entries[i].starting_lba;
        CHECK_AND_RETHROW(register_interface(&new_part->instance));
    }

cleanup:
    pmfree(mbr, interface->block.block_size);
    return err;
}

static bool mbr_check(driver_instance_t* interface) {
    bool result = false;
    mbr_t* mbr = pmalloc(interface->block.block_size);
    ASSERT(!IS_ERROR(interface->block.read_blocks(interface, 0, mbr, interface->block.block_size)));

    // Check the header
    if (mbr->signature != 0xAA55) goto cleanup;

    // make sure there are actually partitions
    for (int i = 0; i < 4; i++) {
        if (mbr->entries[i].os_indicator != 0 && mbr->entries[i].size_in_lba != 0) {
            result = true;
            break;
        }
    }

cleanup:
    pmfree(mbr, interface->block.block_size);
    return result;
}

static err_t mbr_entry(driver_bind_data_t* data) {
    return true;
}

DRIVER {
    .name = "MBR",
    .entry = mbr_entry,
    .interface = {
        .type = DRIVER_BLOCK,
    },
    .binds = (driver_bind_t[]) {
        {
            .type = BIND_INTERFACE,
            .interface = {
                .type = DRIVER_BLOCK,
                .connect = mbr_connect,
                .check = mbr_check,
            }
        },
        {BIND_END }
    }
};
