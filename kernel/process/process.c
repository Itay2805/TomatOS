//
// Created by Itay on 24/03/2019.
//

#include <common/buf.h>
#include <common/string.h>
#include <resource/resource.h>
#include "process.h"

process_t* processes = NULL;

static size_t alive_processes = 0;

static size_t next_pid = 1;

process_t* process_create(thread_start_f start, bool kernel) {
    process_t* proc = NULL;

    // should we even search
    if(alive_processes < buf_len(processes)) {
        for(process_t* it = processes; it < buf_end(processes); it++) {
            if(it->pid == DEAD_PROCESS_PID) {
                proc = it;
                break;
            }
        }
    }
    if(proc == NULL) {
        // add a new one
        buf_push(processes, (process_t){0});
        proc = &processes[buf_len(processes) - 1];
    }

    proc->pid = next_pid++;
    proc->next_resource = 1;
    proc->next_tid = 1;
    proc->kernel = kernel;

    if(kernel) {
        // for a kernel process use the kernel address space
        proc->address_space = kernel_address_space;
    }else {
        // initialize using a new address space
        proc->address_space = vmm_create_address_space();
    }

    buf_push(proc->threads, (thread_t) {
            .start = start,
            .parent = proc
    });

    thread_init(&proc->threads[0]);

    return proc;
}

error_t process_find(size_t pid, process_t** process) {
    error_t err = NO_ERROR;
    process_t* proc = NULL;

    for(process_t* it = processes; it < buf_end(processes); it++) {
        if(it->pid == pid) {
            process = it;
            break;
        }
    }
    
    CHECK_ERROR(proc != NULL, ERROR_NOT_FOUND);
    
    if(process != NULL) *process = proc;

cleanup:
    return err;
}

process_t* process_find(size_t pid) {
    return NULL;
}

thread_t* process_start_thread(process_t* process, thread_start_f start) {
    thread_t* thread = NULL;
    for(thread_t* it = process->threads; it < buf_end(process->threads); it++) {
        if(it->state == THREAD_DEAD) {
            thread = it;
            break;
        }
    }
    if(thread == NULL) {
        // add a new one
        buf_push(process->threads, (thread_t){0});
        thread = &process->threads[buf_len(process->threads) - 1];
    }

    thread->start = start;
    thread->parent = process;

    thread_init(thread);

    return thread;
}

void process_remove(process_t* process) {
    for(thread_t* it = process->threads; it < buf_end(process->threads); it++) {
        if(it->state != THREAD_DEAD) {
            thread_kill(it);
        }
    }

    for(resource_t* it = process->resources; it < buf_end(process->threads); it++) {
        if(*it != 0) {
            resource_queue_close(process, *it);
        }
    }

    if(!process->kernel) {
        // free the address space if not a kernel process
        vmm_free_address_space(process->address_space);
    }

    process->pid = DEAD_PROCESS_PID;
}
