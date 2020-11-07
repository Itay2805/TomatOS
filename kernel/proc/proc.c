#include <util/except.h>
#include "proc.h"

static err_t delete_kernel(handle_meta_t* proc) {
    err_t err = NO_ERROR;

    CHECK_FAIL("Tried to kill kernel!");

cleanup:
    return err;
}

process_t g_kernel = {
    .handle_meta = {
        .kind = HANDLE_PROCESS,
        .refcount = 1,
        .dtor = delete_kernel,
    },
    .pid = 0,
    .name = "kernel",
};
