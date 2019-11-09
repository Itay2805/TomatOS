#include <libc/string.h>
#include "process.h"

lock_t processes_list = {0};
list_entry_t processes = INIT_LIST_ENTRY(processes);
process_t kernel_process = {0};

void kernel_process_init() {
    process_init(&kernel_process);

    // just set the vmm handle
    kernel_process.vmm_handle = kernel_handle;
}

void process_init(process_t* process) {
    ASSERT(process != NULL);

    memset(process, 0, sizeof(process_t));

    // initialize the rest of the stuff
    process->threads = INIT_LIST_ENTRY(process->threads);

    // initialize the object
    process->object.handle = generate_object_handle();
    process->object.type = OBJECT_PROCESS;

    // add to process list
    acquire_lock(&processes_list);
    insert_tail_list(&processes, &process->object.link);
    release_lock(&processes_list);
}
