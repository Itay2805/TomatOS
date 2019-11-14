#ifndef TOMATKERNEL_AHCI_SPEC_H
#define TOMATKERNEL_AHCI_SPEC_H

#include <stdint.h>

#define AHCI_CAP2_BOH (1u << 0u)
#define AHCI_CAP2_NVMP (1u << 1u)
#define AHCI_CAP2_APST (1u << 2u)
#define AHCI_CAP2_SDS (1u << 3u)
#define AHCI_CAP2_SADM (1u << 3u)
#define AHCI_CAP2_DESO (1u << 5u)

#define AHCI_BOHC_BOS (1u << 0u)
#define AHCI_BOHC_OOS (1u << 1u)
#define AHCI_BOHC_SOOE (1u << 2u)
#define AHCI_BOHC_OOC (1u << 3u)
#define AHCI_BOHC_BB (1u << 4u)

#define AHCI_PxTFD_STS  (1u << 0u) /* 8 bits */
#define AHCI_PxTFD_STS_ERR  (1u << 1u)
#define AHCI_PxTFD_STS_DRQ  (1u << 3u)
#define AHCI_PxTFD_STS_RDY  (1u << 6u)
#define AHCI_PxTFD_STS_BSY  (1u << 7u)
#define AHCI_PxTFD_ERR  (1u << 8u) /* 8 bits  */

#define AHCI_PxCMD_ST  (1u << 0u)
#define AHCI_PxCMD_SUD  (1u << 1u)
#define AHCI_PxCMD_POD  (1u << 2u)
#define AHCI_PxCMD_CLO  (1u << 3u)
#define AHCI_PxCMD_FRE  (1u << 4u)
#define AHCI_PxCMD_CCS  (1u << 8u) /* 4 bits */
#define AHCI_PxCMD_MPSS  (1u << 13u)
#define AHCI_PxCMD_FR  (1u << 14u)
#define AHCI_PxCMD_CR  (1u << 15u)
#define AHCI_PxCMD_CPS  (1u << 16u)
#define AHCI_PxCMD_PMA  (1u << 17u)
#define AHCI_PxCMD_HPCP  (1u << 18u)
#define AHCI_PxCMD_MPSP  (1u << 19u)
#define AHCI_PxCMD_CPD  (1u << 20u)
#define AHCI_PxCMD_ESP  (1u << 21u)
#define AHCI_PxCMD_FBSCP  (1u << 22u)
#define AHCI_PxCMD_APSTE  (1u << 23u)
#define AHCI_PxCMD_ATAPI  (1u << 24u)
#define AHCI_PxCMD_DLAE  (1u << 26u)
#define AHCI_PxCMD_ASP  (1u << 27u)
#define AHCI_PxCMD_ICC  (1u << 28u) /* 4 bits */

#define AHCI_PxIS_DHRS  (1u << 0u)
#define AHCI_PxIS_PSS  (1u << 1u)
#define AHCI_PxIS_DSS  (1u << 2u)
#define AHCI_PxIS_SDBS  (1u << 3u)
#define AHCI_PxIS_UFS  (1u << 4u)
#define AHCI_PxIS_DPS  (1u << 5u)
#define AHCI_PxIS_PCS  (1u << 6u)
#define AHCI_PxIS_DMPS  (1u << 7u)
#define AHCI_PxIS_PRCS  (1u << 22u)
#define AHCI_PxIS_IPMS  (1u << 23u)
#define AHCI_PxIS_OFS  (1u << 24u)
#define AHCI_PxIS_INFS  (1u << 26u)
#define AHCI_PxIS_IFS  (1u << 27u)
#define AHCI_PxIS_HBDS  (1u << 28u)
#define AHCI_PxIS_HBFS  (1u << 29u)
#define AHCI_PxIS_TFES  (1u << 30u)
#define AHCI_PxIS_CPDS  (1u << 31u)

