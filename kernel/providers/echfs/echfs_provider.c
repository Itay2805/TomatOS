//
#include <resource/resource_manager.h>
#include <common/klib.h>
#include <process/process.h>
#include <common/string.h>
#include <common/map.h>

#include "echfs.h"
#include "echfs_provider.h"

////////////////////////////////////////////////////////////////////////////
// The process functions
////////////////////////////////////////////////////////////////////////////

static resource_provider_t echfs_provider = {0};

typedef struct resource_context {
    echfs_directory_entry_t entry;
    resource_t base;
    uint64_t ptr;
} resource_context_t;

static map_t resource_context_map = {0};

static error_t handle_close(process_t* process, thread_t* thread, resource_t resource);

static error_t handle_open(process_t* process, thread_t* thread, resource_descriptor_t* descriptor, resource_t* resource) {
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

    // create the resource
    CHECK_AND_RETHROW(resource_create(process, &echfs_provider, &created_resource));

    // allocate the context
    resource_context_t* context = mm_allocate(&kernel_memory_manager, sizeof(resource_context_t));
    map_put_from_uint64(&resource_context_map, hash_resource(process->pid, created_resource), context);
    context->base = sub_resource;
    context->ptr = 0;

    // resolve the path
    if(strlen(desc->path) == 0) {
        context->entry.parent_id = ECHFS_DIR_ID_END_OF_DIR;
        context->entry.type = ECHFS_OBJECT_TYPE_DIR;
        context->entry.dir_id = ECHFS_DIR_ID_ROOT;
    }else {
        CHECK_AND_RETHROW(echfs_resolve_path(sub_resource, desc->path, &context->entry));
    }

    // copy to user
    CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &created_resource, resource, sizeof(resource_t)));

cleanup:
    if(IS_ERROR(err)) {
        // close
        if(created_resource != NULL) handle_close(process, thread, created_resource);
    }
    if(desc != NULL) {
        delete_resource_descriptor(desc);
    }
    return err;
}

static error_t handle_close(process_t* process, thread_t* thread, resource_t resource) {
    error_t err = NO_ERROR;

    // get the context
    resource_context_t* context = map_get_from_uint64(&resource_context_map, hash_resource(process->pid, resource));
    CHECK_ERROR(context != NULL, ERROR_NOT_FOUND);

    // close the sub resource
    close(context->base);

    // remove the context
    mm_free(&kernel_memory_manager, context);
    map_put_from_uint64(&resource_context_map, hash_resource(process->pid, resource), NULL);

    // Remove the resource from the kernel 
    CHECK_AND_RETHROW(resource_remove(process, resource));

cleanup:
    return err;
}

// TODO: Maybe store the closest chain so we can quickly get to the offset
static error_t handle_read(process_t* process, thread_t* thread, resource_t resource, char* buffer, size_t len, size_t* read_size) {
    error_t err = NO_ERROR;
    size_t bytes_read = 0;
    char* kbuffer = NULL;

    // get the context
    resource_context_t* context = map_get_from_uint64(&resource_context_map, hash_resource(process->pid, resource));
    CHECK_ERROR(context != NULL, ERROR_NOT_FOUND);

    // only read from files
    CHECK_ERROR(context->entry.type == ECHFS_OBJECT_TYPE_FILE, ERROR_NOT_READABLE);

    // get the min len we can read
    len = MIN(context->entry.size, context->ptr + len);
    kbuffer = mm_allocate(&kernel_memory_manager, len);
    CHECK_AND_RETHROW(echfs_read_from_chain(context->base, context->entry.data_start, kbuffer, context->ptr, len, &bytes_read));

    CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, kbuffer, buffer, len));

    context->ptr += bytes_read;
    if(read_size != NULL) CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &bytes_read, read_size, sizeof(size_t)));

cleanup:
    if(kbuffer != NULL) {
        mm_free(&kernel_memory_manager, kbuffer);
    }
    return err;
}

static error_t handle_tell(process_t* process, thread_t* thread, resource_t resource, size_t* pos) {
    error_t err = NO_ERROR;

    // get the context
    resource_context_t* context = map_get_from_uint64(&resource_context_map, hash_resource(process->pid, resource));
    CHECK_ERROR(context != NULL, ERROR_NOT_FOUND);

    // only only tell on files
    CHECK_ERROR(context->entry.type == ECHFS_OBJECT_TYPE_FILE, ERROR_NOT_SEEKABLE);

    // copy offset to user
    CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &context->ptr, pos, sizeof(size_t)));

cleanup:
    return err;
}

static error_t handle_seek(process_t* process, thread_t* thread, resource_t resource, int type, ptrdiff_t pos) {
    error_t err = NO_ERROR;

    // get the context
    resource_context_t* context = map_get_from_uint64(&resource_context_map, hash_resource(process->pid, resource));
    CHECK_ERROR(context != NULL, ERROR_NOT_FOUND);

    CHECK_ERROR(context->entry.type == ECHFS_OBJECT_TYPE_FILE, ERROR_NOT_SEEKABLE);

    switch(type) {
        case SEEK_CUR:
            context->ptr += pos;
            break;

        case SEEK_END:
            context->ptr = context->entry.size - pos;
            break;

        case SEEK_START:
            context->ptr = (size_t) pos;
            break;

        default:
            CHECK_FAIL_ERROR(ERROR_NOT_IMPLEMENTED);
    }

    CHECK_ERROR(context->ptr <= context->entry.size, ERROR_OUT_OF_RANGE);

cleanup:
    if(IS_ERROR(err)) {
        context->ptr = context->entry.size;
    }
    return err;
}

static error_t handle_invoke(process_t* process, thread_t* thread, resource_t resource, int cmd, void* arg) {
    error_t err = NO_ERROR;
    echfs_directory_entry_t entry;

    // get the context
    resource_context_t* context = map_get_from_uint64(&resource_context_map, hash_resource(process->pid, resource));
    CHECK_ERROR(context != NULL, ERROR_NOT_FOUND);

    switch(cmd) {
        case ECHFS_READ_DIR:
            CHECK_ERROR(context->entry.type == ECHFS_OBJECT_TYPE_DIR, ERROR_NOT_IMPLEMENTED);
            CHECK_AND_RETHROW(echfs_read_dir(context->base, context->entry.dir_id, &context->ptr, &entry));
            CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &entry, arg, sizeof(echfs_directory_entry_t)));
            break;

        default:
            CHECK_FAIL_ERROR(ERROR_NOT_IMPLEMENTED);
    }

cleanup:
    return err;
}

////////////////////////////////////////////////////////////////////////////
// Kernel initialization
////////////////////////////////////////////////////////////////////////////

error_t echfs_provider_init() {
    error_t err = NO_ERROR;

    process_t* process = process_create(NULL, true);
    thread_kill(process->threads[0]);

    echfs_provider.scheme = "echfs";
    echfs_provider.pid = process->pid;
    echfs_provider.open = handle_open;
    echfs_provider.close = handle_close;
    echfs_provider.read = handle_read;
    // TODO Write support
    echfs_provider.seek = handle_seek;
    echfs_provider.tell = handle_tell;
    echfs_provider.invoke = handle_invoke;

    CHECK_AND_RETHROW(resource_manager_register_provider(&echfs_provider));

cleanup:
    return err;
}
