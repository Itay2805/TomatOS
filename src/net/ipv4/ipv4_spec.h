#ifndef TOMATOS_IPV4_SPEC_H
#define TOMATOS_IPV4_SPEC_H

#include <stdint.h>
#include <net/netstack.h>

typedef struct ipv4_hdr {
    uint8_t header_length : 4;
    uint8_t version : 4;
    uint8_t tos;
    uint16_t total_length;
    uint16_t identification;
    uint16_t fragmentation;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    ipv4_t src;
    ipv4_t dst;
} ipv4_hdr_t;

#endif //TOMATOS_IPV4_SPEC_H
