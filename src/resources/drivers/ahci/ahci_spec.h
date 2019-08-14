#ifndef TOMATKERNEL_AHCI_SPEC_H
#define TOMATKERNEL_AHCI_SPEC_H


// taken from:
// https://github.com/thomtl/Rhino-OS/blob/master/include/rhino/arch/x86/drivers/ahci.h
#define AHCI_GENERIC_HOST_CONTROL_REGISTERS_SIZE 44
#define AHCI_GENERIC_HOST_CONTROL_REGISTERS_OFFSET 0x0

#define AHCI_PORT_REGISTER_SET_SIZE 0x80
#define AHCI_PORT_REGISTER_SET_OFFSET 0x100

#define AHCI_HBA_MAX_SIZE (AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * AHCI_MAX_DEVICES))

// Generic Host Control Registers
#define AHCI_CAP 0x0 // Capabilities Register, size = 4
#define AHCI_GHC 0x4 // Global Host Control, size = 4
#define AHCI_IS 0x8 // Interrupt Status, size = 4
#define AHCI_PI 0xC // Ports Implemented, size = 4
#define AHCI_VS 0x10 // Version, size = 4
#define AHCI_CCC_CTL 0x14 // Command Completion Coalescing Control, size = 4
#define AHCI_CCC_PORTS 0x18 // Command Completion Coalescing Ports, size = 4
#define AHCI_EM_LOC 0x1C // Enclosure Management Location, size = 4
#define AHCI_EM_CTL 0x20 // Enclosure Management Control, size = 4
#define AHCI_CAP2 0x24 // Extended Capabilities Register, size = 4
#define AHCI_BOHC 0x28 // BIOS/OS Handoff Register, size = 4

#define AHCI_PxCLB(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x0) // Command list base low, size = 4
#define AHCI_PxCLBU(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x4) // Command list base high, size = 4
#define AHCI_PxFB(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x8) // FIS list base low, size = 4
#define AHCI_PxFBU(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0xC) // FIS list base high, size = 4
#define AHCI_PxIS(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x10) // Interrupt status, size = 4
#define AHCI_PxIE(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x14) // Interrupt enable, size = 4
#define AHCI_PxCMD(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x18) // Command and Status, size = 4
#define AHCI_PxTFD(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x20) // Task File Data, size = 4
#define AHCI_PxSIG(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x24) // Signature, size = 4
#define AHCI_PxSSTS(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x28) // Serial ATA Status, size = 4
#define AHCI_PxSCTL(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x2C) // Serial ATA Control, size = 4
#define AHCI_PxSERR(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x30) // Serial ATA Error, size = 4
#define AHCI_PxSACT(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x34) // Serial ATA Active, size = 4
#define AHCI_PxCI(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x38) // Command Issue, size = 4
#define AHCI_PxSNTF(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x3C) // Serial ATA Notification, size = 4
#define AHCI_PxFBS(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x40) // FIS-Based Switch Controlling, size = 4
#define AHCI_PxDEVSLP(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x44) // Device Sleep, size = 4
#define AHCI_PxVS(port) ((AHCI_PORT_REGISTER_SET_OFFSET + (AHCI_PORT_REGISTER_SET_SIZE * port)) + 0x70) // Vendor Specific, size = 16

#define AHCI_CAP_NP 0 // 4 bits
#define AHCI_CAP_SXS 5
#define AHCI_CAP_EMS 6
#define AHCI_CAP_CCCS 7
#define AHCI_CAP_NCS 8
#define AHCI_CAP_PSC 13
#define AHCI_CAP_SSC 14
#define AHCI_CAP_PMD 15
#define AHCI_CAP_FBSS 16
#define AHCI_CAP_SPM 17
#define AHCI_CAP_SAM 18
#define AHCI_CAP_ISS 20
#define AHCI_CAP_SCLO 24
#define AHCI_CAP_SAL 25
#define AHCI_CAP_SALP 26
#define AHCI_CAP_SSS 27
#define AHCI_CAP_SMPS 28
#define AHCI_CAP_SSNTF 29
#define AHCI_CAP_SNCQ 30
#define AHCI_CAP_S64A 31

#define AHCI_GHC_HR 0
#define AHCI_GHC_IE 1
#define AHCI_GHC_MRSM 2
#define AHCI_GHC_EA 31

