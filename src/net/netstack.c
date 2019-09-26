
#include <common/error.h>
#include <stb/stb_ds.h>
#include "peer.h"
#include "netstack.h"

socket_t* sockets;

/**
 * check that the mac matches (mac0 & mac1 == mac0)
 */
bool mac_matches(mac_t a, mac_t b) {
    for(int i = 0; i < 6; i++) {
        if((a.data[i] & b.data[i]) != a.data[i]) {
            return false;
        }
    }
    return true;
}

error_t netstack_process_packet(object_t* netdev, char* buffer, size_t size) {
    error_t err = NO_ERROR;

    CHECK(netdev != NULL);
    CHECK(netdev->type == OBJECT_NETWORK);
    CHECK(buffer != NULL);
    CHECK(size != 0);

    // parse the packet
    peer_t peer;
    size_t left_size = size;
    CHECK_AND_RETHROW(peer_parse(&peer, buffer, &left_size));

    for(int i = 0; i < arrlen(sockets); i++) {
        socket_t* socket = &sockets[i];
        peer_t* target = &socket->target;

        // make sure this socket is attached to this
        // interface
        if(target->netdev == netdev) {
            // check that the macs match
            if(!mac_matches(peer.datalink.src, target->datalink.src)) continue;
            if(!mac_matches(peer.datalink.dst, target->datalink.dst)) continue;

            // if network layer check type and ips
            if(target->network.type == peer.network.type) continue;
            if((target->network.src & peer.network.src) == target->network.src) continue;
            if((target->network.dst & peer.network.dst) == target->network.dst) continue;

            // TODO: Transport

            // matches!
            // TODO: add frame to the thingy
            CHECK_AND_RETHROW(event_signal(&socket->event));
        }
    }

cleanup:
    return err;
}
