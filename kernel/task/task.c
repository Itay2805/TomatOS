#include <util/string.h>
#include "task.h"

task_t* create_task(task_handle_t handle, const char* name, ...) {
    // did not get a task handle, meaning we failed to create
    // the task handle, return null
    if (handle == NULL) {
        return NULL;
    }

    // get the task an initialize it
    task_t* task = __builtin_coro_promise(handle, 0, false);
    strncpy(task->name, name, sizeof(task->name));

    return task;
}
