#ifndef TOMATKERNEL_LOCK_H
#define TOMATKERNEL_LOCK_H

#include <libc/stdbool.h>

typedef volatile uint32_t lock_t;

void acquire_lock(lock_t *lock);
bool acquire_lock_or_fail(lock_t *lock);
void release_lock(lock_t* lock);

uint32_t interlocked_increment(volatile uint32_t* value);
uint32_t interlocked_decrement(volatile uint32_t* value);
uint32_t interlocked_compare_exchange(volatile uint32_t* value, uint32_t comapre, uint32_t exchange);

#endif //TOMATKERNEL_LOCK_H
