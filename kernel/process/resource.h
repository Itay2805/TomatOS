#ifndef PROCESS_RESOURCE_H
#define PROCESS_RESOURCE_H

#include "process.h"

/**
 * Represents a resource handle
 */
typedef int resource_t;

/**
 * A dynamic resource parameter
 */
typedef struct resource_parameter {
    const char* name;
    void* value;
    int value_size;
} resource_parameter_t;

/**
 * A resource description
 *
 * scheme://[subresource]domain:port/path?params
 */
typedef struct resource_description {
    const char* scheme;
    const char* domain;
    short port;
    const char* path;

    /*
    need to be a stretchy buf
    */
    resource_parameter_t* params;

    resource_t subresource;
    struct resource_description* subdescriptor;
} resource_description_t;

typedef struct provider provider_t;

/**
 * A provider, the pointers
 */
struct provider {
    const char* scheme;
    int pid;

    // provider_context*,
    void* init;

    // provider_context*, resource_description*, pid -> resource
    void* open;

    // provider_context*, resource, pid -> resource
    void* dup;

    // provider_context*, resource, pid
    void* close;

    // provider_context*, resource, pid, char* buffer, int buffer -> int
    void* read;

    // provider_context*, resource, pid, char* buffer, int buffer -> int
    void* write;
};

#endif
