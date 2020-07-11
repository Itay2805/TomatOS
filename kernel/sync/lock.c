#include <stddef.h>
#include "lock.h"

void acquire_lock(lock_t* lock) {
    size_t ticket = atomic_fetch_add_explicit(&lock->next_ticket, 1, memory_order_relaxed);
    while (atomic_load_explicit(&lock->now_serving, memory_order_acquire) != ticket) {
        // TODO: cpu relax
    }
}

void release_lock(lock_t* lock) {
    size_t successor = atomic_load_explicit(&lock->now_serving, memory_order_relaxed) + 1;
    atomic_store_explicit(&lock->now_serving, successor, memory_order_release);
}
