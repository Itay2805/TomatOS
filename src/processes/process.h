#ifndef TOMATOS_PROCESS_H
#define TOMATOS_PROCESS_H

#include <memory/vmm.h>
#include "thread.h"

typedef size_t pid_t;

typedef struct process {
    // TODO: Move to a kobject
    int refcount;
    spinlock_t lock;

    // the process id
    pid_t pid;

    // The address space of the process
    address_space_t address_space;

    // is this a kernel process
    bool kernel;

    // the priority of the process
    int priority;

    // the threads of the process
    struct {
        tid_t key;
        thread_t* value;
    }* threads;

    // the next tid to generate
    // starts from 0
    tid_t next_tid;

    // TODO: resources
    // TODO: default signal routing
} process_t;

#endif //TOMATOS_PROCESS_H
