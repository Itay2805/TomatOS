#ifndef TOMATOS_IPV4_H
#define TOMATOS_IPV4_H

#include <net/netstack.h>
#include "../../../boot/modules/shutdown/lib/libc/stdint.h"

typedef struct ipv4_hdr {
    uint8_t head_len : 4;
    uint8_t ver : 4;
    uint8_t tos;
    uint16_t total_length;
    uint16_t id;
    uint16_t fragment;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    ipv4_t src_ip;
    ipv4_t dst_ip;
} ipv4_hdr_t;

#endif //TOMATOS_IPV4_H
