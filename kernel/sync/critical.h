#ifndef __TOMATOS_KERNEL_SYNC_CRITICAL_H__
#define __TOMATOS_KERNEL_SYNC_CRITICAL_H__

#include <stdbool.h>

typedef struct critical {
    bool enable_int;
} critical_t;

void enter_critical(critical_t* crit);
void exit_critical(critical_t* crit);

#endif //__TOMATOS_KERNEL_SYNC_CRITICAL_H__
