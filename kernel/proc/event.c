#include <mem/mm.h>
#include "event.h"

static err_t free_event(handle_meta_t* meta) {
    err_t err = NO_ERROR;
    event_t event;

    // get the event
    CHECK(meta != NULL);
    event = CR(meta, event_data_t, handle_meta);

    // TODO: anything needed

cleanup:
    // free it no matter what
    if (meta != NULL) {
        kfree(event);
    }
    return err;
}

err_t create_event(event_t* event) {
    err_t err = NO_ERROR;

    CHECK(event != NULL);
    *event = kalloc(sizeof(event_data_t));
    CHECK_ERROR(*event != NULL, ERROR_OUT_OF_RESOURCES);

    (*event)->handle_meta.kind = HANDLE_EVENT;
    (*event)->handle_meta.refcount = 1;
    (*event)->handle_meta.dtor = free_event;
    (*event)->signaled = false;

cleanup:
    return err;
}

err_t signal_event(event_t event) {
    err_t err = NO_ERROR;

    CHECK_FAIL_TRACE("TODO");

cleanup:
    return err;
}

err_t close_event(event_t event) {
    err_t err = NO_ERROR;

    CHECK_AND_RETHROW(release_handle_meta(&event->handle_meta));

cleanup:
    return err;
}
