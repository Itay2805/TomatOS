#ifndef TOMATKERNEL_ARP_H
#define TOMATKERNEL_ARP_H

#include <libc/stddef.h>
#include <common/error.h>
#include "peer.h"

typedef struct arp_header {
    uint16_t hw_type;
    uint16_t proto_type; /* uses ether types */
    uint8_t hw_addr_length;
    uint8_t proto_addr_len;
    uint16_t opcode;
#define ARP_OPCODE_EQUEST   (1)
#define ARP_OPCODE_REPLY    (2)
    union {
        struct {
            mac_t sender_hw;
            ipv4_t sender_ip;
            mac_t target_hw;
            ipv4_t target_ip;
        } ipv4;
    };
} __attribute__((packed)) arp_header_t;

/**
 * Will initialize the ARP server
 */
error_t arp_server_init();

error_t arp_parse(peer_t* peer, void* frame, size_t* length);

/**
 * Add an entry to the cache of packets
 */
error_t arp_add_entry_ipv4(object_t* netdev, mac_t src_mac, ipv4_t src_ip);

/**
 * This will attempt to resolve the mac address of a
 * target ip
 *
 * @remark
 * This will check a cache and if it was not found
 * it will send an arp request (and block until getting
 * it back)
 */
error_t arp_resolve_ipv4(ipv4_t target_ip, mac_t* result);

#endif //TOMATKERNEL_ARP_H
