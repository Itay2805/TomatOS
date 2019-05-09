#ifndef TOMATKERNEL_ATA_H
#define TOMATKERNEL_ATA_H

#include <common/stdbool.h>

// TODO: This is just a temp pio driver, eventually we will have a better one

#define ATA_MASTER_IO_BASE 0x1F0
#define ATA_MASTER_CONTROL_BASE 0x3F6

#define ATA_SLAVE_IO_BASE 0x170
#define ATA_SLAVE_CONTROL_BASE 0x376

#define ATA_DATA 0
#define ATA_ERROR 1
#define ATA_SECTOR_COUNT 2
#define ATA_LBA_LO 3
#define ATA_LBA_MID 4
#define ATA_LBA_HI 5
#define ATA_DRIVE 6
#define ATA_COMMAND 7
#define ATA_STATUS 7

#define ATA_CONTROL 0
#define ATA_DRIVE_SELECT 1

typedef struct ata_identify {
    uint16_t always_0 : 1;
    uint16_t hard_sectored : 1;
    uint16_t soft_sectored : 1;
    uint16_t not_mfm_encoded : 1;
    uint16_t low_head_switch_time : 1;
    uint16_t spindle_motor_control_option : 1;
    uint16_t fixed_drive : 1;
    uint16_t removable_cartidge_drive : 1;
    // less than 5Mbit/sec
    uint16_t disc_transfer_rate_low : 1;
    // Higher than 5Mbit/sec but less than 10Mbit/sec
    uint16_t disc_transfer_rate_med : 1;
    // Higher than 10Mbit/sec
    uint16_t disk_transfer_rate_high : 1;
    uint16_t rotational_speed_tolerance : 1;
    uint16_t data_stobe_option_available : 1;
    uint16_t track_offset_option_available : 1;
    uint16_t reserved_for_nonmagnetic_drives : 1;
    uint16_t format_Speed_tolerance_gap_required : 1;
    uint16_t number_of_cylinderss;
    uint16_t reserved_1;
    uint16_t number_of_heads;
    uint16_t number_of_unformated_bytes_per_track;
    uint16_t number_of_unformated_bytes_per_sector;
    uint16_t number_of_sectors_per_track;
    uint16_t reserved_2[3];
    char serial_number[20];
    uint16_t buffer_type;
    // 512 increments
    uint16_t buffer_size;
    uint16_t number_of_long_command_bytes;
    char firmware_revision[8];
    char model_number[40];
    uint16_t multiple_command_max : 8;
    uint16_t reserved_3 : 8;
    uint16_t cam_perform_double_word_io;
    uint16_t reserved_5 : 8;
    uint16_t dma_supported : 1;
    uint16_t lba_supported : 1;
    uint16_t reserved_6 : 6;
    uint16_t reserved_7;
    uint16_t reserved_8 : 8;
    uint16_t pio_data_transfer_cycle_timing : 8;
    uint16_t reserved_9 : 8;
    uint16_t dma_data_transfer_cycle_timing : 8;
    uint16_t fields_54_to_58_valid : 1;
    uint16_t reserved_10 : 15;
    uint16_t number_of_current_cylinders;
    uint16_t number_of_current_heads;
    uint16_t number_of_sectors_per_track_2;
    uint32_t current_capacity_in_sectors;
    uint16_t number_of_sectors_transfered_in_multiple : 8;
    uint16_t multiple_sector_setting_is_valid : 1;
    uint16_t reserved_11 : 7;
    uint32_t user_addressable_sectors;
    uint16_t multiword_dma_words_supported : 8;
    uint16_t multiword_dma_words_active : 8;
    uint16_t singleword_dma_words_supported : 8;
    uint16_t singleword_dma_words_active : 8;
    uint16_t reserved_12[192];
} __attribute__((packed)) ata_identify_t;

bool ata_identify(int drive, int port, ata_identify_t* identify);
void ata_write(int drive, int port, size_t sector, uintptr_t buffer, int count);
void ata_read(int drive, int port, size_t sector, uintptr_t buffer, int count);
void ata_flush(int drive, int port);


#endif //TOMATKERNEL_ATA_H
