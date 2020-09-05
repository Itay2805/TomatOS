#include <util/defs.h>
#include <driver/driver.h>
#include <driver/pci/pci_spec.h>
#include <mem/pmm.h>

typedef struct mbr_entry {
    uint8_t status;
    uint8_t chs_first_sect[3];
    uint8_t type;
    uint8_t chs_last_sect[3];
    uint32_t first_sect;
    uint32_t sect_count;
} PACKED mbr_entry_t;

static err_t mbr_connect(driver_instance_t* interface) {
    return NO_ERROR;
}

static bool mbr_check(driver_instance_t* interface) {
    bool result = false;
    uint8_t* mbr = pmalloc(interface->block.block_size);
    ASSERT(!IS_ERROR(interface->block.read_blocks(interface, 1, mbr, interface->block.block_size)));

    // Check the header
    if (RELAXED_POKE16(mbr + 444) != 0x5a5a && RELAXED_POKE16(mbr + 444) != 0) goto cleanup;


    result = true;

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
