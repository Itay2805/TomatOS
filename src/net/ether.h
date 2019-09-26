//
// Created by Itay on 9/26/2019.
//

#ifndef TOMATKERNEL_ETHER_H
#define TOMATKERNEL_ETHER_H

#include <libc/stddef.h>
#include <common/error.h>
#include "peer.h"

#define ETHER_IPV4  0x0800
#define ETHER_ARP   0x0806

typedef struct ether_hdr {
    mac_t dst;
    mac_t src;
    uint16_t type;
} __attribute__((packed)) ether_hdr_t;

/**
 * Parse the frame into the peer
 */
error_t ether_parse(peer_t* peer, void* frame, size_t* length);

/**
 * Create a packet from the header
 *
 * @param peer      [IN]        The peer to create from
 * @param frame     [IN]        Where to write to
 * @param length    [IN/OUT]    The length of the frame, and the length left in the frame
 */
//error_t ether_create(peer_t* peer, void* frame, size_t* length);

#endif //TOMATKERNEL_ETHER_H
