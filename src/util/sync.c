#include <libc/stddef.h>
#include <smp/percpu_storage.h>
#include <interrupts/interrupts.h>
#include <processes/scheduler.h>
#include "sync.h"
#include "arch.h"
#include "debug.h"

#define SPIN_LOCK_RELEASED          (0)
#define SPIN_LOCK_ACQUIRED          (1)

void acquire_lock(lock_t *lock) {
    memory_fence();

    while(!acquire_lock_or_fail(lock)) {
        cpu_pause();
    }
}

bool acquire_lock_or_fail(lock_t *lock) {
    ASSERT(lock != NULL);

    memory_fence();
    uint32_t result = interlocked_compare_exchange(lock, SPIN_LOCK_RELEASED, SPIN_LOCK_ACQUIRED);
    memory_fence();

    return result == SPIN_LOCK_RELEASED;
}

void release_lock(lock_t* lock) {
    memory_fence();
    *lock = SPIN_LOCK_RELEASED;
    memory_fence();
}

uint32_t interlocked_increment(volatile uint32_t* value) {
    uint32_t  result;
    __asm__ __volatile__ (
        "movl    $1, %%eax  \n\t"
        "lock               \n\t"
        "xadd    %%eax, %1  \n\t"
        "inc     %%eax      \n\t"
        : "=&a" (result),
          "+m" (*value)
        :
        : "memory", "cc"
    );
    return result;
}

uint32_t interlocked_decrement(volatile uint32_t* value) {
    uint32_t  result;
    __asm__ __volatile__ (
        "movl    $1, %%eax  \n\t"
        "lock               \n\t"
        "xadd    %%eax, %1  \n\t"
        "dec     %%eax      \n\t"
        : "=&a" (result),
          "+m" (*value)
        :
        : "memory", "cc"
    );
    return result;
}

uint32_t interlocked_compare_exchange(volatile uint32_t* value, uint32_t comapre, uint32_t exchange) {
    __asm__ __volatile__ (
        "lock cmpxchgl %2, %1"
        : "+a" (comapre),
          "+m" (*value)
        : "r"  (exchange)
        : "memory", "cc"
    );
    return comapre;
}

void event_wait(event_t* event) {
    bool enabled = save_and_disable_interrupts();
    acquire_lock(&event->lock);

    thread_t* thread = get_percpu_storage()->running_thread;

    // signal the scheduler to not re-add to the queue
    thread->state = THREAD_STATE_WAITING;

    // add to the queue of threads to start
    acquire_lock(&thread->lock);
    if(event->threads.next == NULL) {
        event->threads = INIT_LIST_ENTRY(event->threads);
    }
    insert_head_list(&event->threads, &thread->scheduler_link);
    release_lock(&thread->lock);

    release_lock(&event->lock);
    set_interrupt_state(enabled);

    // reschedule ourselves
    asm("int %0" : : "i"(IPI_SCHEDULER_RESCHEDULE));
}

void event_signal(event_t* event) {
    bool enabled = save_and_disable_interrupts();
    acquire_lock(&event->lock);

    if(event->threads.next == NULL) {
        event->threads = INIT_LIST_ENTRY(event->threads);
    }

    list_entry_t* link = event->threads.next;
    while(link != &event->threads) {
        thread_t* thread = CR(link, thread_t, scheduler_link);
        link = link->next;

        acquire_lock(&thread->lock);
        thread->state = THREAD_STATE_NORMAL;
        release_lock(&thread->lock);

        // add to scheduler
        scheduler_queue_thread(thread);
    }

    event->threads = INIT_LIST_ENTRY(event->threads);

    release_lock(&event->lock);
    set_interrupt_state(enabled);
}