#define AHCI_VS_MAJOR 0 // 16 bits
#define AHCI_VS_MINOR 16 // 16 bits

#define AHCI_CCC_CTL_EN 0
#define AHCI_CCC_CTL_INT 3 // 4bits
#define AHCI_CCC_CTL_CC 8 // 8bits
#define AHCI_CCC_CTL_TV 16 // 16bits

#define AHCI_CCC_PORTS_PRT 0

#define AHCI_EM_LOC_SZ 0 // 16 bits
#define AHCI_EM_LOC_OFST 16 // 16 bits

#define AHCI_EM_CTL_STS_MR 0
#define AHCI_EM_CTL_CTL_TM 8
#define AHCI_EM_CTL_CTL_RST 9
#define AHCI_EM_CTL_SUPP_LED 16
#define AHCI_EM_CTL_SUPP_SAFTE 17
#define AHCI_EM_CTL_SUPP_SES2 18
#define AHCI_EM_CTL_SUPP_SGPIO 19
#define AHCI_EM_CTL_ATTR_SMB 24
#define AHCI_EM_CTL_ATTR_XMT 25
#define AHCI_EM_CTL_ATTR_ALHD 26
#define AHCI_EM_CTL_ATTR_PM 27

#define AHCI_CAP2_BOH 0
#define AHCI_CAP2_NVMP 1
#define AHCI_CAP2_APST 2
#define AHCI_CAP2_SDS 3
#define AHCI_CAP2_SADM 3
#define AHCI_CAP2_DESO 5

#define AHCI_BOHC_BOS 0
#define AHCI_BOHC_OOS 1
#define AHCI_BOHC_SOOE 2
#define AHCI_BOHC_OOC 3
#define AHCI_BOHC_BB 4

#define AHCI_PxIS_DHRS 0
#define AHCI_PxIS_PSS 1
#define AHCI_PxIS_DSS 2
#define AHCI_PxIS_SDBS 3
#define AHCI_PxIS_UFS 4
#define AHCI_PxIS_DPS 5
#define AHCI_PxIS_PCS 6
#define AHCI_PxIS_DMPS 7
#define AHCI_PxIS_PRCS 22
#define AHCI_PxIS_IPMS 23
#define AHCI_PxIS_OFS 24
#define AHCI_PxIS_INFS 26
#define AHCI_PxIS_IFS 27
#define AHCI_PxIS_HBDS 28
#define AHCI_PxIS_HBFS 29
#define AHCI_PxIS_TFES 30
#define AHCI_PxIS_CPDS 31

#define AHCI_PxIE_DHRE 0
#define AHCI_PxIE_PSE 1
#define AHCI_PxIE_DSE 2
#define AHCI_PxIE_SDBE 3
#define AHCI_PxIE_UFE 4
#define AHCI_PxIE_DPE 5
#define AHCI_PxIE_PCE 6
#define AHCI_PxIE_DMPE 7
#define AHCI_PxIE_PRCE 22
#define AHCI_PxIE_IPME 23
#define AHCI_PxIE_OFE 24
#define AHCI_PxIE_INFE 26
#define AHCI_PxIE_IFE 27
#define AHCI_PxIE_HBDE 28
#define AHCI_PxIE_HBFE 29
#define AHCI_PxIE_TFEE 30
#define AHCI_PxIE_CPDE 31

#define AHCI_PxCMD_ST 0
#define AHCI_PxCMD_SUD 1
#define AHCI_PxCMD_POD 2
#define AHCI_PxCMD_CLO 3
#define AHCI_PxCMD_FRE 4
#define AHCI_PxCMD_CCS 8 // 4 bits
#define AHCI_PxCMD_MPSS 13
#define AHCI_PxCMD_FR 14
#define AHCI_PxCMD_CR 15
#define AHCI_PxCMD_CPS 16
#define AHCI_PxCMD_PMA 17
#define AHCI_PxCMD_HPCP 18
#define AHCI_PxCMD_MPSP 19
#define AHCI_PxCMD_CPD 20
#define AHCI_PxCMD_ESP 21
#define AHCI_PxCMD_FBSCP 22
#define AHCI_PxCMD_APSTE 23
#define AHCI_PxCMD_ATAPI 24
#define AHCI_PxCMD_DLAE 26
#define AHCI_PxCMD_ASP 27
#define AHCI_PxCMD_ICC 28 // 4 bits

