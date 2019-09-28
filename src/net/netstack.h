#ifndef TOMATOS_NETSTACK_H
#define TOMATOS_NETSTACK_H

#include <common/error.h>
#include <objects/object.h>

#define HTONS(n) ((uint16_t)(((((uint16_t)(n) & 0xFF)) << 8) | (((uint16_t)(n) & 0xFF00) >> 8)))
#define NTOHS(n) ((uint16_t)(((((uint16_t)(n) & 0xFF)) << 8) | (((uint16_t)(n) & 0xFF00) >> 8)))

#define HTONL(n) (((((unsigned long)(n) & 0xFF)) << 24) | \
                  ((((unsigned long)(n) & 0xFF00)) << 8) | \
                  ((((unsigned long)(n) & 0xFF0000)) >> 8) | \
                  ((((unsigned long)(n) & 0xFF000000)) >> 24))

#define NTOHL(n) (((((unsigned long)(n) & 0xFF)) << 24) | \
                  ((((unsigned long)(n) & 0xFF00)) << 8) | \
                  ((((unsigned long)(n) & 0xFF0000)) >> 8) | \
                  ((((unsigned long)(n) & 0xFF000000)) >> 24))

typedef struct {
    uint8_t data[6];
} mac_t;

bool mac_equals(mac_t a, mac_t b);

typedef union {
    uint8_t data[4];
    uint32_t raw;
} ipv4_t;

typedef enum packet_type {
    PACKET_TYPE_TCP,
    PACKET_TYPE_UDP,
} packet_type_t;

typedef struct packet {
    packet_type_t type;
    // only includes Transport
    size_t headers_length;

    // TODO: For now only IPv4
    ipv4_t src_ip;
    ipv4_t dst_ip;

    uint16_t src_port;
    uint16_t dst_port;
} packet_t;

error_t netstack_process_frame(object_t* netdev, void* buffer, size_t size);
error_t netstack_process_packet(object_t* netdev, packet_t* packet, void* buffer, size_t size);

#endif //TOMATOS_NETSTACK_H
