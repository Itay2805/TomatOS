#include <stb/stb_ds.h>
#include <pci/pci.h>
#include <common/common.h>
#include <common/cpu/atomic.h>
#include <interrupts/interrupts.h>
#include <pci/pci_ids.h>
#include "rtl8169.h"

#include "rtl8169_spec.h"

typedef struct rtl8169_dev {
    pci_dev_t* dev;
    uintptr_t mmio_base;

    rx_desc_t* rx_ring;
    tx_desc_t* tx_ring;

    size_t rxi;
    size_t txi;
} rtl8169_dev_t;

static uint8_t read8(rtl8169_dev_t* dev, size_t offset) {
    return POKE8(dev->mmio_base + offset);
}

static uint16_t read16(rtl8169_dev_t* dev, size_t offset) {
    return POKE16(dev->mmio_base + offset);
}

static uint32_t read32(rtl8169_dev_t* dev, size_t offset) {
    return POKE32(dev->mmio_base + offset);
}

static uint8_t read8_vol(rtl8169_dev_t* dev, size_t offset) {
    return VOL_POKE8(dev->mmio_base + offset);
}

static uint16_t read16_vol(rtl8169_dev_t* dev, size_t offset) {
    return VOL_POKE16(dev->mmio_base + offset);
}

static uint32_t read32_vol(rtl8169_dev_t* dev, size_t offset) {
    return VOL_POKE32(dev->mmio_base + offset);
}

static void write32(rtl8169_dev_t* dev, size_t offset, uint32_t val) {
    POKE32(dev->mmio_base + offset) = val;
}

static error_t interrupt_handler(registers_t* regs, rtl8169_dev_t* dev) {
    error_t err = NO_ERROR;

    CHECK(dev != NULL);

    // check we got this interrupt
    if((read8(dev, ISR) & ROK) == 1) {
        goto cleanup;
    }

    // don't touch anything before we read it
    _barrier();

    /*
     * go over the packets, either the full ring or until we
     * get to a descriptor which is nic owned
     */
    for(int left = 1024; left > 0; left--, dev->rxi = (dev->rxi + 1) % 1024) {
        rx_desc_t* rx = &dev->rx_ring[dev->rxi];

        // once we get to nic owned ones we need to ignore it
        if(rx->own) {
            break;
        }

        // make sure we do not touch descriptor which is not ours
        _barrier();

        // get buffer and length
        uintptr_t pkt_buffer = rx->rxbuff;
        size_t pkt_length = rx->frame_length;
        // TODO: pass these stuff to the network stack
        log_debug("Got packet! %p %d", pkt_buffer, pkt_length);

        // restore descriptor
        *rx = (rx_desc_t){
            .eor = 1,
            .rxbuff = pkt_buffer,
            .frame_length = 1500
        };
    }

cleanup:
    return err;
}

static error_t init_device(rtl8169_dev_t* dev) {
    error_t err = NO_ERROR;

    CHECK(dev != NULL);

    dev->rx_ring = kmalloc(1024 * sizeof(rx_desc_t));
    dev->tx_ring = kmalloc(1024 * sizeof(tx_desc_t));

    for(int i = 0; i < 1024; i++) {
        rx_desc_t* rx_desc = &dev->rx_ring[i];
        *rx_desc = (rx_desc_t){
            .eor = 1,
            .rxbuff = (uintptr_t) kmalloc(1500),
            .frame_length = 1500
        };
    }

    // reset
    write32(dev, CR, RST);
    while(read32_vol(dev, CR) & RST) _pause();

    // enable Rx and Tx rings
    write32(dev, CR, RE | TE);

    // set the descriptors
    write32(dev, RDSAR_LOW, (uint32_t) ((uintptr_t)dev->rx_ring & 0xFFFFFFFF));
    write32(dev, RDSAR_HIGH, (uint32_t) ((uintptr_t)dev->rx_ring >> 32));
    write32(dev, TNPDS_LOW, (uint32_t) ((uintptr_t)dev->tx_ring & 0xFFFFFFFF));
    write32(dev, TNPDS_HIGH, (uint32_t) ((uintptr_t)dev->tx_ring >> 32));

    // set interrupts
    CHECK(dev->dev->irq != -1);
    _cli();
    write32(dev, IMR, ROK);
    CHECK_AND_RETHROW(interrupt_register((uint8_t) dev->dev->irq, interrupt_handler, dev));
    _sti();

    log_info("rtl8169: Initialized card (%x:%x:%x:%x:%x:%x)",
             read8(dev, IDR0),
             read8(dev, IDR1),
             read8(dev, IDR2),
             read8(dev, IDR3),
             read8(dev, IDR4),
             read8(dev, IDR5)
            );

cleanup:
    return err;
}

error_t rtl8169_init() {
    error_t err = NO_ERROR;

    CHECK(sizeof(rx_desc_t) == 16);
    CHECK(sizeof(tx_desc_t) == 16);

    for(int i = 0; i < arrlen(pci_devices); i++) {
        pci_dev_t* dev = pci_devices[i];
        if(dev->vendor_id == PCI_VENDOR_REALTEK && dev->device_id == 0x8139) {
            rtl8169_dev_t* rtldev = kmalloc(sizeof(rtl8169_dev_t));
            rtldev->dev = dev;
            rtldev->mmio_base = dev->bars[1].base;
            CATCH(init_device(rtldev));
        }
    }

cleanup:
    return err;
}