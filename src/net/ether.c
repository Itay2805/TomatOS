
#include <common/error.h>
#include "peer.h"
#include "ether.h"

error_t ether_parse(peer_t* peer, void* frame, size_t* length) {
    error_t err = NO_ERROR;

    CHECK(length != NULL);
    CHECK(*length >= sizeof(ether_hdr_t));

    ether_hdr_t* header = frame;
    *length = sizeof(ether_hdr_t);

    // convert to system endian
    header->type = HTONS(header->type);

    // set the peer
    peer->datalink.src = header->src;
    peer->datalink.dst = header->dst;

    // set the type
    switch(header->type) {
        case ETHER_ARP:
            peer->network.type = NETWORK_ARP;
            break;

        case ETHER_IPV4:
            peer->network.type = NETWORK_IPV4;
            break;

        default:
            CHECK_FAIL();
            break;
    }

cleanup:
    return err;
}
