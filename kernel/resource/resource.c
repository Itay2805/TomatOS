#include "resource.h"

#include <process/process.h>

#include <common/stdbool.h>
#include <common/string.h>
#include <common/buf.h>

#include "resource_manager.h"

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
