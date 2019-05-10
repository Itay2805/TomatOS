//
// Created by Itay on 24/03/2019.
//

#include <common/buf.h>
#include <common/string.h>
#include <resource/resource.h>
#include "process.h"
#include "syscalls.h"
#include "syscall.h"
#include "scheduler.h"

process_t** processes = NULL;

static size_t alive_processes = 0;

static int next_pid = 1;

process_t* process_create(thread_start_f start, bool kernel) {
    // TODO: Proper error handling
    process_t** proc_slot = NULL;
    process_t* proc = NULL;

    // should we even search
    if(alive_processes < buf_len(processes)) {
        for(process_t** it = processes; it < buf_end(processes); it++) {
            if(*it == NULL) {
                proc_slot = it;
                break;
            }
        }
    }
    if(proc_slot == NULL) {
        // add a new one
        buf_push(processes, 0);
        proc_slot = &processes[buf_len(processes) - 1];
    }

    proc = mm_allocate(&kernel_memory_manager, sizeof(process_t));
    memset(proc, 0, sizeof(process_t));
    *proc_slot = proc;

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

    buf_push(proc->threads, thread_create(proc, start));

    return proc;
}

error_t process_find(size_t pid, process_t** process) {
    error_t err = NO_ERROR;
    process_t* proc = NULL;

    for(process_t** it = processes; it < buf_end(processes); it++) {
        if(*it != NULL && (*it)->pid == pid) {
            proc = *it;
            break;
        }
    }
    
    CHECK_ERROR(proc != NULL, ERROR_NOT_FOUND);
    
    if(process != NULL) *process = proc;

cleanup:
    return err;
}

thread_t* process_start_thread(process_t* process, thread_start_f start) {
    thread_t** thread = NULL;
    thread_t* new_thread = thread_create(process, start);
        
    for(thread_t** it = process->threads; it < buf_end(process->threads); it++) {
        if(*it == NULL) {
            thread = it;
            break;
        }
    }

    if(thread == NULL) {
        buf_push(process->threads, new_thread);
    }else {
        *thread = new_thread;
    }

    return new_thread;
}

void process_remove(process_t* process) {
    for(thread_t** it = process->threads; it < buf_end(process->threads); it++) {
        if(*it != NULL && (*it)->state != THREAD_DEAD) {
            thread_kill(*it);
        }
    }

    for(resource_t* it = process->resources; it < buf_end(process->resources); it++) {
        if(*it != 0) {
            (void)it;
            // TODO: Implement resource closing, right now a process has to close all of it's resources or there will be a leak
            // close(process, *it);
        }
    }

    if(!process->kernel) {
        // free the address space if not a kernel process
        vmm_free_address_space(process->address_space);
    }

    // free the process itself and set the slot as empty
    for(process_t** it = processes; it < buf_end(processes); it++) {
        if(*it == process) {
            *it = NULL;
            mm_free(&kernel_memory_manager, process);
            break;
        }
    }
}

static error_t syscall_thread_kill(registers_t* regs) {
    error_t err = NO_ERROR;

    if(regs->rdi == 0 || regs->rdi == running_thread->tid) {
        thread_kill(running_thread);
        running_thread = NULL;
        schedule(regs, 0);
    }else {
        thread_t* thread = NULL;
        CHECK_AND_RETHROW(thread_find(running_thread->parent, regs->rdi, &thread));
        thread_kill(thread);
    }

cleanup:
    return err;
}

error_t process_init() {
    syscalls[SYSCALL_THREAD_KILL] = syscall_thread_kill;
    return NO_ERROR;
}
