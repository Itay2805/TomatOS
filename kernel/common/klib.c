//
// Created by Itay on 09/05/2019.
//

#include "klib.h"
#include "string.h"
#include "stdarg.h"
#include "mini-printf.h"
#include <process/syscalls.h>
#include <process/scheduler.h>

error_t forward_open(thread_t* thread, resource_descriptor_t* desc, resource_t* resource) {
    error_t err = NO_ERROR;
    resource_provider_t* provider = NULL;

    CHECK_AND_RETHROW(resource_manager_get_provider_by_scheme(desc->scheme, &provider));
    CHECK_AND_RETHROW(provider->open(thread->parent, thread, desc, resource));

cleanup:
    return err;
}

error_t forward_read(thread_t* thread, resource_t res, void* buffer, int size, size_t* outSize) {
    error_t err = NO_ERROR;
    resource_provider_t* provider = NULL;

    CHECK_AND_RETHROW(resource_manager_get_provider_by_resource(thread->parent, res, &provider));
    CHECK_AND_RETHROW(provider->read(thread->parent, thread, res, buffer, size, outSize));

cleanup:
    return err;
}

error_t forward_write(thread_t* thread, resource_t res, const void* buffer, int size, size_t* outSize) {
    error_t err = NO_ERROR;
    resource_provider_t* provider = NULL;

    CHECK_AND_RETHROW(resource_manager_get_provider_by_resource(thread->parent, res, &provider));
    CHECK_AND_RETHROW(provider->write(thread->parent, thread, res, buffer, size, outSize));

cleanup:
    return err;
}

error_t forward_seek(thread_t* thread, resource_t res, int relative, ptrdiff_t offset) {
    error_t err = NO_ERROR;
    resource_provider_t* provider = NULL;

    CHECK_AND_RETHROW(resource_manager_get_provider_by_resource(thread->parent, res, &provider));
    CHECK_AND_RETHROW(provider->seek(thread->parent, thread, res, relative, offset));

cleanup:
    return err;
}

error_t forward_tell(thread_t* thread, resource_t res, uint64_t* offset) {
    error_t err = NO_ERROR;
    resource_provider_t* provider = NULL;

    CHECK_AND_RETHROW(resource_manager_get_provider_by_resource(thread->parent, res, &provider));
    CHECK_AND_RETHROW(provider->tell(thread->parent, thread, res, offset));

cleanup:
    return err;
}

error_t forward_close(thread_t* thread, resource_t res) {
    error_t err = NO_ERROR;
    resource_provider_t* provider = NULL;

    CHECK_AND_RETHROW(resource_manager_get_provider_by_resource(thread->parent, res, &provider));
    CHECK_AND_RETHROW(provider->close(thread->parent, thread, res));

cleanup:
    return err;
}

error_t forward_poll(thread_t* thread, resource_t res) {
    error_t err = NO_ERROR;
    resource_provider_t* provider = NULL;

    CHECK_AND_RETHROW(resource_manager_get_provider_by_resource(thread->parent, res, &provider));
    CHECK_AND_RETHROW(provider->poll(thread->parent, thread, res));

cleanup:
    return err;
}

error_t forward_invoke(thread_t* thread, resource_t res, uint64_t command, void* arg) {
    error_t err = NO_ERROR;
    resource_provider_t* provider = NULL;

    CHECK_AND_RETHROW(resource_manager_get_provider_by_resource(thread->parent, res, &provider));
    CHECK_AND_RETHROW(provider->invoke(thread->parent, thread, res, command, arg));

cleanup:
    return err;
}

error_t forward_readdir(thread_t* thread, resource_t res, resource_stat_t* stat) {
    error_t err = NO_ERROR;
    resource_provider_t* provider = NULL;

    CHECK_AND_RETHROW(resource_manager_get_provider_by_resource(thread->parent, res, &provider));
    CHECK_AND_RETHROW(provider->readdir(thread->parent, thread, res, stat));

cleanup:
    return err;
}

error_t kopen(resource_descriptor_t* desc, resource_t* resource) {
    return forward_open(running_thread, desc, resource);
}

error_t kread(resource_t res, void* buffer, int size, size_t* outSize) {
    return forward_read(running_thread, res, buffer, size, outSize);
}

error_t kwrite(resource_t res, const void* buffer, int size, size_t* outSize) {
    return forward_write(running_thread, res, buffer, size, outSize);
}

error_t kseek(resource_t res, int relative, ptrdiff_t offset) {
    return forward_seek(running_thread, res, relative, offset);
}

error_t ktell(resource_t res, uint64_t* offset) {
    return forward_tell(running_thread, res, offset);
}

error_t kinvoke(resource_t res, uint64_t command, void* arg) {
    return forward_invoke(running_thread, res, command, arg);
}

error_t kclose(resource_t res) {
    return forward_close(running_thread, res);
}

error_t kpoll(resource_t res) {
    return forward_poll(running_thread, res);
}

error_t kreaddir(resource_t res, resource_stat_t* stat) {
    return forward_readdir(running_thread, res, stat);
}

bool wait(resource_t res) {
    bool result;
    asm volatile ("int $0x80" : "=a"(result) : "a"(SYSCALL_WAIT), "D"(res));
    return result;
}

void fprintf(resource_t res, const char* fmt, ...) {
    va_list list;
    va_start(list, fmt);
    char buffer[1024];
    mini_vsnprintf(buffer, 1024u, fmt, list);
    kwrite(res, buffer, sizeof(buffer), NULL);
    va_end(list);
}


bool tkill(int tid) {
    bool result;
    asm volatile ("int $0x80" : "=a"(result) : "a"(SYSCALL_THREAD_KILL), "D"(tid));
    return result;
}

