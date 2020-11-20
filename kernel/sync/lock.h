#ifndef TOMATOS_LOCK_H
#define TOMATOS_LOCK_H

#include <stdatomic.h>
#include <stdbool.h>

typedef struct lock {
    atomic_size_t now_serving;
    atomic_size_t next_ticket;
    bool ints;
} lock_t;

#define INIT_LOCK() ((lock_t){0})

void lock(lock_t* lock);
bool try_lock(lock_t* lock);
void unlock(lock_t* lock);

void irq_lock(lock_t* lock);
void irq_unlock(lock_t* lock);

#endif //TOMATOS_LOCK_H
