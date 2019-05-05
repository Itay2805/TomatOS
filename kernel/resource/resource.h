#ifndef TOMATKERNEL_RESOURCE_H
#define TOMATKERNEL_RESOURCE_H

#include <common/except.h>

typedef int resource_t;

typedef struct resource_descriptor {
    const char* scheme;
    const char* domain;
    const char* path;
    int port;
    struct resource_descriptor* sub;
} resource_descriptor_t;

// forward declare
struct process;

error_t resource_create(process_t* process, resource_t* resource);

error_t resource_queue_open(struct process* proc, resource_descriptor_t* descriptor, resource_t* res);

error_t resource_queue_close(struct process* proc, resource_t resource);

#endif
