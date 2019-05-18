#include <common/string.h>
#include <memory/vmm.h>

#include "virtio-queue.h"
#include "virtio.h"

virtio_device_t* virtio_devices;

static void handle_virtio_pci_device(pci_device_t* it) {
    error_t err = NO_ERROR;

//    uint8_t cap = (uint8_t) (pci_read_uint16(it, PCI_REG_CAP) & 0xFF);
//    while(cap != NULL) {
//        struct virtio_pci_cap pci_cap = {0};
//        for(int i = 0; i < sizeof(struct virtio_pci_cap) / 4; i++) {
//            ((uint32_t *)&pci_cap)[i] = pci_read_uint32(it, (uint16_t) (cap + i * 4));
//        }
//        CHECK(pci_cap.cap_vndr == 0x09);
//
//        LOG_INFO("\t\tCap %d", pci_cap.cfg_type);
//        cap = pci_cap.cap_next;
//    }



cleanup:
    if(IS_ERROR(err)) {
        KERNEL_STACK_TRACE(err);
        ERROR_FREE(err);
    }
}

error_t virtio_init() {
    error_t err = NO_ERROR;

    LOG_NOTICE("Searching for virtio devices (using PCI)");
    for(pci_device_t* it = pci_devices; it < buf_end(pci_devices); it++) {
        if(it->vendor == VIRTIO_VENDOR) {
            switch(it->device) {
                case VIRTIO_DEVICE_NETWORK_CARD:
                    LOG_NOTICE("\tnetwork card (%d:%d:%d)", it->bus, it->slot, it->function);
                    handle_virtio_pci_device(it);
                    break;
                case VIRTIO_DEVICE_BLOCK_DEVICE:
                    LOG_NOTICE("\tblock device (%d:%d:%d)", it->bus, it->slot, it->function);
                    handle_virtio_pci_device(it);
                    break;
                case VIRTIO_DEVICE_MEMORY_BALLOONING:
                    LOG_NOTICE("\tmemory ballooning (%d:%d:%d)", it->bus, it->slot, it->function);
                    handle_virtio_pci_device(it);
                    break;
                case VIRTIO_DEVICE_CONSOLE:
                    LOG_NOTICE("\tconsole (%d:%d:%d)", it->bus, it->slot, it->function);
                    handle_virtio_pci_device(it);
                    break;
                case VIRTIO_DEVICE_SCSI_HOST:
                    LOG_NOTICE("\tSCSI host (%d:%d:%d)", it->bus, it->slot, it->function);
                    handle_virtio_pci_device(it);
                    break;
                case VIRTIO_DEVICE_ENTROPY_SOURCE:
                    LOG_NOTICE("\tentropy source (%d:%d:%d)", it->bus, it->slot, it->function);
                    handle_virtio_pci_device(it);
                    break;
                case VIRTIO_DEVICE_9P_TRANSPORT:
                    LOG_NOTICE("\t9p transport (%d:%d:%d)", it->bus, it->slot, it->function);
                    handle_virtio_pci_device(it);
                    break;
                default:
                    LOG_WARN("\tunknown device (%d:%d:%d -> %d)", it->bus, it->slot, it->function, it->device);
                    break;
            }
        }
    }

    CHECK_ERROR(buf_len(virtio_devices) > 0, ERROR_NOT_FOUND);

cleanup:
    return err;
}
