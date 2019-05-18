#include <process/thread.h>
#include <common/string.h>
#include <process/process.h>
#include "virtio_provider.h"
#include "virtio.h"

static resource_provider_t virtio_provider = {0};


////////////////////////////////////////////////////////////////////////////
// Kernel initialization
////////////////////////////////////////////////////////////////////////////

error_t virtio_provider_init() {
    error_t err = NO_ERROR;
    process_t* process = NULL;

    err = virtio_init();
    if(IS_ERROR(err)) {
        if(IS_ERROR(err) != ERROR_NOT_FOUND) {
            CHECK_AND_RETHROW(err);
        }else {
            LOG_WARN("No virtio devices found, virtio provider will be disabled");
            ERROR_FREE(err);
            return NO_ERROR;
        }
    }

    process = process_create(NULL, true);
    thread_kill(process->threads[0]);

    virtio_provider.scheme = "virtio";
    virtio_provider.pid = process->pid;
//    virtio_provider.open = handle_open;
//    virtio_provider.close = handle_close;
//    virtio_provider.read = handle_read;
//    virtio_provider.write = handle_write;
//    virtio_provider.tell = handle_tell;
//    virtio_provider.seek = handle_seek;

    CHECK_AND_RETHROW(resource_manager_register_provider(&virtio_provider));

cleanup:
    return err;
}
