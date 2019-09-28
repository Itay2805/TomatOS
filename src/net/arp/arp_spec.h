#ifndef TOMATOS_ARP_SPEC_H
#define TOMATOS_ARP_SPEC_H

#include <net/netstack.h>
#include <net/ether.h>

typedef struct {
    uint16_t hw_type;
    uint16_t proto_type;
#define ARP_PROTO_IPV4  ETHER_IPV4
    uint8_t hw_addr_len;
    uint8_t proto_addr_len;
    uint16_t opcode;
#define ARP_OPCODE_REQUEST      HTONS(1)
#define ARP_OPCODE_REPLY        HTONS(2)
} arp_hdr_t;

typedef struct arp_ipv4 {
    mac_t sender_mac;
    ipv4_t sender_ip;
    mac_t target_mac;
    ipv4_t target_ip;
} arp_ipv4_t;

#endif //TOMATOS_ARP_SPEC_H
