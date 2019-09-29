#include <common/error.h>
#include <objects/object.h>
#include <stb/stb_ds.h>
#include <processes/event.h>
#include <processes/process.h>
#include <processes/thread.h>
#include <common/locks/preemption.h>
#include <processes/scheduler.h>
#include <objects/network.h>
#include "arp_spec.h"


////////////////////////////////////////////////////////////////////////////////////
// ARP Tables
////////////////////////////////////////////////////////////////////////////////////

// TODO: might want to have the netdev as part of the key
static struct {
    ipv4_t key;
    mac_t value;
}* arp_table;

static bool arp_resolve(ipv4_t address, mac_t* mac) {
    int i = hmgeti(arp_table, address);
    if(i == -1) {
        // TODO: need to have it send a packet or something
        return false;
    }

    // set the entry
    if(mac != NULL) *mac = arp_table[i].value;
    return true;
}

static error_t arp_add_entry(mac_t mac, ipv4_t address) {
    mac_t gotmac;
    if(arp_resolve(address, &gotmac)) {
        if(!mac_equals(mac, gotmac)) {
            log_warn("Got a different mac address for %d.%d.%d.%d (new=%02x:%02x:%02x:%02x:%02x:%02x, old=%02x:%02x:%02x:%02x:%02x:%02x)",
                 address.data[0],
                 address.data[1],
                 address.data[2],
                 address.data[3],

                 gotmac.data[0],
                 gotmac.data[1],
                 gotmac.data[2],
                 gotmac.data[3],
                 gotmac.data[4],
                 gotmac.data[5],

                 mac.data[0],
                 mac.data[1],
                 mac.data[2],
                 mac.data[3],
                 mac.data[4],
                 mac.data[5]
            );
        }else {
            return NO_ERROR;
        }
    }
    hmput(arp_table, address, mac);
    log_debug("arp table updated %d.%d.%d.%d -> %02x:%02x:%02x:%02x:%02x:%02x",
              address.data[0],
              address.data[1],
              address.data[2],
              address.data[3],

              mac.data[0],
              mac.data[1],
              mac.data[2],
              mac.data[3],
              mac.data[4],
              mac.data[5]
            );
    return NO_ERROR;
}

////////////////////////////////////////////////////////////////////////////////////
// ARP Server (handles any arp requests/replies)
////////////////////////////////////////////////////////////////////////////////////

typedef struct arp_entry {
    object_t* netdev;
    void* buffer;
} arp_entry_t;

static arp_entry_t* entries = NULL;
static thread_t* thread = NULL;
static event_t event = {0};

static error_t send_reply_packet(object_t* netdev, mac_t target_mac, ipv4_t target_ip) {
    error_t err = NO_ERROR;
    void* buffer = NULL;
    mac_t mymac = {0};
    ipv4_t myip = {0};

    CHECK(netdev != NULL);
    CHECK(netdev->functions != NULL);
    CHECK(netdev->type == OBJECT_NETWORK);

    network_functions_t* funcs = netdev->functions;
    CHECK(funcs->get_mac != NULL);
    CHECK(funcs->send != NULL);

    CHECK_AND_RETHROW(funcs->get_mac(netdev, &mymac));
    CHECK_AND_RETHROW(netstack_get_interface_ip(netdev, &myip, NULL));

    // we use pmalloc since we gonna free it in a second
    size_t buffer_size = sizeof(ether_hdr_t) + sizeof(arp_hdr_t) + sizeof(arp_ipv4_t);
    buffer = pmalloc(buffer_size);

    // fill the packet
    ether_hdr_t* ether = buffer;
    arp_hdr_t* arp = buffer + sizeof(ether_hdr_t);
    arp_ipv4_t* ipv4 = buffer + sizeof(ether_hdr_t) + sizeof(arp_hdr_t);

    ether->type = ETHER_ARP;
    ether->src = mymac;
    ether->dst = target_mac;

    arp->proto_type = ARP_PROTO_IPV4;
    arp->hw_type = ARP_HW_ETHER;
    arp->hw_addr_len = sizeof(mac_t);
    arp->proto_addr_len = sizeof(ipv4_t);
    arp->opcode = ARP_OPCODE_REPLY;

    ipv4->sender_ip = myip;
    ipv4->sender_mac = mymac;
    ipv4->target_mac = target_mac;
    ipv4->target_ip = target_ip;

    CHECK_AND_RETHROW(funcs->send(netdev, buffer, buffer_size));

cleanup:
    if(buffer != NULL) {
        pfree(buffer, buffer_size);
    }
    return err;
}

