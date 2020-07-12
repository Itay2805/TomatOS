#ifndef __TOMATOS_KERNEL_PROC_PROCESS_H__
#define __TOMATOS_KERNEL_PROC_PROCESS_H__

#include <arch/vmm.h>

typedef struct process {
    int pid;

    // TODO: use rwlock instead
    ticket_lock_t lock;

    address_space_t address_space;
} process_t;

/**
 * The kernel process
 */
extern process_t g_kernel;

#endif //__TOMATOS_KERNEL_PROC_PROCESS_H__
