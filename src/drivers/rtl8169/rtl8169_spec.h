#ifndef TOMATKERNEL_SPEC_H
#define TOMATKERNEL_SPEC_H

/* Mac Address */
#include <libc/stddef.h>

#define IDR0            (0x0000)
#define IDR1            (0x0001)
#define IDR2            (0x0002)
#define IDR3            (0x0003)
#define IDR4            (0x0004)
#define IDR5            (0x0005)

/* Multicase Address */
#define MAR0            (0x0008)
#define MAR1            (0x0007)
#define MAR2            (0x000a)
#define MAR3            (0x000b)
#define MAR4            (0x000c)
#define MAR5            (0x000d)
#define MAR6            (0x000e)
#define MAR7            (0x000f)

/* TX ring */
#define TNPDS_LOW       (0x0020)
#define TNPDS_HIGH      (0x0024)

/* TX high priority ring */
#define THPDS_LOW       (0x0028)
#define HNPDS_HIGH      (0x003b)

/* Command Registe */
#define CR              (0x0037)
#define     RST             (1 << 4) /* Reset */
#define     RE              (1 << 3) /* Receiver enable */
#define     TE              (1 << 2) /* Transmit enable */

/* Transmit polling */
#define TPPoll          (0x0038)
#define     HQP             (1 << 7) /* Trigger high priority queue */
#define     NPQ             (1 << 6) /* Trigger normal priority queue */

/* Interrupt Mask and Status registers */
#define IMR             (0x003C)
#define ISR             (0x003E)
#define     ROK             (1 << 0) /* Recevive Ok Interrupt */

/* TX and RX configuration registers */
#define TCR             (0x0040)
#define     IFG_NORMAL      (0b11 << 24) /* InterFrameGap Time */
#define     CRC             (1 << 16) /* Append CRC */

#define RCR             (0x0044)
#define     RXFTH_NONE      (0b111 << 13) /* no rx threshold */
#define     MXDMA_UNLIMITED (0b111 << 8) /* no mac size of dma data burst */
#define     AB              (1 << 3) /* Accept Broadcast Packets */
#define     APM             (1 << 1) /* Accept Physical Match Packets */

/* Configuration registers */
#define CR9346          (0x0050)
#define     CR9346_Unlock   (0b11 << 6) /* Unlock config registers */
#define     CR9346_Lock     (0) /* Lock config registers */
#define CONFIG0         (0x0051)
#define CONFIG1         (0x0052)
#define CONFIG2         (0x0053)
#define CONFIG3         (0x0054)
#define CONFIG4         (0x0055)
#define CONFIG5         (0x0056)

/* RX packet max size */
#define RMS             (0x00DA)
    #define RMS_MASK        (0xe)

#define CPCR            (0x00E0)
#define     ENDIAN          (1 << 9) /* Endian mode (1=big, 0=low) */
#define     RxChkSum        (1 << 5) /*  */

/* RX ring */
#define RDSAR_LOW       (0x00E4)
#define RDSAR_HIGH      (0x00E8)

#define MTPS            (0x00EC)

typedef struct tx_desc {
    // dword 0
    uint16_t frame_length;
    uint16_t tcpcs : 1;
    uint16_t udpcs : 1;
    uint16_t ipcs : 1;
    uint16_t _reserved0 : 5;
    uint8_t  _reserved1 : 3;
    uint16_t lgsen : 1;
    uint16_t ls : 1;
    uint16_t fs : 1;
    uint16_t eor : 1;
    uint16_t own : 1;

    // dword 1
    uint16_t vlan_tag;
    uint16_t _reserved2 : 1;
    uint16_t tagc : 1;
    uint16_t _reserved3 : 14;

    // dword 2 + 3
    uintptr_t txbuff;
} __attribute__((packed)) tx_desc_t;

typedef struct rx_desc {
    // dword 0
    uint16_t frame_length : 14; // also the buffer_length
    // TCP Checksum Failure
    uint16_t tcpf : 1;
    // UDP Checksum Failure
    uint16_t udpf : 1;
    // IP Checksum Failure
    uint16_t ipf : 1;
    uint16_t pid : 2;
#define PID_NON_IP 0
#define PID_TCP_IP 1
#define PID_UDP_IP 2
#define PID_IP 3
    // CRC Error
    uint16_t crc : 1;
    // Runt Packet
    uint16_t runt : 1;
    // Receive Error Summary
    uint16_t res : 1;
    // Receive Watchdog Timer Expired
    uint16_t rwt : 1;
    uint16_t _reserved0 : 2;
    // Broadcast Address Received
    uint16_t bar : 1;
    // Physical Address Matched
    uint16_t pam : 1;
    // Multicast Address Packet Received
    uint16_t mar : 1;
    uint16_t ls : 1;
    uint16_t fs : 1;
    uint16_t eor : 1;
    uint16_t own : 1;

    // dword 1
    uint16_t vlan_tag;
    uint16_t tava : 1;
    uint16_t _reserved1 : 15;

    // dword 2 + 3
    uintptr_t rxbuff;
} __attribute__((packed)) rx_desc_t;

#endif //TOMATKERNEL_SPEC_H
