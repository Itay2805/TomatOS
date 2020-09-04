#ifndef TOMATOS_ATA_SPEC_H
#define TOMATOS_ATA_SPEC_H

#include <stdint.h>

#define ATA_ER_BBK      0x80    // Bad block
#define ATA_ER_UNC      0x40    // Uncorrectable data
#define ATA_ER_MC       0x20    // Media changed
#define ATA_ER_IDNF     0x10    // ID mark not found
#define ATA_ER_MCR      0x08    // Media change request
#define ATA_ER_ABRT     0x04    // Command aborted
#define ATA_ER_TK0NF    0x02    // Track 0 not found
#define ATA_ER_AMNF     0x01    // No address mark

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

typedef struct ata_identify_data {
    uint16_t config;
    uint16_t cylinders;
    uint16_t specific_config;
    uint16_t heads;
    uint16_t vendor_data1;
    uint16_t vendor_data2;
    uint16_t sectors_per_track;
    uint16_t vendor_specific_7_9[3];
    uint8_t serial_no[20];
    uint16_t vendor_specific_20_21[2];
    uint16_t ecc_bytes_available;
    uint8_t firmware_ver[8];
    uint8_t model_name[40];
    uint16_t multi_sector_cmd_max_sct_cnt;
    uint16_t trusted_computing_support;
    uint16_t capabilities_49;
    uint16_t capabilities_50;
    uint16_t pio_cycle_timing;
    uint16_t reserved_52;
    uint16_t field_validity;
    uint16_t current_cylinders;
    uint16_t current_heads;
    uint16_t current_sectors;
    uint16_t current_capacity_lsb;
    uint16_t current_capacity_msb;
    uint16_t multi_sector_setting;
    uint16_t user_addressable_sectors_lo;
    uint16_t user_addressable_sectors_hi;
    uint16_t obsolete_62;
    uint16_t multi_word_dma_mode;
    uint16_t advanced_pio_modes;
    uint16_t min_multi_word_dma_cycle_time;
    uint16_t rec_multi_word_dma_cycle_time;
    uint16_t min_pio_cycle_time_without_flow_control;
    uint16_t min_pio_cycle_time_with_flow_control;
    uint16_t additional_supported;
    uint16_t reserved_70;
    uint16_t reserved_71_74[4];
    uint16_t queue_depth;
    uint16_t serial_ata_capabilities;
    uint16_t reserved_77;
    uint16_t serial_ata_features_supported;
    uint16_t serial_ata_features_enabled;
    uint16_t major_version_no;
    uint16_t minor_version_no;
    uint16_t command_set_supported_82;
    uint16_t command_set_supported_83;
    uint16_t command_set_feature_extn;
    uint16_t command_set_feature_enb_85;
    uint16_t command_set_feature_enb_86;
    uint16_t command_set_feature_default;
    uint16_t ultra_dma_mode;
    uint16_t time_for_security_erase_unit;
    uint16_t time_for_enhanced_security_erase_unit;
    uint16_t advanced_power_management_level;
    uint16_t master_password_identifier;
    uint16_t hardware_configuration_test_result;
    uint16_t obsolete_94;
    uint16_t stream_minimum_request_size;
    uint16_t streaming_transfer_time_for_dma;
    uint16_t streaming_access_latency_for_dma_and_pio;
    uint16_t streaming_performance_granularity[2];
    uint16_t maximum_lba_for_48bit_addressing[4];
    uint16_t streaming_transfer_time_for_pio;
    uint16_t max_no_of_512byte_blocks_per_data_set_cmd;
    uint16_t phy_logic_sector_support;
    uint16_t interseek_delay_for_iso7779;
    uint16_t world_wide_name[4];
    uint16_t reserved_for_128bit_wwn_112_115[4];
    uint16_t reserved_for_technical_report;
    uint16_t logic_sector_size_lo;
    uint16_t logic_sector_size_hi;
    uint16_t features_and_command_sets_supported_ext;
    uint16_t features_and_command_sets_enabled_ext;
    uint16_t reserved_121_126[6];
    uint16_t obsolete_127;
    uint16_t security_status;
    uint16_t vendor_specific_129_159[31];
    uint16_t cfa_power_mode;
    uint16_t reserved_for_compactflash_161_167[7];
    uint16_t device_nominal_form_factor;
    uint16_t is_data_set_cmd_supported;
    uint8_t additional_product_identifier[8];
    uint16_t reserved_174_175[2];
    uint8_t media_serial_number[60];
    uint16_t sct_command_transport;
    uint16_t reserved_207_208[2];
    uint16_t alignment_logic_in_phy_blocks;
    uint16_t write_read_verify_sector_count_mode3[2];
    uint16_t verify_sector_count_mode2[2];
    uint16_t nv_cache_capabilities;
    uint16_t nv_cache_size_in_logical_block_lsw;
    uint16_t nv_cache_size_in_logical_block_msw;
    uint16_t nominal_media_rotation_rate;
    uint16_t reserved_218;
    uint16_t nv_cache_options;
    uint16_t write_read_verify_mode;
    uint16_t reserved_221;
    uint16_t transport_major_revision_number;
    uint16_t transport_minor_revision_number;
    uint16_t reserved_224_229[6];
    uint64_t extended_no_of_addressable_sectors;
    uint16_t min_number_per_download_microcode_mode3;
    uint16_t max_number_per_download_microcode_mode3;
    uint16_t reserved_236_254[19];
    uint16_t integrity_word;
} __attribute__((__packed__)) ata_identify_data_t;

#endif //TOMATOS_ATA_SPEC_H
