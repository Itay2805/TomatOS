


#include <common/error.h>
#include "peer.h"
#include "ether.h"
#include "arp.h"

error_t peer_parse(peer_t* peer, void* buffer, size_t* length) {
    error_t err = NO_ERROR;
    size_t header_length;
    size_t size_left = *length;
    char* parse_buffer = buffer;

    // only supports ethernet
    header_length = size_left;
    CHECK_AND_RETHROW(ether_parse(peer, parse_buffer, &header_length));

    // Parse network layer
    size_left -= header_length;
    parse_buffer += header_length;
    header_length = *length - header_length;
    switch(peer->network.type) {
        case NETWORK_ARP:
            CHECK_AND_RETHROW(arp_parse(peer, buffer, length));
            break;

        case NETWORK_IPV4:
            // TODO: parse ipv4
            break;

        default:
            CHECK_FAIL();
    }

    // Parse transport stack
    size_left -= header_length;
    parse_buffer += header_length;
    header_length = *length - header_length;
    switch(peer->transport.type) {

        case TRANSPORT_ICMP:
            // TODO: Parse ICMP
            break;

        case TRANSPORT_TCP:
            // TODO: Parse TCP
            break;

        case TRANSPORT_UDP:
            // TODO: Parse UDP
            break;

        case TRANSPORT_NONE:
            goto cleanup;

        default:
            CHECK_FAIL();
    }

cleanup:
    if(err == NO_ERROR) {
        *length = size_left;
    }
    return err;
}

