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
    uint16_t general_config; // word 0

    uint16_t obsolete; // word 1
    uint16_t specific_config; //word 2

    uint16_t obsolete2; // word 3
    uint16_t retired[2]; // word 4-5
    uint16_t obsolete3; // word 6

    uint16_t reserved[2]; // word 7-8
    uint16_t retired2; // word 9

    uint16_t serial_number[20-10]; // word 10-19
    uint16_t retired3[2]; //word 20 - 21

    uint16_t obsolete4; // word 22

    uint16_t firmware_revision[27-23]; // word 23-26
    uint16_t model_number[47-27]; // word 27-46

    uint16_t obsolete5; //word 47

    uint16_t trusted_computing_features; // word 48
    uint16_t capabilities; // word 49
    uint16_t capabilities2; // word 50

    uint16_t obsolete6[53-51]; // word 51-52
    uint16_t features; // word 53

    uint16_t obsolete7[59-54]; // word 54-59
    uint16_t features2; // word 59

    uint16_t total_addressable_logical_sectors[2]; // word 60-61

    uint16_t obsolete8; // word 62
    uint16_t multiword_dma_mode; // word 63

    uint16_t pio_mode_supported; // word 64
    uint16_t mintime_multiword_dma_transfer_cycle; // word 65

    uint16_t recommended_multiword_dma_cycletime; // word 66
    uint16_t mintime_pio_transfer_cycle_without_ctrl; // word 67
    uint16_t mintime_pio_transfer_cycle_with_ctrl; // word 68

    uint16_t additional_supported; // word 69
    uint16_t reserved2; // word 70

    uint16_t reserved_for_atapi_identify[75-71]; // word 71-74
    uint16_t queue_depth; // word 75

    uint16_t sata_capabilities; // word 76
    uint16_t sata_additional_capabilities; // word 77
    uint16_t sata_features_supported; // word 78
    uint16_t sata_features_enabled; // word 79

    uint16_t major_version_number; // word 80
    uint16_t minor_version_number; // word 81

    uint16_t features_commands_sets_supported1; // word 82
    uint16_t features_commands_sets_supported2; // word 83
    uint16_t features_commands_sets_supported3; // word 84

    uint16_t features_commands_sets_supported_enabled1; // word 85
    uint16_t features_commands_sets_supported_enabled2; // word 86
    uint16_t features_commands_sets_supported_enabled3; // word 87

    uint16_t ultra_dma_modes; // word 88

    uint16_t normal_security_erase_time_copy; // word 89
    uint16_t enhanced_security_erase_time_copy; // word 90

    uint16_t apm_level; // word 91
    uint16_t master_password_identifier; // word 92

    uint16_t hardware_reset_results; // word 93
    uint16_t obsolete9; // word 94

    uint16_t stream_minimum_request_size; // word 95
    uint16_t streaming_dma_transfer_time; // word 96
    uint16_t streaming_access_latency; // word 97
    uint16_t streaming_performance_granularity[100-98]; // word 98-100
    uint16_t number_of_addressable_logical_sectors[104-100]; // word 100-103

    uint16_t streaming_pio_transfer_time; // word 104
    uint16_t maximum_number_512byte_blocks_per_dataset_management_cmd; // word 105

    uint16_t physical_logical_sector; // word 106
    uint16_t inter_seek_delay; // word 107

    uint16_t world_wide_name[112-108]; // word 108-111
    uint16_t reserved3[116-112]; // word 112-115

    uint16_t obsolete10; // word 116
    uint16_t logical_sector_size[119-117]; // word 117-118

    uint16_t features_commands_sets_supported4; // word 119
    uint16_t features_commands_sets_supported_enabled4; // word 120

    uint16_t reserved_expanded_supported_enabled_settings[127-121]; // word 121-126

    uint16_t obsolete11; // word 127
    uint16_t security_status; // word 128

    uint16_t vendor_specific[160-129]; // word 129-159
    uint16_t reserved_for_cfa[168-160]; // word 160-167
    uint16_t device_nominal_form_factor; // word 168
    uint16_t dataset_management_cmd_support; // word 169

    uint16_t additional_product_identifier[174-170]; // word 170-173

    uint16_t reserved4[176-174]; // word 174-175
    uint16_t current_media_serial_number[206-176]; // word 176-205

    uint16_t sct_cmd_transport; // word 206
    uint16_t reserved5[209-207]; // word 207-208

    uint16_t alignment_logical_pyhsical_sectors; // word 209
    uint16_t write_read_verify_sector_mode3_count[212-210]; // word 210-211
    uint16_t write_read_verify_sector_mode2_count[214-212]; // word 212-213

    uint16_t obsolete12[217-214]; // word 214-216
    uint16_t nominal_media_rotation_rate; // word 217
    uint16_t reserved7; // word 218
    uint16_t obsolete13; // word 219
    uint16_t write_read_verify_feature_set_mode; // word 220
    uint16_t reserved13; // word 221

    uint16_t transport_major_version_number; // word 222
    uint16_t transport_minor_version_number; // word 223

    uint16_t reserved14[230-224]; // word 224-229
    uint16_t extended_addressable_sectors_number[234-230]; // word 230-233

    uint16_t minimum_512byte_datablocks_per_downloadmicrocode_op; // word 234
    uint16_t max_512byte_datablocks_per_downloadmicrocode_op; // word 235

    uint16_t reserved15[255-236]; // word 236-254
    uint16_t integrity_word; // word 255
} __attribute__((__packed__)) ata_identify_data_t;

#endif //TOMATOS_ATA_SPEC_H