static void arp_server_thread() {
    error_t err = NO_ERROR;

    log_info("ARP server started");
    while(true) {
        // if no entries just wait
        while(arrlen(entries) <= 0) {
            CHECK_AND_RETHROW(event_wait(&event));
        }

        // handle the buffer
        uint64_t prempt = prempsave();

        // handle entry
        arp_entry_t* entry = &entries[0];
        arp_hdr_t* header = (arp_hdr_t*)entry->buffer;
        arp_ipv4_t* ipv4 = entry->buffer + sizeof(arp_hdr_t);

        // verify buffer
        // TODO: Handle IPv6
        CHECK(header->proto_type == ARP_PROTO_IPV4);
        CHECK(header->hw_addr_len == sizeof(mac_t));
        CHECK(header->proto_addr_len == sizeof(ipv4_t));

        // add the entry for the sender
        CHECK_AND_RETHROW(arp_add_entry(ipv4->sender_mac, ipv4->sender_ip));

        // process opcode
        switch(header->opcode) {
            case ARP_OPCODE_REQUEST: {
                ipv4_t myip = {{0}};
                CHECK_AND_RETHROW(netstack_get_interface_ip(entry->netdev, &myip, NULL));
                log_debug("my ip = %d.%d.%d.%d",
                      myip.data[0],
                      myip.data[1],
                      myip.data[2],
                      myip.data[3]
                );

                // if we are the target ip, send the response to sender
                // TODO: should check if the target mac is also good I think
                if(myip.raw == ipv4->target_ip.raw) {
                    CHECK_AND_RETHROW(send_reply_packet(entry->netdev, ipv4->sender_mac, ipv4->sender_ip));
                }
            } break;

            case ARP_OPCODE_REPLY:
                // this will be added automatically
                break;

            default:
                CHECK_FAIL_TRACE("Got invalid ARP opcode %x", NTOHS(header->opcode));
        }

    cleanup:
        // finish handling the buffer
        if(entries->buffer != NULL) vfree(entries->buffer);

        arrpop(entries);
        prempstore(prempt);

        CATCH(err);
    }
}

error_t arp_server_init() {
    error_t err = NO_ERROR;

    CHECK_AND_RETHROW(create_thread(kernel_process, &thread));
    thread->context.cpu.rip = (uint64_t) arp_server_thread;
    thread->status = THREAD_STATUS_NORMAL;
    CHECK_AND_RETHROW(scheduler_queue_thread(thread));

cleanup:
    return err;
}

error_t arp_handle_frame(object_t* netdev, void* nic_buffer, size_t size) {
    error_t err = NO_ERROR;
    void* buffer = NULL;

    // verify the length
    CHECK(size >= sizeof(arp_hdr_t) + sizeof(arp_ipv4_t));

    // allocate a new buffer for the buffer
    buffer = vmalloc(sizeof(arp_hdr_t) + sizeof(arp_ipv4_t));
    CHECK(buffer != NULL);

    // setup the entry and add it
    memcpy(buffer, nic_buffer, sizeof(arp_hdr_t) + sizeof(arp_ipv4_t));
    arp_entry_t entry = {
        .netdev = netdev,
        .buffer = buffer
    };
    arrpush(entries, entry);

    CHECK_AND_RETHROW(event_signal(&event));

cleanup:
    if(err != NO_ERROR) {
        if(buffer != NULL) {
            vfree(buffer);
        }
    }
    return err;
}
