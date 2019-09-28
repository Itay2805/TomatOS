#ifndef TOMATOS_ETHER_H
#define TOMATOS_ETHER_H

#include <net/netstack.h>

typedef struct ether_hdr {
    mac_t dst;
    mac_t src;
    uint16_t type;
#define ETHER_IPV4  HTONS(0x0800)
#define ETHER_ARP   HTONS(0x0806)
} ether_hdr_t;

#endif //TOMATOS_ETHER_H
