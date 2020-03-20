#include <mm/mm.h>
#include <util/def.h>
#include <stb_ds.h>
#include "workspace.h"

static spinlock_t m_workspaces_lock = SPINLOCK_INIT;
static workspace_t* m_workspaces = NULL;

err_t create_workspace(workspace_t* workspace, event_t event, screen_t screen) {
    err_t err = NO_ERROR;

    CHECK_ERROR(workspace != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(event != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(screen != NULL, ERROR_INVALID_PARAM);

    workspace_t ws = mm_allocate(sizeof(workspace_data_t));
    screen->component.ref_count++;
    ws->screen = screen;
    ws->event = event;
    ws->messages = NULL;
    ws->messages_lock = SPINLOCK_INIT;

    *workspace = ws;

cleanup:
    return err;
}

err_t workspace_pull_message(workspace_t workspace, message_t* message) {
    err_t err = NO_ERROR;

    CHECK_ERROR(workspace != NULL, ERROR_INVALID_PARAM);
    spinlock_acquire(&workspace->messages_lock);
    CHECK_ERROR(message != NULL, ERROR_INVALID_PARAM);

    // make sure we have a message
    CHECK_ERROR(arrlen(workspace->messages) > 0, ERROR_NOT_READY);

    // copy it and remove from the array
    // TODO: this is not efficient :(
    *message = workspace->messages[0];
    arrdel(message, 0);

cleanup:
    if (UNLIKELY(workspace != NULL)) {
        spinlock_release(&workspace->messages_lock);
    }
    return err;
}

err_t start_workspace(workspace_t workspace) {
    err_t err = NO_ERROR;

    spinlock_acquire(&m_workspaces_lock);

    CHECK_ERROR(workspace->start_index < 0, ERROR_ALREADY_STARTED);

    // add it
    workspace->start_index = arrlen(m_workspaces);
    arrpush(m_workspaces, workspace);

cleanup:
    spinlock_release(&m_workspaces_lock);
    return err;
}

err_t stop_workspace(workspace_t workspace) {
    err_t err = NO_ERROR;

    spinlock_acquire(&m_workspaces_lock);

    CHECK_ERROR(workspace->start_index >= 0, ERROR_NOT_READY);

    // remove from the workspace list if need to
    arrdel(m_workspaces, workspace->start_index);

cleanup:
    spinlock_release(&m_workspaces_lock);
    return err;
}

err_t close_workspace(workspace_t workspace) {
    err_t err = NO_ERROR;

    spinlock_acquire(&m_workspaces_lock);

    CHECK_ERROR(workspace != NULL, ERROR_INVALID_PARAM);

    if (workspace->start_index >= 0) {
        WARN(!IS_ERROR(stop_workspace(workspace)), "Failed to stop workspace, still freeing");
    }

    // free the workspace data
    mm_free(workspace);

cleanup:
    spinlock_release(&m_workspaces_lock);
    return err;
}

err_t workspace_queue_message(message_t* message) {
    err_t err = NO_ERROR;

    spinlock_acquire(&m_workspaces_lock);

    // push it and signal that there is a new message
    arrpush(m_workspaces[0]->messages, *message);
    CHECK_AND_RETHROW(signal_event(m_workspaces[0]->event));

cleanup:
    spinlock_release(&m_workspaces_lock);
    return err;
}
