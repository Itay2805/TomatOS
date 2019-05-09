#include "context_manager.h"

ata_resource_context_t** ata_contexts;

error_t ata_context_get(resource_t res, ata_resource_context_t **context) {
    error_t err = NO_ERROR;
    ata_resource_context_t* found = NULL;

    for(ata_resource_context_t** it = ata_contexts; it < buf_end(ata_contexts); it++) {
        if(*it != NULL && (*it)->resource == res) {
            found = *it;
            break;
        }
    }

    CHECK_ERROR(found != NULL, ERROR_INVALID_RESOURCE);

    *context = found;

    cleanup:
    return err;
}

error_t ata_context_remove(resource_t res) {
    error_t err = NO_ERROR;
    ata_resource_context_t** found = NULL;

    for(ata_resource_context_t** it = ata_contexts; it < buf_end(ata_contexts); it++) {
        if(*it != NULL && (*it)->resource == res) {
            found = it;
            break;
        }
    }

    CHECK_ERROR(found != NULL, ERROR_INVALID_RESOURCE);

    // delete the context
    mm_free(&kernel_memory_manager, *found);
    *found = NULL;

    cleanup:
    return err;
}

error_t ata_context_add(ata_resource_context_t *context) {
    ata_resource_context_t** slot = NULL;

    for(ata_resource_context_t** it = ata_contexts; it < buf_end(ata_contexts); it++) {
        if(*it == NULL) {
            slot = it;
            break;
        }
    }

    if(slot != NULL) {
        *slot = context;
    }else {
        buf_push(ata_contexts, context);
    }

    return NO_ERROR;
}

