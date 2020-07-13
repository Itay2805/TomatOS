#ifndef __TOMATOS_KERNEL_PROC_PROCESS_H__
#define __TOMATOS_KERNEL_PROC_PROCESS_H__

#include <mem/vmm.h>
#include <arch/cpu.h>
#include "handle.h"

struct thread;

/**
 * This represents a process
 */
typedef struct process {
    handle_meta_t handle_meta;
    address_space_t address_space;
} process_t;

/**
 * This represents a thread
 */
typedef struct thread {
    handle_meta_t handle_meta;
    process_t* parent;
} thread_t;

/**
 * The kernel process
 */
extern process_t g_kernel;

/**
 * Pointer to the current process
 */
extern process_t* CPU_LOCAL g_current_process;

/**
 * Pointer to the current thread
 */
extern thread_t* CPU_LOCAL g_current_thread;

/**
 * This will initialize the kernel process
 */
err_t init_kernel_process();

#endif //__TOMATOS_KERNEL_PROC_PROCESS_H__
