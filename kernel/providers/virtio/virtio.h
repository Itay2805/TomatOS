//
// Created by Itay on 18/05/2019.
//

#ifndef TOMATKERNEL_VIRTIO_H
#define TOMATKERNEL_VIRTIO_H

#include <drivers/pci.h>

#define VIRTIO_VENDOR 0x1AF4
#define VIRTIO_DEVICE_NETWORK_CARD      0x1000
#define VIRTIO_DEVICE_BLOCK_DEVICE      0x1001
#define VIRTIO_DEVICE_MEMORY_BALLOONING 0x1002
#define VIRTIO_DEVICE_CONSOLE           0x1003
#define VIRTIO_DEVICE_SCSI_HOST         0x1004
#define VIRTIO_DEVICE_ENTROPY_SOURCE    0x1005
#define VIRTIO_DEVICE_9P_TRANSPORT      0x1009

typedef uint64_t le64;
typedef uint32_t le32;
typedef uint16_t le16;
typedef uint8_t  u8;

// Taken from
// https://docs.oasis-open.org/virtio/virtio/v1.1/cs01/virtio-v1.1-cs01.html#x1-2390002

/* Common configuration */
#define VIRTIO_PCI_CAP_COMMON_CFG        1
/* Notifications */
#define VIRTIO_PCI_CAP_NOTIFY_CFG        2
/* ISR Status */
#define VIRTIO_PCI_CAP_ISR_CFG           3
/* Device specific configuration */
#define VIRTIO_PCI_CAP_DEVICE_CFG        4
/* PCI configuration access */
#define VIRTIO_PCI_CAP_PCI_CFG           5

struct virtio_pci_cap {
    u8 cap_vndr;    /* Generic PCI field: PCI_CAP_ID_VNDR */
    u8 cap_next;    /* Generic PCI field: next ptr. */
    u8 cap_len;     /* Generic PCI field: capability length */
    u8 cfg_type;    /* Identifies the structure. */
    u8 bar;         /* Where to find it. */
    u8 padding[3];  /* Pad to full dword. */
    le32 offset;    /* Offset within bar. */
    le32 length;    /* Length of the structure, in bytes. */
};

typedef struct virtio_device {
    pci_device_t* pci;
} virtio_device_t;

error_t virtio_init();

#endif //TOMATKERNEL_VIRTIO_H
