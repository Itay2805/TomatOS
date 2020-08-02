#ifndef TOMATOS_AHCI_SPEC_H
#define TOMATOS_AHCI_SPEC_H

#include <util/defs.h>
#include <stdint.h>

typedef enum fis_type {
    FIS_TYPE_REG_H2D    = 0x27, /* Host to device */
    FIS_TYPE_REG_D2H    = 0x34, /* Device to host */
    FIS_TYPE_DMA_ACT    = 0x39, /* Activate DMA - device to host */
    FIS_TYPE_DMA_SETUP  = 0x41, /* setup DMA - either direction */
    FIS_TYPE_DATA       = 0x46, /* data FIS - bidirectional */
    FIS_TYPE_BIST       = 0x58, /* activate BIST - bidirectional */
    FIS_TYPE_PIO_SETUP  = 0x5F, /* PIO setup - device host */
    FIS_TYPE_DEV_BITS   = 0xA1, /* set device bits - device to host */
} fis_type_t;

typedef struct hba_cmd_header {
    uint8_t cfl: 5;             // Command FIS length in DWORDS, 2 ~ 16
    uint8_t a: 1;               // ATAPI
    uint8_t w: 1;               // Write, 1: H2D, 0: D2H
    uint8_t p: 1;               // Prefetchable
    uint8_t  r:1;		        // Reset
    uint8_t  b:1;		        // BIST
    uint8_t  c:1;		        // Clear busy upon R_OK
    uint8_t  rsv0:1;	        // Reserved
    uint8_t  pmp:4;		        // Port multiplier port
    uint16_t prdtl;		        // Physical region descriptor table length in entries
    volatile uint32_t prdbc;    // Physical region descriptor byte count transferred
    uint64_t ctba;              // Command table descriptor base address
    uint32_t rsv1[4];	        // Reserved
} hba_cmd_header_t;

typedef struct hba_prdt_entry {
    uint64_t dba;               // Data base address
    uint32_t _reserved;
    uint32_t dbc : 22;          // Byte count, 4m max
    uint32_t _reserved1 : 9;
    uint32_t i : 1;             // Interrupt on completetion
} hba_prdt_entry_t;

typedef struct hba_cmd_table {
    uint8_t cfis[64];
    uint8_t acmd[16];
    uint8_t _reserved[48];

} hba_cmd_table_t;

typedef struct hba_port {
    uint64_t clb;       // 0x00, command list base address, 1K-byte aligned
    uint64_t fb;        // 0x08, FIS base address, 256-byte aligned
    uint32_t is;        // 0x10, interrupt status
    uint32_t ie;        // 0x14, interrupt enable
    uint32_t cmd;       // 0x18, command and status
#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000
    uint32_t rsv0;      // 0x1C, Reserved
    uint32_t tfd;       // 0x20, task file data
    uint32_t sig;       // 0x24, signature
    union {
        struct {
            uint32_t det : 4;
            uint32_t spd : 4;
            uint32_t ipm : 4;
        };
        uint32_t ssts;      // 0x28, SATA status (SCR0:SStatus)
    };
    uint32_t sctl;      // 0x2C, SATA control (SCR2:SControl)
    uint32_t serr;      // 0x30, SATA error (SCR1:SError)
    uint32_t sact;      // 0x34, SATA active (SCR3:SActive)
    uint32_t ci;        // 0x38, command issue
    uint32_t sntf;      // 0x3C, SATA notification (SCR4:SNotification)
    uint32_t fbs;       // 0x40, FIS-based switch control
    uint32_t rsv1[11];  // 0x44 ~ 0x6F, Reserved
    uint32_t vendor[4]; // 0x70 ~ 0x7F, vendor specific
} hba_port_t;

typedef struct hba_mem {
    /* x00 - 0x2B, Generic Host Control */
    uint32_t cap;       /* 0x00: CAP, host capability */
    uint32_t ghc;       /* 0x04: GHC, global host control */
    uint32_t is;        /* 0x08: IS, interrupt status */
    uint32_t pi;        /* 0x0c: PI, port implemented */
    uint32_t vs;        /* 0x10: VS, version */
    uint32_t ccc_ctl;   /* 0x14: command completion coalescing control */
    uint32_t ccc_pts;   /* 0x18: commmand completion coalescing ports */
    uint32_t em_loc;    /* 0x1c: enclosure management location */
    uint32_t em_ctl;    /* 0x20: enclosure management control */
    uint32_t cap2;      /* 0x24: CAP2, extended host capability */
    uint32_t bohc;      /* 0x28, BIOS/OS handoff control and status */
    uint8_t  rsv[0xA0-0x2C];
    uint8_t  vendor[0x100-0xA0];
    hba_port_t ports[];
} PACKED hba_mem_t;

#endif //TOMATOS_AHCI_SPEC_H