#define AHCI_PxIE_DHRE  (1u << 0u)
#define AHCI_PxIE_PSE  (1u << 1u)
#define AHCI_PxIE_DSE  (1u << 2u)
#define AHCI_PxIE_SDBE  (1u << 3u)
#define AHCI_PxIE_UFE  (1u << 4u)
#define AHCI_PxIE_DPE  (1u << 5u)
#define AHCI_PxIE_PCE  (1u << 6u)
#define AHCI_PxIE_DMPE  (1u << 7u)
#define AHCI_PxIE_PRCE  (1u << 22u)
#define AHCI_PxIE_IPME  (1u << 23u)
#define AHCI_PxIE_OFE  (1u << 24u)
#define AHCI_PxIE_INFE  (1u << 26u)
#define AHCI_PxIE_IFE  (1u << 27u)
#define AHCI_PxIE_HBDE  (1u << 28u)
#define AHCI_PxIE_HBFE  (1u << 29u)
#define AHCI_PxIE_TFEE  (1u << 30u)
#define AHCI_PxIE_CPDE  (1u << 31u)

#define AHCI_SIGNATURE_NOTHING 0x00000000
#define AHCI_SIGNATURE_SATA 0x00000101
#define AHCI_SIGNATURE_SATAPI 0xEB140101
#define AHCI_SIGNATURE_ENCLOSURE_MANAGEMENT_BRIDGE 0xC33C0101
#define AHCI_SIGNATURE_PORT_MULTIPLIER 0x96690101
#define AHCI_SIGNATURE_DEFAULT 0xFFFFFFFF

typedef enum
{
    FIS_TYPE_REG_H2D	= 0x27,	// Register FIS - host to device
    FIS_TYPE_REG_D2H	= 0x34,	// Register FIS - device to host
    FIS_TYPE_DMA_ACT	= 0x39,	// DMA activate FIS - device to host
    FIS_TYPE_DMA_SETUP	= 0x41,	// DMA setup FIS - bidirectional
    FIS_TYPE_DATA		= 0x46,	// Data FIS - bidirectional
    FIS_TYPE_BIST		= 0x58,	// BIST activate FIS - bidirectional
    FIS_TYPE_PIO_SETUP	= 0x5F,	// PIO setup FIS - device to host
    FIS_TYPE_DEV_BITS	= 0xA1,	// Set device bits FIS - device to host
} FIS_TYPE;

typedef struct FIS_HEADER {
    uint8_t  fis_type;
    uint8_t  pmport : 4;
    uint8_t  rsv0 : 3;
    uint8_t  c : 1;
} __attribute__((__packed__)) FIS_HEADER;

typedef struct tagFIS_REG_H2D
{
    // DWORD 0
    FIS_HEADER h;

    uint8_t  command;	// Command register
    uint8_t  featurel;	// Feature register, 7:0

    // DWORD 1
    uint8_t  lba0;		// LBA low register, 7:0
    uint8_t  lba1;		// LBA mid register, 15:8
    uint8_t  lba2;		// LBA high register, 23:16
    uint8_t  device;		// Device register

    // DWORD 2
    uint8_t  lba3;		// LBA register, 31:24
    uint8_t  lba4;		// LBA register, 39:32
    uint8_t  lba5;		// LBA register, 47:40
    uint8_t  featureh;	// Feature register, 15:8

    // DWORD 3
    uint8_t  countl;		// Count register, 7:0
    uint8_t  counth;		// Count register, 15:8
    uint8_t  icc;		// Isochronous command completion
    uint8_t  control;	// Control register

    // DWORD 4
    uint8_t  rsv1[4];	// Reserved
} __attribute__((__packed__)) FIS_REG_H2D;

