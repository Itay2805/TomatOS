#include <arch/cpu.h>
#include <util/except.h>
#include "lock.h"

void acquire_lock(lock_t* lock) {
    DEBUG_ASSERT(lock != NULL, "Tried to acquire a NULL lock");

    size_t ticket = atomic_fetch_add_explicit(&lock->next_ticket, 1, memory_order_relaxed);
    while (atomic_load_explicit(&lock->now_serving, memory_order_acquire) != ticket) {
        cpu_pause();
    }

    lock->owner_tpl = raise_tpl(lock->tpl);
}

bool acquire_lock_or_fail(lock_t* lock) {
    DEBUG_ASSERT(lock != NULL, "Tried to acquire a NULL lock");

    size_t ticket = atomic_load_explicit(&lock->now_serving, memory_order_relaxed);
    bool result = atomic_compare_exchange_strong_explicit(&lock->now_serving, &ticket, ticket + 1, memory_order_relaxed, memory_order_acquire);

    if (result) {
        lock->owner_tpl = raise_tpl(lock->tpl);
    }

    return result;
}

void release_lock(lock_t* lock) {
    DEBUG_ASSERT(lock != NULL, "Tried to release a NULL lock");


    tpl_t tpl = lock->owner_tpl;

    size_t successor = atomic_load_explicit(&lock->now_serving, memory_order_relaxed) + 1;
    atomic_store_explicit(&lock->now_serving, successor, memory_order_release);

    restore_tpl(tpl);
}
