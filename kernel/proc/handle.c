#include <util/except.h>
#include "handle.h"

err_t release_handle_meta(handle_meta_t* meta) {
    err_t err = NO_ERROR;

    CHECK(meta != NULL);

    if (atomic_fetch_sub(&meta->refcount, 1) - 1 == 0) {
        CHECK(meta->dtor != NULL);
        CHECK_AND_RETHROW(meta->dtor(meta));
    }

cleanup:
    return err;
}