typedef struct tagFIS_REG_D2H
{
    // DWORD 0
    FIS_HEADER h;

    uint8_t  status;      // Status register
    uint8_t  error;       // Error register

    // DWORD 1
    uint8_t  lba0;        // LBA low register, 7:0
    uint8_t  lba1;        // LBA mid register, 15:8
    uint8_t  lba2;        // LBA high register, 23:16
    uint8_t  device;      // Device register

    // DWORD 2
    uint8_t  lba3;        // LBA register, 31:24
    uint8_t  lba4;        // LBA register, 39:32
    uint8_t  lba5;        // LBA register, 47:40
    uint8_t  rsv2;        // Reserved

    // DWORD 3
    uint8_t  countl;      // Count register, 7:0
    uint8_t  counth;      // Count register, 15:8
    uint8_t  rsv3[2];     // Reserved

    // DWORD 4
    uint8_t  rsv4[4];     // Reserved
} __attribute__((__packed__)) FIS_REG_D2H;

typedef struct tagFIS_DATA
{
    // DWORD 0
    FIS_HEADER h;

    uint8_t  rsv1[2];	// Reserved

    // DWORD 1 ~ N
    uint32_t data[1];	// Payload
} __attribute__((__packed__)) FIS_DATA;

typedef struct tagFIS_PIO_SETUP
{
    // DWORD 0
    FIS_HEADER h;

    uint8_t  status;		// Status register
    uint8_t  error;		// Error register

    // DWORD 1
    uint8_t  lba0;		// LBA low register, 7:0
    uint8_t  lba1;		// LBA mid register, 15:8
    uint8_t  lba2;		// LBA high register, 23:16
    uint8_t  device;		// Device register

    // DWORD 2
    uint8_t  lba3;		// LBA register, 31:24
    uint8_t  lba4;		// LBA register, 39:32
    uint8_t  lba5;		// LBA register, 47:40
    uint8_t  rsv2;		// Reserved

    // DWORD 3
    uint8_t  countl;		// Count register, 7:0
    uint8_t  counth;		// Count register, 15:8
    uint8_t  rsv3;		// Reserved
    uint8_t  e_status;	// New value of status register

    // DWORD 4
    uint16_t tc;		// Transfer count
    uint8_t  rsv4[2];	// Reserved
} __attribute__((__packed__)) FIS_PIO_SETUP;

typedef struct tagFIS_DMA_SETUP
{
    // DWORD 0
    FIS_HEADER h;

    uint8_t  rsved[2];       // Reserved

    //DWORD 1&2

    uint32_t DMAbufferID;    // DMA Buffer Identifier. Used to Identify DMA buffer in host memory. SATA Spec says host specific and not in Spec. Trying AHCI spec might work.
    uint32_t DMAbufferID2;    // DMA Buffer Identifier. Used to Identify DMA buffer in host memory. SATA Spec says host specific and not in Spec. Trying AHCI spec might work.
    //DWORD 3
    uint32_t rsvd;           //More reserved
    //DWORD 4
    uint32_t DMAbufOffset;   //Byte offset into buffer. First 2 bits must be 0

    //DWORD 5
    uint32_t TransferCount;  //Number of bytes to transfer. Bit 0 must be 0
    //DWORD 6
    uint32_t resvd;          //Reserved

} __attribute__((__packed__)) FIS_DMA_SETUP;


typedef volatile struct AHCI_HBA_PORT {
    uint32_t clb;		// 0x00, command list base address, 1K-byte aligned
    uint32_t clbu;		// 0x04, command list base address upper 32 bits
    uint32_t fb;		// 0x08, FIS base address, 256-byte aligned
    uint32_t fbu;		// 0x0C, FIS base address upper 32 bits
    uint32_t is;		// 0x10, interrupt status
    uint32_t ie;		// 0x14, interrupt enable
    uint32_t cmd;		// 0x18, command and status
    uint32_t rsv0;		// 0x1C, Reserved
    uint32_t tfd;		// 0x20, task file data
    uint32_t sig;		// 0x24, signature
    uint32_t ssts;		// 0x28, SATA status (SCR0:SStatus)
    uint32_t sctl;		// 0x2C, SATA control (SCR2:SControl)
    uint32_t serr;		// 0x30, SATA error (SCR1:SError)
    uint32_t sact;		// 0x34, SATA active (SCR3:SActive)
    uint32_t ci;		// 0x38, command issue
    uint32_t sntf;		// 0x3C, SATA notification (SCR4:SNotification)
    uint32_t fbs;		// 0x40, FIS-based switch control
    uint32_t rsv1[11];	// 0x44 ~ 0x6F, Reserved
    uint32_t vendor[4];	// 0x70 ~ 0x7F, vendor specific
} __attribute__((__packed__)) AHCI_HBA_PORT;

