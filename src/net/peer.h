#ifndef TOMATKERNEL_PEER_H
#define TOMATKERNEL_PEER_H

#include <libc/stddef.h>
#include <common/error.h>
#include <objects/object.h>

typedef struct {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t f;
} mac_t;
typedef uint32_t ipv4_t;

#define BROADCAST_MAC ((mac_t){ 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu })
#define BROADCAST_IP ((ipv4_t){ 0xFFFFFFFF })

typedef enum network_type {
    NETWORK_ARP,
    NETWORK_IPV4,
} network_type_t;

typedef enum transport_type {
    TRANSPORT_NONE,
    TRANSPORT_TCP,
    TRANSPORT_UDP,
    TRANSPORT_ICMP,
} transport_type_t;

typedef struct peer {
    // the network device
    object_t* netdev;

    // the peer info
    struct {
        mac_t dst;
        mac_t src;
    } datalink;
    struct {
        network_type_t type;
        ipv4_t src;
        ipv4_t dst;
    } network;
    struct {
        transport_type_t type;
        uint16_t src;
        uint16_t dst;
    } transport;
} peer_t;

error_t peer_parse(peer_t* peer, void* buffer, size_t* length);

#endif //TOMATKERNEL_PEER_H
