#ifndef TOMATKERNEL_HPET_H
#define TOMATKERNEL_HPET_H

#include "rsdt.h"

typedef struct hpet {
    sdt_hdr_t header;

    uint8_t hardware_id;
    uint8_t comparator_count : 5;
    uint8_t counter_size : 1;
    uint8_t _reserved : 1;
    uint8_t legacy_replacement_capable : 1;
    uint16_t pci_vendor;

    uint32_t base_addr;
    uint8_t hpet_id;
    uint16_t clock_tick;
    uint8_t page_protection;
} __attribute__((packed)) hpet_t;

extern hpet_t* hpet;

/**
 * Will find and map the hpet table
 */
error_t hpet_search();

#endif //TOMATKERNEL_HPET_H
