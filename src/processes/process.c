#include <stdlib.h>
#include <stb/stb_ds.h>
#include "process.h"

process_t* kernel_process;
spinlock_t processes_lock;
process_map_entry_t* processes;

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

    // add the process to the processes list
    lock_preemption(&processes_lock);
    hmput(processes, new_proc->pid, new_proc);
    unlock_preemption(&processes_lock);

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
        CHECK_TRACE(hmlen(process->threads) == 0, "Tried to free a process with running threads");

        kfree(process);
        goto deleted;
    }

cleanup:
    unlock_preemption(&process->lock);
deleted:
    return err;
}

error_t process_kill(process_t* process, registers_t* regs) {
    error_t err = NO_ERROR;

    CHECK(process);

    lock_preemption(&process->lock);

    // kill all the threads
    for(int i = 0; i < hmlen(process->threads); i++) {
        CHECK_AND_RETHROW(thread_kill(process->threads[i].value, regs));
    }

cleanup:
    unlock_preemption(&process->lock);
    return err;
}
