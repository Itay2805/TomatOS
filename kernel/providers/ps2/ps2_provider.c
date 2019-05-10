#include <resource/resource.h>
#include <common/map.h>
#include <common/string.h>
#include <common/logging.h>
#include <process/process.h>
#include <interrupts/irq.h>
#include <locks/critical_section.h>
#include "ps2_provider.h"
#include "ps2.h"

////////////////////////////////////////////////////////////////////////////
// The process functions
////////////////////////////////////////////////////////////////////////////

static resource_provider_t ps2_provider = {0};

#define PS2_KEYBOARD    0
#define PS2_MOUSE       1

typedef struct resource_context {
    resource_t resource;
    thread_t* thread;
    int type;
    char* buf;
} resource_context_t;

resource_context_t** contexts = NULL;

static void add_context(resource_context_t* context) {
    resource_context_t** found = NULL;
    for(resource_context_t** it = contexts; it < buf_end(contexts); it++) {
        if(*it == NULL) {
            found = it;
            break;
        }
    }
    if(found != NULL) {
        *found = context;
    }else {
        buf_push(contexts, context);
    }
}

static error_t get_context(resource_t res, resource_context_t** context) {
    error_t err = NO_ERROR;
    resource_context_t* found = NULL;

    for(resource_context_t** it = contexts; it < buf_end(contexts); it++) {
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

static error_t remove_context(resource_t res) {
    error_t err = NO_ERROR;
    bool found = false;

    for(resource_context_t** it = contexts; it < buf_end(contexts); it++) {
        if(*it != NULL && (*it)->resource == res) {
            mm_free(&kernel_memory_manager, *it);
            *it = NULL;
            break;
        }
    }

    CHECK_ERROR(found, ERROR_INVALID_RESOURCE);

cleanup:
    return err;
}

static error_t handle_close(process_t* process, thread_t* thread, resource_t resource);

static error_t handle_open(process_t* process, thread_t* thread, resource_descriptor_t* descriptor, resource_t* resource) {
    error_t err = NO_ERROR;
    resource_t created_resource = 0;
    resource_descriptor_t* desc = NULL;

    CHECK_ERROR(descriptor != NULL, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(resource != NULL, ERROR_INVALID_ARGUMENT);

    // get the sub resource
    CHECK_AND_RETHROW(copy_resource_descriptor_to_kernel(process, descriptor, &desc));

    // create the resource
    CHECK_AND_RETHROW(resource_create(process, &ps2_provider, &created_resource));

    // allocate and add the context
    resource_context_t* context = mm_allocate(&kernel_memory_manager, sizeof(resource_context_t));
    context->buf = NULL;
    context->resource = created_resource;
    context->thread = thread;
    if(strcmp(desc->domain, "keyboard") == 0) {
        context->type = PS2_KEYBOARD;
    }else if(strcmp(desc->domain, "mouse") == 0) {
        context->type = PS2_MOUSE;
    }else {
        CHECK_FAIL_ERROR(ERROR_INVALID_DOMAIN);
    }
    add_context(context);

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

    // remove the context
    CHECK_AND_RETHROW(remove_context(resource));

    // Remove the resource from the kernel
    CHECK_AND_RETHROW(resource_remove(process, resource));

    cleanup:
    return err;
}

// TODO: Maybe store the closest chain so we can quickly get to the offset
static error_t handle_read(process_t* process, thread_t* thread, resource_t resource, char* buffer, size_t len, size_t* read_size) {
    error_t err = NO_ERROR;
    resource_context_t* context = NULL;

    // get the context
    CHECK_AND_RETHROW(get_context(resource, &context));

    // protect against interrupts modifying this while we modify it
    CRITICAL_SECTION_LABEL(copy_to_user_failed, {
        len = MIN(buf_len(context->buf), len);
        CHECK_AND_RETHROW_LABEL(vmm_copy_to_user(process->address_space, context->buf, buffer, len), copy_to_user_failed);
         memcpy(context->buf, context->buf + len, buf_len(context->buf) - len);
         buf_resize(context->buf, buf_len(context->buf) - len);
    });

    if(read_size != NULL) CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, &len, read_size, sizeof(size_t)));

cleanup:

    return err;
}

static error_t handle_poll(process_t* process, thread_t* thread, resource_t resource) {
    error_t err = NO_ERROR;
    resource_context_t* context = NULL;

    // get the context
    CHECK_AND_RETHROW(get_context(resource, &context));

    CHECK_ERROR(buf_len(context->buf) > 0, ERROR_FINISHED);

cleanup:
    return err;
}

////////////////////////////////////////////////////////////////////////////
// Kernel initialization
////////////////////////////////////////////////////////////////////////////

static void handle_keyboard_interrupt() {
    error_t err = NO_ERROR;
    uint8_t ch;

    CHECK_AND_RETHROW(ps2_read(&ch));
    for(resource_context_t** it = contexts; it < buf_end(contexts); it++) {
        if(*it != NULL && (*it)->type == PS2_KEYBOARD) {
            buf_push((*it)->buf, ch);
            resource_manager_resource_ready((*it)->thread, (*it)->resource);
        }
    }

cleanup:
    if(IS_ERROR(err)) {
        KERNEL_STACK_TRACE(err);
        ERROR_FREE(err);
    }
}

static void handle_mouse_interrupt() {

}

error_t ps2_provider_init() {
    error_t err = NO_ERROR;

    process_t* process = process_create(NULL, true);
    thread_kill(process->threads[0]);

    ps2_provider.scheme = "ps2";
    ps2_provider.wait_support = true;
    ps2_provider.pid = process->pid;
    ps2_provider.open = handle_open;
    ps2_provider.close = handle_close;
    ps2_provider.read = handle_read;
    ps2_provider.poll = handle_poll;

    LOG_INFO("setting interrupt handlers");
    irq_handlers[IRQ_KEYBOARD] = handle_keyboard_interrupt;
    irq_handlers[IRQ_PS2_MOUSE] = handle_mouse_interrupt;

    ps2_init();

    CHECK_AND_RETHROW(resource_manager_register_provider(&ps2_provider));

cleanup:
    return NO_ERROR;
}
