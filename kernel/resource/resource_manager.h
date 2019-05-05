#ifndef TOMATKERNEL_RESOURCE_MANAGER_H
#define TOMATKERNEL_RESOURCE_MANAGER_H

#include <common/except.h>
#include <process/process.h>

typedef struct resource_provider {
    int pid;
    const char* scheme;
    void* open;
    void* close;
} resource_provider_t;

error_t resource_manager_init();

error_t resource_manager_register_provider(resource_provider_t* provider);

error_t resource_manager_get_provider_by_scheme(const char* scheme);

error_t resource_manager_get_provider_by_pid(int pid);

#endif