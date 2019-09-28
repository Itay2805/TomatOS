#ifndef TOMATOS_ARP_H
#define TOMATOS_ARP_H


#include <error.h>
#include <objects/object.h>

error_t arp_server_init();
error_t arp_handle_frame(object_t* netdev, void* buffer, size_t size);

#endif //TOMATOS_ARP_H
