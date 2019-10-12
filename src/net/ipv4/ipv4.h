#ifndef TOMATOS_IPV4_H
#define TOMATOS_IPV4_H

// TODO: Routing related stuff

#include <common/error.h>
#include <objects/object.h>

error_t ipv4_start_ip_server();

error_t ipv4_handle_frame(object_t* netdev, void* buffer, size_t size);

#endif //TOMATOS_IPV4_H
