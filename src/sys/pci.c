#include <stdbool.h>
#include <acpi/tables/rsdt.h>
#include <acpi/tables/mcfg.h>
#include <stb_ds.h>
#include "pci.h"
#include "pci_spec.h"

uint32_t (*pci_read_dword)(pci_dev_t* dev, int pos) = NULL;
uint16_t (*pci_read_word)(pci_dev_t* dev, int pos) = NULL;
uint8_t (*pci_read_byte)(pci_dev_t* dev, int pos) = NULL;

void (*pci_write_dword)(pci_dev_t* dev, int pos, uint32_t data) = NULL;
void (*pci_write_word)(pci_dev_t* dev, int pos, uint16_t data) = NULL;
void (*pci_write_byte)(pci_dev_t* dev, int pos, uint8_t data) = NULL;

pci_dev_t* g_pci_devices = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Legacy read and write implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static uint32_t legacy_pci_read_dword(pci_dev_t* dev, int pos) {
    ASSERT(pos <= 0xFF);
    __outdword(0xCF8, 0x80000000  | (dev->bus << 16) | (dev->dev << 11) | (dev->func << 8) | (pos & ~3));
    return __indword(0xcfc + (pos & 3));
}

static uint16_t legacy_pci_read_word(pci_dev_t* dev, int pos) {
    ASSERT(pos <= 0xFF);
    __outdword(0xCF8, 0x80000000  | (dev->bus << 16) | (dev->dev << 11) | (dev->func << 8) | (pos & ~3));
    return __inword(0xcfc + (pos & 3));
}

static uint8_t legacy_pci_read_byte(pci_dev_t* dev, int pos) {
    ASSERT(pos <= 0xFF);
    __outdword(0xCF8, 0x80000000  | (dev->bus << 16) | (dev->dev << 11) | (dev->func << 8) | (pos & ~3));
    return __inbyte(0xcfc + (pos & 3));
}

static void legacy_pci_write_dword(pci_dev_t* dev, int pos, uint32_t data) {
    ASSERT(pos <= 0xFF);
    __outdword(0xCF8, 0x80000000  | (dev->bus << 16) | (dev->dev << 11) | (dev->func << 8) | (pos & ~3));
    __outdword(0xcfc + (pos & 3), data);
}

static void legacy_pci_write_word(pci_dev_t* dev, int pos, uint16_t data) {
    ASSERT(pos <= 0xFF);
    __outdword(0xCF8, 0x80000000  | (dev->bus << 16) | (dev->dev << 11) | (dev->func << 8) | (pos & ~3));
    __outword(0xcfc + (pos & 3), data);
}

static void legacy_pci_write_byte(pci_dev_t* dev, int pos, uint8_t data) {
    ASSERT(pos <= 0xFF);
    __outdword(0xCF8, 0x80000000  | (dev->bus << 16) | (dev->dev << 11) | (dev->func << 8) | (pos & ~3));
    __outbyte(0xcfc + (pos & 3), data);
}


static err_t detect_legacy_pci() {
    err_t err = NO_ERROR;

    __outbyte(0xCFB, 0x01);
    uint32_t tmp = __indword(0xCF8);
    __outdword(0xCF8, 0x80000000);
    CHECK_ERROR_TRACE(__indword(0xCF8) == 0x80000000, ERROR_NOT_FOUND, "pci: Legacy PCI is not supported on this machine!");

    TRACE("pci: Using legacy PCI access");

cleanup:
    __outword(0xCF8, tmp);
    return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCFG read and write implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static err_t detect_mmio_pci() {
    err_t err = NO_ERROR;

    // Search for the mcfg table
    acpi_mcfg_t* header = (acpi_mcfg_t*)rsdt_search("MCFG", 0);
    CHECK_ERROR_TRACE(header != NULL, ERROR_NOT_FOUND, "pci: MMIO PCI access is not supported on this machine!");

    CHECK_FAIL();

cleanup:
    return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// generic code
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pci_read_bytes(pci_dev_t* dev, int pos, uint8_t* buf, int len) {
    for (int i = 0; i < len; i++) {
        buf[i] = pci_read_byte(dev, pos + i);
    }
}

static void scan_bus(int bus) {
    pci_dev_t pci = { .bus = bus };

    // iterate all the devices on the bus
    for (int dev = 0; dev < 32; dev++) {
        pci.dev = dev;

        // iterate all the functions
        int multi = 0;
        for (int func = 0; func < 8 && (multi || func == 0); func++) {
            uint32_t vendor_device_id = pci_read_dword(&pci, PCI_DEVICE_ID_OFFSET);

            // check the device id is valid
            if (vendor_device_id == 0xffffffff) {
                continue;
            }

            // check for multi device
            uint8_t header_type = pci_read_byte(&pci, PCI_HEADER_TYPE_OFFSET);
            if (func == 0) {
                multi = header_type & HEADER_TYPE_MULTI_FUNCTION;
            }
            header_type &= HEADER_LAYOUT_CODE;

            // add the device
            arrpush(g_pci_devices, (pci_dev_t){});
            pci_dev_t* d = &arrlast(g_pci_devices);
            d->bus = bus;
            d->dev = dev;
            d->func = func;
            d->vendor_id = vendor_device_id & 0xffff;
            d->device_id = vendor_device_id >> 16;

            // fill common information
            d->device_class = pci_read_word(d, PCI_CLASSCODE_OFFSET);

            // log it
            // TODO: include name
            TRACE("pci: \t%02x:%02x.%d", d->bus, d->dev, d->func);

            // figure the device type
            switch (header_type) {
                // this is a normal device
                case HEADER_TYPE_DEVICE: {
                    arrsetcap(d->bars, 6);
                } break;

                // recurse over the next bus
                case HEADER_TYPE_PCI_TO_PCI_BRIDGE:
                    arrsetcap(d->bars, 2);
                    scan_bus(pci_read_byte(&pci, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));
                    break;

                default:
                    WARN(false, "pci: \t\thas unknown header type %x", header_type);
            }

            // TODO: fill bars

            // TODO: fill caps
        }
    }
}

err_t init_pci() {
    err_t err = NO_ERROR;

    if (!IS_ERROR(detect_mmio_pci())) {
        ASSERT(false);
        goto found_access_method;
    }

    if (!IS_ERROR(detect_legacy_pci())) {

        pci_read_byte = legacy_pci_read_byte;
        pci_read_word = legacy_pci_read_word;
        pci_read_dword = legacy_pci_read_dword;

        pci_write_byte = legacy_pci_write_byte;
        pci_write_word = legacy_pci_write_word;
        pci_write_dword = legacy_pci_write_dword;

        goto found_access_method;
    }

    CHECK_FAIL_ERROR_TRACE(ERROR_NOT_FOUND, "pci: No PCI access method found");

found_access_method:

    TRACE("pci: Scanning for pci devices:");
    scan_bus(0);

cleanup:
    return err;
}
