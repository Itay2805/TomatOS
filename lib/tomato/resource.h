#ifndef TOMATLIB_RESOURCE_H
#define TOMATLIB_RESOURCE_H

#include <stdint.h>

typedef int resource_handle_t;

typedef struct resource_descriptor {
    const char* scheme;
    const char* domain;
    uint64_t port;
    const char* path;
    struct resource_descriptor* sub_descriptor;
} resource_descriptor_t;

#endif //TOMATLIB_RESOURCE_H
