#include <arch/cpu.h>
#include "lock.h"

void lock(lock_t* l) {
    size_t ticket = atomic_fetch_add_explicit(&l->next_ticket, 1, memory_order_relaxed);
    while (atomic_load_explicit(&l->now_serving, memory_order_acquire) != ticket) {
        cpu_pause();
    }
}

bool try_lock(lock_t* l) {
    size_t ticket = atomic_load_explicit(&l->now_serving, memory_order_relaxed);
    return atomic_compare_exchange_strong_explicit(&l->now_serving, &ticket, ticket + 1, memory_order_relaxed, memory_order_acquire);
}

void unlock(lock_t* l) {
    size_t successor = atomic_load_explicit(&l->now_serving, memory_order_relaxed) + 1;
    atomic_store_explicit(&l->now_serving, successor, memory_order_release);
}

void irq_lock(lock_t* l) {
    l->ints = are_interrupts_enabled();
    disable_interrupts();
    size_t ticket = atomic_fetch_add_explicit(&l->next_ticket, 1, memory_order_relaxed);
    while (atomic_load_explicit(&l->now_serving, memory_order_acquire) != ticket) {
        if (l->ints) {
            enable_interrupts();
        }
        cpu_pause();
        disable_interrupts();
    }
}

void irq_unlock(lock_t* l) {
    unlock(l);
    if (l->ints) {
        enable_interrupts();
    }
}
