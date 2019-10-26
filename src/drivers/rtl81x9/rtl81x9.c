#include <stb/stb_ds.h>
#include <pci/pci.h>
#include <common/common.h>
#include <common/cpu/atomic.h>
#include <interrupts/interrupts.h>
#include <pci/pci_ids.h>
#include <interrupts/apic/ioapic.h>
#include <interrupts/apic/lapic.h>
#include <objects/object.h>
#include <net/netstack.h>
#include <objects/network.h>
#include "rtl81x9.h"

#include "../registers.h"
#include "rtl81x9_spec.h"

static const char* supported_devices_names[];

typedef struct rtl8169_dev {
    pci_dev_t* dev;
    object_t* obj;

    uint8_t vector;
    uintptr_t mmio_base;

    rx_desc_t* rx_ring;
    tx_desc_t* tx_ring;

    size_t rxi;
    size_t txi;
} rtl8169_dev_t;

GENERATE_REGISTER_READ_WRITE(rtl8169_dev_t, mmio_base);

static error_t interrupt_handler(registers_t* regs, rtl8169_dev_t* dev) {
    error_t err = NO_ERROR;

    CHECK(dev != NULL);

    /*
     * go over the packets, either the full ring or until we
     * get to a descriptor which is nic owned
     */
    for(int left = RX_MAX_ENTRIES; left > 0; left--, dev->rxi = (dev->rxi + 1) % RX_MAX_ENTRIES) {
        rx_desc_t* rx = &dev->rx_ring[dev->rxi];

        // once we get to nic owned ones we need to ignore it
        if(rx->opts1 & OWN) {
            break;
        }

        // make sure we do not touch descriptor which is not ours
        _barrier();

        // get buffer and length
        uintptr_t pkt_buffer = rx->rxbuff + DIRECT_MAPPING_BASE;
        size_t pkt_length = rx->opts1 & FRAME_LENGTH_MASK;
        // we do not support split packets as of right now
        CHECK(rx->opts1 & (LS | FS));
        CATCH(netstack_process_frame(dev->obj, (char*)pkt_buffer, pkt_length));

        // restore descriptor
        uint32_t eor = rx->opts1 & EOR;
        rx->opts2 = 0;
        rx->opts1 = eor | 1500 | OWN;
        _barrier();
    }


cleanup:
    write16(dev, ISR, read16(dev, ISR));
    lapic_send_eoi();
    return err;
}

static error_t rtl81x9_get_mac(object_t* obj, mac_t* mac) {
    error_t err = NO_ERROR;
    rtl8169_dev_t* dev = NULL;

    CHECK(obj != NULL);
    dev = obj->context;
    CHECK(dev != NULL);

    POKE32(&mac->data[0]) = read32_relaxed(dev, IDR0);
    POKE32(&mac->data[4]) = read32_relaxed(dev, IDR4);

cleanup:
    return err;
}

static error_t rtl81x9_send(object_t* obj, void* buffer, size_t size) {
    error_t err = NO_ERROR;
    rtl8169_dev_t* dev = NULL;

    CHECK(obj != NULL);
    dev = obj->context;
    CHECK(dev != NULL);

    // get the tx
    tx_desc_t* tx = &dev->tx_ring[dev->txi++];
    CHECK(!(tx->opts1 * OWN));

    // set the descriptor
    CHECK(size <= 1500);
    memcpy((void *) CONVERT_TO_DIRECT(tx->txbuff), buffer, size);
    tx->opts2 = 0;
    tx->opts1 = (uint32_t) (OWN | LS | FS | size);
    _barrier();

    // tell the network card it got a new packet to send
    if(dev->dev->device_id == 0x8139) {
        write8(dev, TPPoll_8139, NPQ);
    }else {
        write8(dev, TPPoll, NPQ);
    }

cleanup:
    return err;
}

static network_functions_t rtl81x9_functions = {
    .get_mac = rtl81x9_get_mac,
    .send = rtl81x9_send,
};

