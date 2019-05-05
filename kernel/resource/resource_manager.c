#include "resource_manager.h"

#include <common/buf.h>
#include <common/string.h>

resource_provider_t* providers = NULL;

extern void syscall_queue_trampoline_start();
extern void syscall_queue_trampoline_end();

static uint64_t *syscall_resume_thread(uint64_t pid, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f) {
        
}

error_t resource_manager_init() {

}

error_t resource_manager_register_provider(resource_provider_t* provider) {
    term_print("[resource_manager_register] registered provider for '%s' (pid=%d)", provider->scheme, provider->pid);
    buf_push(providers, *provider);
    return NO_ERROR;
}


