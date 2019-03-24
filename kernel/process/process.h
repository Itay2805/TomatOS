//
// Created by Itay on 24/03/2019.
//

#ifndef PROCESS_H
#define PROCESS_H

#include <common/map.h>
#include "thread.h"
#include "resource.h"

struct process {
    size_t pid;

    thread_t* threads;

    /**
     * The id of the next resource
     */
    resource_t next_resource_id;

    /**
     * A map between the resources and the providers of the resources
     */
    Map resource_provider_map;
};

typedef struct process process_t;

#endif //TOMATKERNEL_PROCESS_H