#define AHCI_PxTFD_STS 0 // 8 bits
#define AHCI_PxTFD_STS_ERR 1
#define AHCI_PxTFD_STS_DRQ 3
#define AHCI_PxTFD_STS_RDY 6
#define AHCI_PxTFD_STS_BSY 7
#define AHCI_PxTFD_ERR 8 // 8 bits

#define AHCI_PxSIG_SECTOR_CNT 0 // 8 bits
#define AHCI_PxSIG_LBA_LOW 8 // 8 bits
#define AHCI_PxSIG_LBA_MID 16 // 8 bits
#define AHCI_PxSIG_LBA_HIGH 24 // 8 bits

#define AHCI_PxSSTS_DET 0 // 4 bits
#define AHCI_PxSSTS_SPD 4 // 4 bits
#define AHCI_PxSSTS_IPM 8 // 4 bits

#define AHCI_PxSCTL_DET 0 // 4 bits
#define AHCI_PxSCTL_SPD 4 // 4 bits
#define AHCI_PxSCTL_IPM 8 // 4 bits
#define AHCI_PxSCTL_SPM 12 // 4 bits
#define AHCI_PxSCTL_PMP 16 // 4 bits

#define AHCI_PxSERR_ERR 0 // 16 bits
#define AHCI_PxSERR_ERR_I 0
#define AHCI_PxSERR_ERR_M 1
#define AHCI_PxSERR_ERR_T 8
#define AHCI_PxSERR_ERR_C 9
#define AHCI_PxSERR_ERR_P 10
#define AHCI_PxSERR_ERR_E 11
#define AHCI_PxSERR_DIAG 16 // 16 bits
#define AHCI_PxSERR_DIAG_N 16
#define AHCI_PxSERR_DIAG_I 17
#define AHCI_PxSERR_DIAG_W 18
#define AHCI_PxSERR_DIAG_B 19
#define AHCI_PxSERR_DIAG_D 20
#define AHCI_PxSERR_DIAG_C 21
#define AHCI_PxSERR_DIAG_H 22
#define AHCI_PxSERR_DIAG_S 23
#define AHCI_PxSERR_DIAG_T 24
#define AHCI_PxSERR_DIAG_F 25
#define AHCI_PxSERR_DIAG_X 26

#define AHCI_PxSACT_DS 0 // 32 bits

#define AHCI_PxCI_CI 0 // 32 bits

#define AHCI_PxSNTF_PMN 0 // 16 bits

#define AHCI_PxFBS_EN 0
#define AHCI_PxFBS_DEC 1
#define AHCI_PxFBS_SDE 2
#define AHCI_PxFBS_DEV 8 // 4 bits
#define AHCI_PxFBS_ADO 12 // 4 bits
#define AHCI_PxFBS_DWE 16 // 4 bits

#define AHCI_PxDEVSLP_ADSE 0
#define AHCI_PxDEVSLP_DSP 1
#define AHCI_PxDEVSLP_DETO 2 // 8 bits
#define AHCI_PxDEVSLP_MDAT 10 // 5 bits
#define AHCI_PxDEVSLP_DITO 15 // 10 bits
#define AHCI_PxDEVSLP_DM 25 // 4 bits

#define AHCI_SIGNATURE_NOTHING 0x00000000
#define AHCI_SIGNATURE_SATA 0x00000101
#define AHCI_SIGNATURE_SATAPI 0xEB140101
#define AHCI_SIGNATURE_ENCLOSURE_MANAGEMENT_BRIDGE 0xC33C0101
#define AHCI_SIGNATURE_PORT_MULTIPLIER 0x96690101
#define AHCI_SIGNATURE_DEFAULT 0xFFFFFFFF

enum {
    AHCI_FIS_TYPE_REG_H2D = 0x27,
    AHCI_FIS_TYPE_REG_D2H = 0x34,
    AHCI_FIS_TYPE_DMA_ACT = 0x39,
    AHCI_FIS_TYPE_DMA_SETUP = 0x41,
    AHCI_FIS_TYPE_DATA = 0x46,
    AHCI_FIS_TYPE_BIST = 0x58,
    AHCI_FIS_TYPE_PIO_SETUP = 0x5F,
    AHCI_FIS_TYPE_DEV_BITS = 0xA1,
} AHCI_FIS_TYPE;



#endif //TOMATKERNEL_AHCI_SPEC_H
