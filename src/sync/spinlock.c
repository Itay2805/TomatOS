#include <stdbool.h>
#include <event/event.h>
#include "spinlock.h"

void spinlock_acquire_high_tpl_raw(spinlock_t* lock, const char* func, const char* filename, int line) {
    lock->owner_tpl = raise_tpl(TPL_HIGH_LEVEL);
    while(!atomic_flag_test_and_set_explicit(&lock->flag, memory_order_acquire)) {
        // restore it if failed and raise it again, this makes so
        // if we hit highest priority level it will enable back
        // interrupts and then lower them again
        restore_tpl(lock->owner_tpl);
        raise_tpl(TPL_HIGH_LEVEL);
    }

    lock->func = func;
    lock->file = filename;
    lock->line = line;
}

void spinlock_acquire_raw(spinlock_t* lock, const char* func, const char* filename, int line) {
    lock->owner_tpl = get_tpl();
    while(!atomic_flag_test_and_set_explicit(&lock->flag, memory_order_acquire));

    lock->func = func;
    lock->file = filename;
    lock->line = line;
}

void spinlock_release(spinlock_t* lock) {
    if (lock->flag._Value) {
        atomic_flag_clear_explicit(&lock->flag, memory_order_release);
        restore_tpl(lock->owner_tpl);
        lock->func = NULL;
        lock->file = NULL;
        lock->line = 0;
    }
}
