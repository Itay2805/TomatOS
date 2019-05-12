#ifndef TOMATKERNEL_ATA_H
#define TOMATKERNEL_ATA_H

#include <common/stdbool.h>

#define ATA_REG_DATA                        (uint16_t)0u
#define ATA_REG_ERROR                       (uint16_t)1u
#define ATA_REG_FEATURES                    (uint16_t)1u
#define ATA_REG_SECTOR_COUNT_0              (uint16_t)2u
#define ATA_REG_LBA0                        (uint16_t)3u
#define ATA_REG_LBA1                        (uint16_t)4u
#define ATA_REG_LBA2                        (uint16_t)5u
#define ATA_REG_HARD_DRIVE_DEVICE_SELECT    (uint16_t)6u
#define ATA_REG_COMMAND                     (uint16_t)7u
#define ATA_REG_STATUS                      (uint16_t)7u
#define ATA_REG_SECTOR_COUNT_1              (uint16_t)8u
#define ATA_REG_LBA3                        (uint16_t)9u
#define ATA_REG_LBA4                        (uint16_t)10u
#define ATA_REG_LBA5                        (uint16_t)11u
#define ATA_REG_CONTROL                     (uint16_t)12u
#define ATA_REG_ALT_STATUS                  (uint16_t)13u
#define ATA_REG_DEVICE_ADDRESS              (uint16_t)14u

#define ATA_COMMAND_READ_PIO                (uint8_t)0x20u
#define ATA_COMMAND_READ_PIO_EXT            (uint8_t)0x24u
#define ATA_COMMAND_READ_DMA                (uint8_t)0xC8u
#define ATA_COMMAND_READ_DMA_EXT            (uint8_t)0x25u
#define ATA_COMMAND_WRITE_PIO               (uint8_t)0x30u
#define ATA_COMMAND_WRITE_PIO_EXT           (uint8_t)0x34u
#define ATA_COMMAND_WRITE_DMA               (uint8_t)0xCAu
#define ATA_COMMAND_WRITE_DMA_EXT           (uint8_t)0x35u
#define ATA_COMMAND_CACHE_FLUSH             (uint8_t)0xE7
#define ATA_COMMAND_CACHE_FLUSH_EXT         (uint8_t)0xEA
#define ATA_COMMAND_PACKET                  (uint8_t)0xA0
#define ATA_COMMAND_IDENTIFY_PACKET         (uint8_t)0xA1
#define ATA_COMMAND_IDENTIFY                (uint8_t)0xEC

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

typedef struct ata_status {
    uint8_t error : 1;
    uint8_t index : 1;
    uint8_t corrected_data : 1;
    uint8_t data_request_ready : 1;
    uint8_t drive_seek_complete : 1;
    uint8_t drive_write_fault : 1;
    uint8_t drive_ready : 1;
    uint8_t drive_busy : 1;
} __attribute__((packed)) ata_status_t;

/**
 * Identify the drive, will return error if the drive was not found or is not ata
 *
 * @param drive     [IN]    Will decide the bus to use
 * @param port      [IN]    The port of the bus
 * @param identify  [OUT]   The identify packet
 */
error_t ata_identify(int drive, int port, ata_identify_t* identify);

/**
 * Write sectors to the drive with the given drive and port
 *
 * @param drive     [IN]    The bus
 * @param port      [IN]    The port on the bus
 * @param lba       [IN]    The lba address
 * @param buffer    [IN]    The buffer to write (must be 512bytes long)
 */
error_t ata_write_sector(int drive, int port, uint64_t lba, char* buffer);

/**
 * Read sectors from the drive with the given bus and port
 *
 * @param drive         [IN]    The bus
 * @param port          [IN]    The port on the bus
 * @param lba           [IN]    The lba address
 * @param buffer        [OUT]   The buffer to write (must be 512bytes long)
 * @param sector_count  [IN]    The sector count
 */
error_t ata_read_sector(int drive, int port, uint64_t lba, char* buffer);

#endif //TOMATKERNEL_ATA_H
