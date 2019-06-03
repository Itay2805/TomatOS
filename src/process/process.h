#ifndef TOMATKERNEL_PROCESS_H
#define TOMATKERNEL_PROCESS_H

#include "thread.h"

typedef struct process {
    // the process pid
    int pid;

    // the next tid to give to the process
    int next_tid;

    // holds a list of all threads
    thread_t** threads;
} process_t;

thread_t* process_get_thread(process_t* process, int tid);

#endif //TOMATKERNEL_PROCESS_H
