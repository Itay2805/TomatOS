#ifndef TOMATKERNEL_SPILOCK_H
#define TOMATKERNEL_SPILOCK_H

#include <stdint.h>
#include <stdbool.h>

typedef struct spinlock {
    volatile uint32_t locked;
    uint64_t interrupts;

    // where it was locked/unlocked
    char* filename;
    int line;

    // TODO: Maybe dead lock detection?
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
void do_lock(spinlock_t* spinlock, char* file, int line);

/**
 * Will unlock the given spinlock
 *
 * @remark
 * Not safe in either irq/shared context! use
 * unlock_sti for such contexts instead!
 *
 * @param spinlock  [IN] Spinlock to unlock
 */
void do_unlock(spinlock_t* spinlock, char* file, int line);

/**
 * Will attempt to lock (will not block)
 *
 * @param spinlock  [IN] Spinlock to attempt to lock
 */
bool do_try_lock(spinlock_t* spinlock, char* file, int line);

/**
 * Interrupt clearing spinlock
 *
 * @param spinlock1 [IN] Spinlock to lock
 *
 * @see
 * lock
 */
void do_lock_preemption(spinlock_t *spinlock, char* file, int line);

/**
 * unlock and enable interrupts (if needed)
 *
 * @param spinlock  [IN] Spinlock to unlock
 *
 * @see
 * unlock
 */
void do_unlock_preemption(spinlock_t *spinlock, char* file, int line);

#define lock(spinlock) lock(spinlock, __FILENAME__, __LINE__)
#define unlock(spinlock) unlock(spinlock, __FILENAME__, __LINE__)
#define try_lock(spinlock) do_try_lock(spinlock, __FILENAME__, __LINE__)
#define lock_preemption(spinlock) do_lock_preemption(spinlock, __FILENAME__, __LINE__)
#define unlock_preemption(spinlock) do_unlock_preemption(spinlock, __FILENAME__, __LINE__)

#endif //TOMATKERNEL_SPILOCK_H
