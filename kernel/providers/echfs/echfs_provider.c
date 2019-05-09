//
#include <resource/resource_manager.h>
#include <common/klib.h>
#include <process/process.h>
#include <common/string.h>
#include <common/map.h>
#include "echfs_provider.h"

////////////////////////////////////////////////////////////////////////////
// The process functions
////////////////////////////////////////////////////////////////////////////

static resource_provider_t echfs_provider = {0};

typedef struct resource_context {
    resource_t base;
    int ptr;
} resource_context_t;

static map_t resource_context_map = {0};

static uint64_t hash_resource(int pid, int tid, resource_t resource) {
    uint64_t values[] = {(uint64_t) pid, (uint64_t) tid, (uint64_t) resource };
    return hash_bytes(&values, sizeof(values));
}

static error_t handle_close(process_t* process, int tid, resource_t resource);

static void start() {
    tkill(0);
}

static error_t handle_open(process_t* process, int tid, resource_descriptor_t* descriptor, resource_t* resource) {
    error_t err = NO_ERROR;
    resource_t created_resource = 0;
    resource_descriptor_t* desc = NULL;
    resource_t sub_resource = NULL;

    CHECK_ERROR(descriptor != NULL, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(resource != NULL, ERROR_INVALID_ARGUMENT);

    // get the sub resource
    CHECK_AND_RETHROW(copy_resource_descriptor_to_kernel(process, descriptor, &desc));
    CHECK_ERROR(desc->sub != NULL, ERROR_INVALID_ARGUMENT);
    CHECK(open(desc->sub, &sub_resource));

    // resolve the path
    // TODO

    // create the resource
    CHECK_AND_RETHROW(resource_create(process, &echfs_provider, &created_resource));

    // allocate the context
    resource_context_t* context = mm_allocate(&kernel_memory_manager, sizeof(resource_context_t));
    map_put_from_uint64(&resource_context_map, hash_resource(process->pid, tid, created_resource), context);
    context->base = sub_resource;
    context->ptr = 0;

    // copy to user
    CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &created_resource, resource, sizeof(resource_t)));

cleanup:
    if(IS_ERROR(err)) {
        // close
        if(created_resource != NULL) handle_close(process, tid, created_resource);
    }
    if(desc != NULL) {
        delete_resource_descriptor(desc);
    }
    return err;
}

static error_t handle_close(process_t* process, int tid, resource_t resource) {
    error_t err = NO_ERROR;

    // get the context
    resource_context_t* context = map_get_from_uint64(&resource_context_map, hash_resource(process->pid, tid, resource));
    CHECK_ERROR(context != NULL, ERROR_NOT_FOUND);

    // close the sub resource
    close(context->base);

    // remove the context
    mm_free(&kernel_memory_manager, context);
    map_put_from_uint64(&resource_context_map, hash_resource(process->pid, tid, resource), NULL);

    // TODO: Remove the resource from the kernel

cleanup:
    return err;
}

static error_t handle_read(process_t* process, int tid, resource_t resource, char* buffer, size_t len, size_t* read_size) {
    error_t err = NO_ERROR;

cleanup:
    return err;
}

static error_t handle_tell(process_t* process, int tid, resource_t resource, size_t* pos) {
    error_t err = NO_ERROR;

cleanup:
    return err;
}

static error_t handle_seek(process_t* process, int tid, resource_t resource, int type, ptrdiff_t pos) {
    error_t err = NO_ERROR;



cleanup:
    return err;
}

////////////////////////////////////////////////////////////////////////////
// Kernel initialization
////////////////////////////////////////////////////////////////////////////

error_t echfs_provider_init() {
    process_t* process = process_create(start, true);
    thread_kill(&process->threads[0]);

    echfs_provider.scheme = "echfs";
    echfs_provider.open = handle_open;
    echfs_provider.close = handle_close;
    echfs_provider.read = handle_read;
    // TODO Write support
    echfs_provider.seek = handle_seek;
    echfs_provider.tell = handle_tell;

    return NO_ERROR;
}
