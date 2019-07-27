#include <stdlib.h>
#include <stb/stb_ds.h>
#include "process.h"

process_t* kernel_process;

static pid_t next_pid = 0;

error_t create_process(address_space_t address_space, process_t** process) {
    error_t err = NO_ERROR;

    CHECK(process);

    // allocate and set
    process_t* new_proc = kmalloc(sizeof(process_t));
    new_proc->address_space = address_space;
    new_proc->refcount = 1;
    new_proc->pid = next_pid++;

    *process = new_proc;

cleanup:
    return err;
}

error_t release_process(process_t* process) {
    error_t err = NO_ERROR;

    CHECK(process);

    lock_preemption(&process->lock);

    process->refcount--;

    // each thread has a ref to the process, so we need to take that into account
    if(process->refcount <= 0) {
        // release all the threads
        bool delete = false;
        for(int i = 0; i < hmlen(process->threads); i++) {
            if(!delete) {
                delete = process->threads[i].value->refcount > 1;
            }
            CHECK_AND_RETHROW(release_thread(process->threads[i].value));
        }

        if(delete) {
            // ok, we were the last holders of each thread, we can
            // delete ourselves
            kfree(process);
            goto deleted;
        }
    }

cleanup:
    unlock_preemption(&process->lock);
deleted:
    return err;
}

error_t process_kill(process_t* process) {
    error_t err = NO_ERROR;

    CHECK(process);

    lock_preemption(&process->lock);

    // kill all the threads
    for(int i = 0; i < hmlen(process->threads); i++) {
        CHECK_AND_RETHROW(thread_kill(process->threads[i].value));
    }

cleanup:
    unlock_preemption(&process->lock);
    return err;
}
