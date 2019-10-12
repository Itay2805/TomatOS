
#include <libc/stddef.h>
#include <objects/object.h>
#include <net/ether.h>
#include <objects/network.h>
#include <net/arp/arp.h>
#include <net/ipv4/ipv4.h>

error_t netstack_process_frame(object_t* netdev, void* buffer, size_t size) {
    error_t err = NO_ERROR;
    mac_t my_mac = {0};

    // get the mac
    CHECK(netdev != NULL);
    CHECK(netdev->type == OBJECT_NETWORK);
    network_functions_t* functions = netdev->functions;
    CHECK(functions != NULL && functions->get_mac != NULL);
    functions->get_mac(netdev, &my_mac);

    // parse the ethernet stuff
    ether_hdr_t* hdr = buffer;
    buffer += sizeof(ether_hdr_t);

    switch(hdr->type) {
        case ETHER_IPV4:
            CHECK_AND_RETHROW(ipv4_handle_frame(netdev, buffer, size));
            break;

        case ETHER_ARP:
            CHECK_AND_RETHROW(arp_handle_frame(netdev, buffer, size));
            break;

        default:
            CHECK_FAIL_TRACE("Got invalid ether type (%x)", NTOHS(hdr->type));
    }

cleanup:
    return err;
}
