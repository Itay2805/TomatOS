#include "process.h"

static err_t delete_kernel(handle_meta_t* proc) {
    err_t err = NO_ERROR;

    CHECK_FAIL_TRACE("Tried to kill kernel!");

cleanup:
    return err;
}

process_t g_kernel = {
    .handle_meta = {
        .kind = HANDLE_PROCESS,
        .refcount = 1,
        .dtor = (handle_dtor_t)delete_kernel,
    },
    .handles_lock = INIT_LOCK(),
    .handles = NULL,
    .next_handle = 1,
};
