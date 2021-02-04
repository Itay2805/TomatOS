#include <task/task.h>
#include <task/sched.h>
#include "main.h"

task(main_task, (), (
    TRACE("Hello from main task!");

    while(1);
));

void queue_main_task() {
    task_t* task = create_task(main_task(), "main_task");
    queue_task(task);
}
