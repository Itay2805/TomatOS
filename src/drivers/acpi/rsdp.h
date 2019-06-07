#ifndef TOMATKERNEL_RSDP_H
#define TOMATKERNEL_RSDP_H

#include <stdint.h>
#include <error.h>

typedef struct rsdp {
    char signature[8];
    uint8_t checksum;
    char oemid[6];
    uint8_t revision;
    uint32_t rsdt_addr;

} __attribute__((packed)) rsdp_t;

typedef struct rsdp2 {
    rsdp_t rsdp;

    uint32_t length;
    uint64_t xsdt_addr;
    uint8_t checksum;
    uint8_t reserved[3];
} __attribute__((packed)) rsdp2_t;

extern rsdp2_t* rsdp2;
extern rsdp_t* rsdp;


/**
 * Will search for the rsdp
 */
error_t rsdp_init();

#endif //TOMATKERNEL_RSDP_H
