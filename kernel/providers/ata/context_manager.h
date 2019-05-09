//
// Created by Itay on 09/05/2019.
//

#ifndef TOMATKERNEL_IDE_CONTEXT_MANAGEMENT_H
#define TOMATKERNEL_IDE_CONTEXT_MANAGEMENT_H

#include <resource/resource.h>

#include <common/string.h>
#include <common/except.h>

#include <locks/rwlock.h>

#include "ata_provider.h"

/**
 * This represents a resource entry and has the info required for doing everything we need
 */
typedef struct ata_resource_context {
    /**
     * The resource this context is attached to
     */
    resource_t resource;

    /**
     * The entry this resource uses
     */
    ata_entry_t* entry;

    /**
     * Read/Write, so only one can read and one can write
     */
    rwlock_t rwlock;

    // TODO: Different pointer for read and write
    /**
     * The pointer used for reading/writing
     */
    int ptr;
} ata_resource_context_t;

extern ata_resource_context_t** ata_contexts;

error_t ata_context_get(resource_t res, ata_resource_context_t **context);
error_t ata_context_remove(resource_t res);
error_t ata_context_add(ata_resource_context_t *context);

#endif //TOMATKERNEL_IDE_CONTEXT_MANAGEMENT_H
