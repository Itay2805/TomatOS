#ifndef TOMATKERNEL_RESOURCE_H
#define TOMATKERNEL_RESOURCE_H

#include <resource/resource_manager.h>

#include <common/except.h>

// TODO: Use this, basically the first few bits are gonna be used just for
//       storing the resource information (will not require a syscall to query
//       these stuff)
#define RESOURCE_ATTR_SEEKABLE (1 << 0)
#define RESOURCE_ATTR_READABLE (1 << 1)
#define RESOURCE_ATTR_WRITEABLE (1 << 2)
#define RESOURCE_ATTR_WAITABLE (1 << 3)

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

// TODO: Maybe move to the resource manager
error_t resource_create(struct process* process, resource_provider_t* provider, resource_t* resource);

// These functions are meant to be called from the kernel processes
// basically allows to call the (they can not be called from interrupt
// context)

error_t open(struct process* proc, resource_descriptor_t* descriptor, resource_t* res);

error_t close(struct process* proc, resource_t resource);

#endif
