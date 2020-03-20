#ifndef __COMPO_WM_WORKSPACE_H__
#define __COMPO_WM_WORKSPACE_H__

#include <event/event.h>
#include <compo/component.h>
#include <sync/spinlock.h>
#include "compo/screen/screen.h"

typedef enum message_type {
    /**
     * a new window was created
     *
     * - kernel -> workspace
     *
     * arg0 - window id
     */
    MESSAGE_CREATED_WINDOW,

    /**
     * a window have been closed
     *
     * - kernel -> workspace
     *
     * arg0 - window id
     */
    MESSAGE_CLOSED_WINDOW,

    /**
     * keyboard key state changed
     *
     * - kernel -> workspace
     * - workspace -> window
     *
     * arg0 - the vkey
     * arg1 - up or down
     * arg2&arg3 - keyboard address
     */
    MESSAGE_KEY,

    /**
     * mouse button state changed
     *
     * - kernel -> workspace
     * - workspace -> window
     *
     * arg0 - the vbutton
     * arg1 - up or down
     * arg2 - the mouse x
     * arg3 - the mouse y
     * arg4 & arg5 - mouse address
     */
    MESSAGE_MOUSE_BUTTON,

    /**
     * Mouse moved
     *
     * - kernel -> workspace
     * - workspace -> window
     *
     * arg0 - delta x
     * arg1 - delta y
     * arg2 & arg3 - mouse address
     */
    MESSAGE_MOUSE_MOVE,

    /**
     * Mouse scroll
     *
     * - kernel -> workspace
     * - workspace -> window
     *
     * arg0 - delta x
     * arg1 - delta y
     * arg2 & arg3 - mouse address
     */
    MESSAGE_MOUSE_SCROLL,

    /**
     * Request to terminate the window
     *
     * - workspace -> window
     */
    MESSAGE_TERMINATE,
} message_type_t;

typedef struct message {
    message_type_t type;
    uint64_t arg0;
    uint64_t arg1;
    uint64_t arg2;
    uint64_t arg3;
    uint64_t arg4;
    uint64_t arg5;
} message_t;

typedef struct workspace_data {
    // events for communicating with the workspace
    event_t event;

    // the screen attached to this workspace
    screen_t screen;

    // the messages
    // TODO: turn into a cyclic buffer for better performance
    spinlock_t messages_lock;
    message_t* messages;

    // index in the started workspace list
    int start_index;
} workspace_data_t;

typedef workspace_data_t* workspace_t;

/**
 * This will create a new workspace with the given event and screen to back it
 *
 * @param workspace     [OUT]   The new workspace
 * @param event         [IN]    The event that will be signaled
 * @param screen        [IN]    The screen to back this, NULL for primary one
 */
err_t create_workspace(workspace_t* workspace, event_t event, screen_t screen);

/**
 * Pull a message from the workspace
 *
 * @param workspace     [IN]    The workspace to pull a message from
 * @param message       [OUT]   The message
 */
err_t workspace_pull_message(workspace_t workspace, message_t* message);

/**
 * Start the workspace, that work space will be the one
 * receiving new events. The prev workspace will still
 * be running in the background and will receive events
 * related to child processes.
 *
 * @param workspace     [IN]    The workspace to start
 */
err_t start_workspace(workspace_t workspace);

/**
 * Stop the given workspace, will make the prev working
 * workspace receive input related events again
 *
 * @param workspace     [IN]    The workspace to stop
 */
err_t stop_workspace(workspace_t workspace);

/**
 * Will close the workspace
 *
 * @param workspace     [IN] The workspace to close
 */
err_t close_workspace(workspace_t workspace);

/**
 * Queue a message for the currently running workspace
 *
 * @remark
 * This will drop the message if there is no running workspace
 *
 * @param message   [IN] The message to queue
 */
err_t workspace_queue_message(message_t* message);

#endif //__COMPO_WM_WORKSPACE_H__
