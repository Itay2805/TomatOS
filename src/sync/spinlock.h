#ifndef __SYNC_SPINLOCK_H__
#define __SYNC_SPINLOCK_H__

#include <stdatomic.h>
#include <event/event.h>

#define SPINLOCK_INIT (spinlock_t){ ATOMIC_FLAG_INIT, 0 }

typedef struct {
    atomic_flag flag;
    tpl_t owner_tpl;

    const char* func;
    const char* file;
    int line;
} spinlock_t;

/**
 * Will acquire the lock
 */
void spinlock_acquire_raw(spinlock_t* lock, const char* func, const char* filename, int line);

/**
 * Will acquire the lock, first raising to the given tpl level
 *
 * NOTE: This must raise to higher tpl level than the current one or the raise_tpl
 *       function will assert!
 *
 * @param lock  [IN] The lock
 * @param tpl   [IN] The tpl to raise to
 */
void spinlock_acquire_high_tpl_raw(spinlock_t* lock, const char* func, const char* filename, int line);

/**
 * Release the spinlock, restoring the original tpl
 */
void spinlock_release(spinlock_t* lock);

#define spinlock_acquire(lock) spinlock_acquire_raw(lock, __func__, __FILENAME__, __LINE__)
#define spinlock_acquire_high_tpl(lock) spinlock_acquire_high_tpl_raw(lock, __func__, __FILENAME__, __LINE__)

#endif //__SYNC_SPINLOCK_H__
