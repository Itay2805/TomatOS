#include <string.h>
#include <objects/network.h>
#include "netstack.h"

bool mac_equals(mac_t a, mac_t b) {
    return memcmp(a.data, b.data, 6) == 0;
}

error_t netstack_get_interface_mac(object_t* netdev, mac_t* mac) {
    error_t err = NO_ERROR;

    CHECK(netdev != NULL);
    CHECK(netdev->type == OBJECT_NETWORK);
    CHECK(netdev->functions != NULL);

    network_functions_t* funcs = netdev->functions;
    CHECK(funcs->get_mac != NULL);

    CHECK_AND_RETHROW(funcs->get_mac(netdev, mac));

cleanup:
    return err;
}

error_t netstack_get_interface_ip(object_t* netdev, ipv4_t* ip, ipv4_t* mast) {
    error_t err = NO_ERROR;

    // TODO: Have a real routing table

    if(ip != NULL) {
        *ip = (ipv4_t){{ 0x11, 0x22, 0x33, 0x44 }};
    }

    if(mast != NULL) {
        *mast = (ipv4_t){ .raw = 0xFFFFFFFF };
    }

cleanup:
    return err;
}