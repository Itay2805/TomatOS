#include <common/error.h>
#include <objects/object.h>
#include <stb/stb_ds.h>
#include <processes/event.h>
#include <processes/process.h>
#include <processes/thread.h>
#include <common/locks/preemption.h>
#include <processes/scheduler.h>
#include "arp_spec.h"

typedef struct arp_entry {
    object_t* netdev;
    void* buffer;
} arp_entry_t;

static arp_entry_t* entries = NULL;
static thread_t* thread = NULL;
static event_t event = {0};

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

        switch(header->opcode) {
            case ARP_OPCODE_REQUEST:
                log_debug("got arp request");
                break;

            case ARP_OPCODE_REPLY:
                log_debug("got arp reply");
                break;

            default:
                CHECK_FAIL_TRACE("Got invalid ARP opcode %x", NTOHS(header->opcode));
        }

    cleanup:
        // finish handling the buffer
        vfree(entries->buffer);
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
