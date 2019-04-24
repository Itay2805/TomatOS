//
// Created by Itay on 24/03/2019.
//

#include <memory/vmm.h>
#include <memory/mm.h>
#include <common/common.h>
#include "thread.h"
#include "process.h"
#include "rflags.h"

void thread_init(thread_t* thread) {
    thread->tid = thread->parent->next_tid++;

    thread->state = THREAD_NORMAL;

    if(thread->parent->kernel) {
        // will have the ring set to 0
        thread->cpu_state.rflags = RFLAGS_DEFAULT;
        thread->cpu_state.ss = GDT_KERNEL_DATA;
        thread->cpu_state.ds = GDT_KERNEL_DATA;
        thread->cpu_state.cs = GDT_KERNEL_CODE;
    }else {
        // will have the ring set to 3
        thread->cpu_state.rflags = RFLAGS_DEFAULT_USER;
        thread->cpu_state.ss = GDT_USER_DATA;
        thread->cpu_state.ds = GDT_USER_DATA;
        thread->cpu_state.cs = GDT_USER_CODE;
    }

    thread->cpu_state.rip = (uint64_t) thread->start;
}

void thread_kill(thread_t* thread) {
    thread->state = THREAD_DEAD;
}