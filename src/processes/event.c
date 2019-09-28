#include "event.h"

#include <stdlib.h>
#include <stb/stb_ds.h>
#include <smp/percpustorage.h>
#include <interrupts/apic/lapic.h>

#include "scheduler.h"

error_t event_wait(event_t* event) {
    error_t err = NO_ERROR;

    lock_preemption(&running_threads_lock);

    // add to waiting list
    lock_preemption(&event->lock);
    arrpush(event->waiting_threads, running_threads[get_cpu_index()]);
    unlock_preemption(&event->lock);

    // set as waiting, will cause the scheduler to not add this thread to the queue
    running_threads[get_cpu_index()]->status = THREAD_STATUS_WAITING;

    unlock_preemption(&running_threads_lock);

    // reschedule
    asm("int %0" : : "i"(INTERRUPT_SCHEDULER_RESCHEDULE));
//    CHECK_AND_RETHROW(lapic_send_ipi(lapic_get_id(), INTERRUPT_SCHEDULER_RESCHEDULE));

cleanup:
    return err;
}

error_t event_signal(event_t* event) {
    error_t err = NO_ERROR;

    lock_preemption(&event->lock);

    // add each of the threads to the scheduler
    for(int i = 0; i < arrlen(event->waiting_threads); i++) {
        CATCH(scheduler_queue_thread(event->waiting_threads[i]));
    }

    // free the thread, we don't need it for now
    arrfree(event->waiting_threads);

//cleanup:
    unlock_preemption(&event->lock);
    return err;
}