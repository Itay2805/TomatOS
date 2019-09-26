#include <common/error.h>
#include <processes/thread.h>
#include <processes/process.h>
#include <processes/scheduler.h>
#include <libc/stdlib.h>
#include "netstack.h"
#include "arp.h"
#include "ether.h"

typedef struct arp_server_instance {
    object_t* netdev;
    thread_t* arp_server_thread;
} arp_server_instance_t;

static void arp_server_thread(arp_server_instance_t* instance) {
    error_t err = NO_ERROR;

    socket_t* arp_socket;
    arp_header_t arp_header;
    peer_t peer = {
        .netdev = instance->netdev,
        .datalink = {
            .src = BROADCAST_MAC,
            .dst = BROADCAST_MAC,
        },
        .network = {
            .type = NETWORK_ARP,
            .src = BROADCAST_IP, // sender
            .dst = BROADCAST_IP, // target
        }
    };

    // create a socket listening for arp
    CHECK_AND_RETHROW(socket_create(&arp_socket, &peer));

    // wait for arp packets
    while(true) {
        // read the packet
        size_t size = sizeof(arp_header_t);
        CHECK_AND_RETHROW_LABEL(socket_recv(arp_socket, (void*)&arp_header, &size), fail_handle_packet);
        CHECK_LABEL(size == sizeof(arp_header_t), fail_handle_packet);

        // verify this looks good
        CHECK_LABEL(arp_header.hw_addr_length == sizeof(mac_t), fail_handle_packet);
        CHECK_LABEL(arp_header.proto_addr_len == sizeof(ipv4_t), fail_handle_packet);
        CHECK_LABEL(arp_header.proto_type == ETHER_ARP, fail_handle_packet);

        // add the entry to our list of destinations
        CHECK_AND_RETHROW_LABEL(arp_add_entry_ipv4(instance->netdev, arp_header.ipv4.sender_hw, arp_header.ipv4.sender_ip), fail_handle_packet);

        // TODO: send the response if the target is to our network device

    fail_handle_packet:
        CATCH(err);
    }

cleanup:
    CATCH(err);
    log_critical("ARP server stopped!");

    // free everything related to the server
    CATCH(socket_close(arp_socket));
    vfree(instance);

    // TODO: kill the thread
    while(true);
}

error_t arp_server_init() {
    error_t err = NO_ERROR;

    // TODO: iterate all network objects
    arp_server_instance_t* instance = vmalloc(sizeof(arp_server_instance_t));
    CHECK_AND_RETHROW(object_get_primary(OBJECT_NETWORK, &instance->netdev));
    CHECK_AND_RETHROW(create_thread(kernel_process, &instance->arp_server_thread));
    instance->arp_server_thread->context.cpu.rip = (uint64_t) arp_server_thread;
    instance->arp_server_thread->context.cpu.rdi = (uint64_t) instance;
    instance->arp_server_thread->status = THREAD_STATUS_NORMAL;
    CHECK_AND_RETHROW(scheduler_queue_thread(instance->arp_server_thread));

cleanup:
    return err;
}
