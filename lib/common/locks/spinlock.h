#ifndef TOMATKERNEL_SPILOCK_H
#define TOMATKERNEL_SPILOCK_H

#include <stdint.h>
#include <stdbool.h>

typedef struct spinlock {
    uint32_t locked;
    uint64_t interrupts;
} spinlock_t;

/**
 * Will block until can lock
 *
 * @remark
 * Not safe in either irq/shared context! use
 * lock_cli for such contexts instead!
 *
 * @param spinlock  [IN] Spinlock to lock
 */
void lock(spinlock_t* spinlock);

/**
 * Will unlock the given spinlock
 *
 * @remark
 * Not safe in either irq/shared context! use
 * unlock_sti for such contexts instead!
 *
 * @param spinlock  [IN] Spinlock to unlock
 */
void unlock(spinlock_t* spinlock);

/**
 * Will attempt to lock (will not block)
 *
 * @param spinlock  [IN] Spinlock to attempt to lock
 */
bool try_lock(spinlock_t* spinlock);

/**
 * Interrupt clearing spinlock
 *
 * @param spinlock1 [IN] Spinlock to lock
 *
 * @see
 * lock
 */
void lock_preemption(spinlock_t *spinlock1);

/**
 * unlock and enable interrupts (if needed)
 *
 * @param spinlock1 [IN] Spinlock to unlock
 *
 * @see
 * unlock
 */
void unlock_preemption(spinlock_t *spinlock);

#endif //TOMATKERNEL_SPILOCK_H