static error_t init_device(pci_dev_t* pcidev, const char* name) {
    error_t err = NO_ERROR;

    CHECK(pcidev != NULL);
    CHECK_AND_RETHROW(pci_setup_device(pcidev));

    rtl8169_dev_t* dev = vmalloc(sizeof(rtl8169_dev_t));
    CHECK(dev != NULL);

    object_t* obj = vmalloc(sizeof(object_t));
    obj->context = dev;
    obj->name = name;
    obj->functions = &rtl81x9_functions;
    obj->type = OBJECT_NETWORK;
    CHECK_AND_RETHROW(object_add(obj));

    dev->obj = obj;

    dev->dev = pcidev;
    dev->mmio_base = pcidev->bars[1].base;

    dev->rx_ring = pmalloc(RX_MAX_ENTRIES * sizeof(rx_desc_t));
    dev->tx_ring = pmalloc(TX_MAX_ENTRIES * sizeof(tx_desc_t));

    CHECK(dev->rx_ring != NULL);
    CHECK(dev->tx_ring != NULL);

    log_info("\t\tRX: %p", dev->rx_ring);
    log_info("\t\tTX: %p", dev->tx_ring);

    // prepare ring
    // TODO: Maybe optimize the buffer allocation to be more efficient on space
    for(int i = 0; i < RX_MAX_ENTRIES; i++) {
        rx_desc_t* rx_desc = &dev->rx_ring[i];
        rx_desc->opts1 = (uint32_t) (OWN | 1500);
        rx_desc->rxbuff = (uintptr_t)pmalloc(RX_MAX_ENTRIES) - DIRECT_MAPPING_BASE;
        CHECK(rx_desc->rxbuff != NULL);

        if(i == RX_MAX_ENTRIES - 1) {
            rx_desc->opts1 |= EOR;
        }
    }

    for(int i = 0; i < TX_MAX_ENTRIES; i++) {
        tx_desc_t* tx_desc = &dev->tx_ring[i];
        tx_desc->txbuff = (uintptr_t) (pmalloc(TX_MAX_ENTRIES) - DIRECT_MAPPING_BASE);
        CHECK(tx_desc->txbuff != NULL);
    }

    // reset
    write8(dev, CR, RST);
    int timeout = UINT16_MAX;
    while((read8(dev, CR) & RST) && timeout--) _pause();
    CHECK_TRACE(timeout != 0, "Got timeout while reseting the nic!");

    /*
     * Accept broadcast and physically match packets
     * Unlimited DMA burst
     * No rx threshold
     */
    write32(dev, RCR, APM | AB | MXDMA_UNLIMITED | RXFTH_NONE);

    /*
     * append crc to every buffer
     * Unlimited DMA burst
     * normal IFG
     */
    write8(dev, CR, TE);
    write32(dev, TCR, MXDMA_UNLIMITED | CRC | IFG_NORMAL);

    /* different for rtl8139 */
    if(pcidev->device_id == 0x8139) {
        write16(dev, CPCR, RxChkSum | CPRx | CPTx);
    }else {
        write16(dev, CPCR, RxChkSum);
    }

    // set the descriptors
    write32(dev, RDSAR_LOW, (uint32_t) (((uintptr_t)dev->rx_ring - DIRECT_MAPPING_BASE) & 0xFFFFFFFF));
    write32(dev, RDSAR_HIGH, (uint32_t) (((uintptr_t)dev->rx_ring - DIRECT_MAPPING_BASE) >> 32));
    write32(dev, TNPDS_LOW, (uint32_t) (((uintptr_t)dev->tx_ring - DIRECT_MAPPING_BASE) & 0xFFFFFFFF));
    write32(dev, TNPDS_HIGH, (uint32_t) (((uintptr_t)dev->tx_ring - DIRECT_MAPPING_BASE) >> 32));

    // enable Rx and Tx rings
    write8(dev, CR, RE | TE);

    // set interrupts
    // TODO: First check for msi before trying to set it up
    dev->vector = interrupt_allocate();
    CHECK_AND_RETHROW(interrupt_register((uint8_t) dev->vector, interrupt_handler, dev));
    if(pci_setup_msi(dev->dev, dev->vector) == NO_ERROR) {
        // msi setup
        log_info("\t\tInterrupt handler (MSI #%d)", dev->vector);

    } else {
        // use normal interrupts
        CHECK(dev->dev->irq != -1);
        log_info("\t\tInterrupt handler (#%d -> #%d)", dev->dev->irq, dev->vector);
        CHECK_AND_RETHROW(ioapic_redirect(dev->dev->irq, dev->vector, 0));
    }

    // enable interrupts
    write16(dev, IMR, ROK);
    _barrier();

    log_info("\t\tMAC: %02x:%02x:%02x:%02x:%02x:%02x",
             read8_relaxed(dev, IDR0),
             read8_relaxed(dev, IDR1),
             read8_relaxed(dev, IDR2),
             read8_relaxed(dev, IDR3),
             read8_relaxed(dev, IDR4),
             read8_relaxed(dev, IDR5)
            );

cleanup:
    // TODO: Really cleanup everything (delete buffers, objects and whats not)
    return err;
}

static pci_sig_t supported_devices[] = {
        { PCI_VENDOR_REALTEK, 0x8139 },
        { PCI_VENDOR_REALTEK, 0x8168 },
        { PCI_VENDOR_REALTEK, 0x8169 },
};

static const char* supported_devices_names[] = {
    "RTL8100/8101L/8139 PCI Fast Ethernet Adapter",
    "RTL8111/8168/8411 PCIe Gigabit Ethernet Controller",
    "RTL8169 PCI Gigabit Ethernet Controller",
};

error_t rtl8169_init() {
    error_t err = NO_ERROR;

    CHECK(sizeof(rx_desc_t) == 16);
    CHECK(sizeof(tx_desc_t) == 16);

    for(int i = 0; i < arrlen(pci_devices); i++) {
        pci_dev_t* dev = pci_devices[i];

        // Identify by type
        for(int j = 0; j < (sizeof(supported_devices) / sizeof(pci_sig_t)); j++) {
            if(supported_devices[j].device_id == dev->device_id && supported_devices[j].vendor_id == dev->vendor_id) {

                // rtl8169cp supported by this driver
                if(dev->vendor_id == PCI_VENDOR_REALTEK && dev->device_id == 0x8139 && dev->revision_id < 0x20) continue;

                // init it!
                log_info("\t%s at %x.%x.%x.%x", supported_devices_names[j], dev->segment, dev->bus, dev->device, dev->function);
                CATCH(init_device(dev, supported_devices_names[j]));
                break;
            }
        }
    }

cleanup:
    return err;
}