typedef volatile struct AHCI_HBA_MEM {
    // 0x00 - 0x2B, Generic Host Control
    uint32_t cap;		// 0x00, Host capability
    uint32_t ghc;		// 0x04, Global host control
    uint32_t is;		// 0x08, Interrupt status
    uint32_t pi;		// 0x0C, Port implemented
    uint32_t vs;		// 0x10, Version
    uint32_t ccc_ctl;	// 0x14, Command completion coalescing control
    uint32_t ccc_pts;	// 0x18, Command completion coalescing ports
    uint32_t em_loc;		// 0x1C, Enclosure management location
    uint32_t em_ctl;		// 0x20, Enclosure management control
    uint32_t cap2;		// 0x24, Host capabilities extended
    uint32_t bohc;		// 0x28, BIOS/OS handoff control and status

    // 0x2C - 0x9F, Reserved
    uint8_t  rsv[0xA0-0x2C];

    // 0xA0 - 0xFF, Vendor specific registers
    uint8_t  vendor[0x100-0xA0];

    // 0x100 - 0x10FF, Port control registers
    AHCI_HBA_PORT	ports[1];	// 1 ~ 32
} __attribute__((__packed__)) AHCI_HBA;

typedef struct AHCI_HBA_CMD_HEADER {
    uint8_t cfl : 5; // Command FIS Length
    uint8_t a : 1; // ATAPI
    uint8_t w : 1; // Write
    uint8_t p : 1; // Prefetchable
    uint8_t r : 1; // Reset
    uint8_t b : 1; // BIST
    uint8_t c : 1; // Clear Busy upon R_OK
    uint8_t rsv0 : 1;
    uint8_t pmp : 4; // Port Multiplier Port
    uint16_t prdtl;	// Physical region descriptor table length in entries
    volatile uint32_t prdbc; // Physical region descriptor byte count transferred
    uint32_t ctba;		// Command table descriptor base address
    uint32_t ctbau;		// Command table descriptor base address upper 32 bits
    uint32_t _reserved0[4];	// Reserved
} __attribute__((__packed__)) AHCI_HBA_CMD_HEADER;

typedef struct AHCI_HBA_PRDT_ENTRY {
    uint32_t dba;		// Data base address
    uint32_t dbau;		// Data base address upper 32 bits
    uint32_t rsv0;		// Reserved
    uint32_t dbc : 22;	// Byte count, 4M max
    uint32_t _reserved0 : 9;	// Reserved
    uint32_t i : 1;		// Interrupt on completion
} __attribute__((__packed__)) AHCI_HBA_PRDT_ENTRY;

typedef struct AHCI_HBA_CMD_TBL {
    uint8_t  cfis[64];	// Command FIS
    uint8_t  acmd[16];	// ATAPI command, 12 or 16 bytes
    uint8_t  _reserved[48];	// Reserved
    AHCI_HBA_PRDT_ENTRY	prdt_entry[1];	// Physical region descriptor table entries, 0 ~ 65535
} __attribute__((__packed__)) AHCI_HBA_CMD_TBL;

typedef struct AHCI_HBA_FIS {
    FIS_DMA_SETUP dsfis;
    uint8_t _pad0[4];
    FIS_PIO_SETUP psfis;
    uint8_t _pad1[12];
    FIS_REG_D2H rfis;
    uint8_t _pad2[4];
    uint8_t sdbfis[8];
    uint8_t ufis[64];
    uint8_t _reserved[0x60];
} __attribute__((__packed__)) AHCI_HBA_FIS;

#endif //TOMATKERNEL_AHCI_SPEC_H
