#ifndef TOMATKERNEL_NETSTACK_H
#define TOMATKERNEL_NETSTACK_H

#include <common/error.h>
#include <processes/event.h>
#include "peer.h"

typedef enum socket_type {
    SOCKET_STREAM,
    SOCKET_DATAGRAM,
} socket_type_t;

typedef struct socket {
    /*
     * The patern the sockets listens for
     */
    peer_t target;

    /*
     * This dictates how the data is going to be saved (per packet
     * or as a stream)
     *
     */
    socket_type_t type;
    union {
        struct {
            // TODO: ringbuffer of bytes
        } stream;
        struct {
            // TODO: ringbuffer of packets
        } datagram;
    };

    // every time data is added to either buffers
    // it will call this event
    event_t event;
} socket_t;

error_t socket_create(socket_t** socket, peer_t* peer);

error_t socket_recv(socket_t* socket, char* buffer, size_t* size);

error_t socket_send(socket_t* socket, char* buffer, size_t* size);

error_t socket_close(socket_t* socket);

/**
 * Process a packet through the network stack
 */
error_t netstack_process_packet(object_t* netdev, char* buffer, size_t size);

#endif //TOMATKERNEL_NETSTACK_H
