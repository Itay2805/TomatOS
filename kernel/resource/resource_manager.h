#ifndef TOMATKERNEL_RESOURCE_MANAGER_H
#define TOMATKERNEL_RESOURCE_MANAGER_H

#include <common/except.h>

struct process;

typedef int resource_t;

typedef struct resource_provider {
    int pid;
    const char* scheme;

    void* open;
    void* read;
    void* write;
    void* seek;
    void* tell;
    void* poll;
    void* close;
} resource_provider_t;

error_t resource_manager_init();

error_t resource_manager_register_provider(resource_provider_t* provider);

error_t resource_manager_get_provider_by_resource(struct process* process, resource_t resource, resource_provider_t** provider);

error_t resource_manager_get_provider_by_scheme(const char* scheme, resource_provider_t** provider);

error_t resource_manager_get_provider_by_pid(int pid, resource_provider_t** provider);

#endif