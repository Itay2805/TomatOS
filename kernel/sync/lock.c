#include <arch/cpu.h>
#include "lock.h"

void ticket_lock(ticket_lock_t* lock) {
    size_t ticket = atomic_fetch_add_explicit(&lock->next_ticket, 1, memory_order_relaxed);
    while (atomic_load_explicit(&lock->now_serving, memory_order_acquire) != ticket) {
        cpu_pause();
    }
}

bool ticket_try_lock(ticket_lock_t* lock) {
    size_t ticket = atomic_load_explicit(&lock->now_serving, memory_order_relaxed);
    return atomic_compare_exchange_strong_explicit(&lock->now_serving, &ticket, ticket + 1, memory_order_relaxed, memory_order_acquire);
}

void ticket_unlock(ticket_lock_t* lock) {
    size_t successor = atomic_load_explicit(&lock->now_serving, memory_order_relaxed) + 1;
    atomic_store_explicit(&lock->now_serving, successor, memory_order_release);
}
