#include "resource.h"

#include <process/process.h>

#include <common/stdbool.h>
#include <common/string.h>
#include <common/buf.h>
#include <common/map.h>

#include "resource_manager.h"

uint64_t hash_resource(int pid, resource_t resource) {
    uint64_t values[] = {(uint64_t) pid, (uint64_t) resource };
    return hash_bytes(&values, sizeof(values));
}

uint64_t hash_resource_thread(int pid, int tid, resource_t resource) {
    uint64_t values[] = {(uint64_t) pid, (uint64_t) tid, (uint64_t) resource };
    return hash_bytes(&values, sizeof(values));
}


error_t resource_create(process_t* process, resource_provider_t* provider, resource_t* resource) {
    error_t err = NO_ERROR;
    bool found = false;
    int index = 0;
    
    // check arguments
    CHECK_ERROR(process != NULL, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(resource != NULL, ERROR_INVALID_ARGUMENT);
    
    // find an empty place for the resource
    for(resource_t* it = process->resources; it < buf_end(process->resources); it++, index++) {
        if(*it == 0) {
            *it = process->next_resource;
            process->providers[index] = provider;
            found = true;
            break;
        }
    }

    // if we didn't find just push a new resource
    if(!found) {
        buf_push(process->resources, process->next_resource);
        buf_push(process->providers, provider);
    }

    // set the resource and increment to the next id
    *resource = process->next_resource;
    process->next_resource++;

cleanup:
    return err;
}

error_t resource_remove(struct process* process, resource_t resource) {
    int index = 0;

    for(resource_t* it = process->resources; it < buf_end(process->resources); it++, index++) {
        if(*it == 0) {
            *it = NULL;
            process->providers[index] = NULL;
            break;
        }
    }

    return NO_ERROR;
}

error_t copy_resource_descriptor_to_kernel(struct process* original_process, resource_descriptor_t* descriptor, resource_descriptor_t** resource) {
    error_t err = NO_ERROR;
    resource_descriptor_t* new_descriptor = NULL;
    resource_descriptor_t copied_descriptor;

    CHECK_ERROR(original_process != NULL, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(descriptor != NULL, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(resource != NULL, ERROR_INVALID_ARGUMENT);

    new_descriptor = kalloc(sizeof(resource_descriptor_t));
    memset(new_descriptor, 0, sizeof(resource_descriptor_t));
    CHECK_AND_RETHROW(vmm_copy_to_kernel(original_process->address_space, descriptor, &copied_descriptor, sizeof(resource_descriptor_t)));

    // copy scheme
    if(copied_descriptor.scheme) {
        size_t len = 0;
        CHECK_AND_RETHROW(vmm_copy_string_to_kernel(original_process->address_space, copied_descriptor.scheme, NULL, &len));
        new_descriptor->scheme = kalloc(len);
        CHECK_AND_RETHROW(vmm_copy_string_to_kernel(original_process->address_space, copied_descriptor.scheme, new_descriptor->scheme, &len));
    }

    // copy domain
    if(copied_descriptor.domain) {
        size_t len = 0;
        CHECK_AND_RETHROW(vmm_copy_string_to_kernel(original_process->address_space, copied_descriptor.domain, NULL, &len));
        new_descriptor->domain = kalloc(len);
        CHECK_AND_RETHROW(vmm_copy_string_to_kernel(original_process->address_space, copied_descriptor.domain, new_descriptor->domain, &len));
    }

    // copy path
    if(copied_descriptor.path) {
        size_t len = 0;
        CHECK_AND_RETHROW(vmm_copy_string_to_kernel(original_process->address_space, copied_descriptor.path, NULL, &len));
        new_descriptor->path = kalloc(len);
        CHECK_AND_RETHROW(vmm_copy_string_to_kernel(original_process->address_space, copied_descriptor.path, new_descriptor->path, &len));
    }

    // copy port
    new_descriptor->port = copied_descriptor.port;

    // copy sub descriptor
    if(copied_descriptor.sub) {
        CHECK_AND_RETHROW(copy_resource_descriptor_to_kernel(original_process, copied_descriptor.sub, &new_descriptor->sub));
    }

    *resource = new_descriptor;

cleanup:
    if(IS_ERROR(err)) {
        if(new_descriptor != NULL) {
            delete_resource_descriptor(new_descriptor);
        }
    }
    return err;
}

error_t delete_resource_descriptor(resource_descriptor_t* resource) {
    error_t err = NO_ERROR;

    // delete the sub descriptor
    if(resource->sub != NULL) {
        CHECK_AND_RETHROW(delete_resource_descriptor(resource->sub));
    }

    // delete the strings
    if(resource->scheme != NULL)    kfree((void *) resource->scheme);
    if(resource->domain != NULL)    kfree((void *) resource->domain);
    if(resource->path != NULL)      kfree((void *) resource->path);

cleanup:
    return err;
}