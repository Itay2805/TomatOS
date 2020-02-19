#ifndef __SYNC_SPINLOCK_H__
#define __SYNC_SPINLOCK_H__

#include <stdatomic.h>
#include <event/event.h>

#define SPINLOCK_INIT (spinlock_t){ ATOMIC_FLAG_INIT, 0 }

typedef struct {
    atomic_flag flag;
    tpl_t owner_tpl;
} spinlock_t;

/**
 * Will acquire the lock
 */
void spinlock_acquire(spinlock_t* lock);

/**
 * Will acquire the lock, first raising to the given tpl level
 *
 * NOTE: This must raise to higher tpl level than the current one or the raise_tpl
 *       function will assert!
 *
 * @param lock  [IN] The lock
 * @param tpl   [IN] The tpl to raise to
 */
void spinlock_acquire_high_tpl(spinlock_t* lock);

/**
 * Release the spinlock, restoring the original tpl
 */
void spinlock_release(spinlock_t* lock);

#endif //__SYNC_SPINLOCK_H__
