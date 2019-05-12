#include <common/string.h>
#include <common/klib.h>
#include <locks/critical_section.h>
#include <common/map.h>
#include "elf_provider.h"
#include "elf64_loader.h"

resource_provider_t elf_provider;

typedef struct resource_context {
    process_t* new_process;
} resource_context_t;

map_t resource_contexts = {};

static error_t handle_close(process_t* process, thread_t* thread, resource_t resource);

static error_t handle_open(process_t* process, thread_t* thread, resource_descriptor_t* descriptor, resource_t* resource) {
    error_t err = NO_ERROR;
    resource_t created_resource = 0;
    resource_descriptor_t* desc = NULL;
    process_t* new_process = NULL;
    resource_t sub_resource = 0;

    CHECK_ERROR(descriptor != NULL, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(resource != NULL, ERROR_INVALID_ARGUMENT);

    // get the sub resource
    CHECK_AND_RETHROW(copy_resource_descriptor_to_kernel(process, descriptor, &desc));
    CHECK_ERROR(desc->sub, ERROR_INVALID_ARGUMENT);
    CHECK_AND_RETHROW(kopen(desc->sub, &sub_resource));

    // create the resource
    CHECK_AND_RETHROW(resource_create(process, &elf_provider, &created_resource));

    CRITICAL_SECTION({
         new_process = process_create(NULL, false);
         new_process->threads[0]->state = THREAD_SUSPENDED;
    });

    // create the context
    resource_context_t* context = kalloc(sizeof(resource_context_t));
    context->new_process = new_process;
    map_put_from_uint64(&resource_contexts, hash_resource(process->pid, created_resource), context);

    // do the actual loading
    CHECK_AND_RETHROW(load_elf64(new_process, sub_resource));

    // set the thread to normal
    new_process->threads[0]->state = THREAD_NORMAL;

    // copy to user
    CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &created_resource, resource, sizeof(resource_t)));

cleanup:
    if(IS_ERROR(err)) {
        // close
        if(created_resource != NULL) handle_close(process, thread, created_resource);
        if(process != NULL) process_remove(process);
    }
    if(desc != NULL) {
        delete_resource_descriptor(desc);
    }
    if(sub_resource != 0) kclose(sub_resource);
    return err;
}

static error_t handle_close(process_t* process, thread_t* thread, resource_t resource) {
    error_t err = NO_ERROR;
    resource_context_t* context = NULL;

    // get context
    context = map_get_from_uint64(&resource_contexts, hash_resource(process->pid, resource));
    CHECK_ERROR(context != NULL, ERROR_INVALID_RESOURCE);

    // Remove the resource from the kernel
    resource_remove(process, resource);

cleanup:
    return err;
}

error_t elf_provider_init() {
    error_t err = NO_ERROR;
    process_t* process = process_create(NULL, true);
    thread_kill(process->threads[0]);

    elf_provider.scheme = "elf";
    elf_provider.pid = process->pid;
    elf_provider.open = handle_open;
    elf_provider.close = handle_close;

    CHECK_AND_RETHROW(resource_manager_register_provider(&elf_provider));

cleanup:
    return err;
}
