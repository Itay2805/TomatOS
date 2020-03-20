#include <mm/stack_allocator.h>
#include <intr/apic/lapic.h>
#include <arch/simd.h>
#include "sched.h"

_Atomic(thread_t*) CPU_LOCAL m_current_thread;

thread_t* get_current_thread() {
    return m_current_thread;
}

process_t* get_current_process() {
    return m_current_thread->parent;
}

_Atomic(uintptr_t) CPU_LOCAL g_kernel_stack;
_Atomic(uintptr_t) CPU_LOCAL g_saved_stack;

static list_entry_t m_threads_queue = INIT_LIST_ENTRY(m_threads_queue);

// we need a custom lock that does not touch tpl
static atomic_flag m_flag;

static void sched_lock() {
    while (!atomic_flag_test_and_set_explicit(&m_flag, memory_order_acquire));
}

static void sched_unlock() {
    atomic_flag_clear_explicit(&m_flag, memory_order_release);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Preemption
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * This is called once every 5ms, it will yield out of the current thread
 * to another one
 *
 * This runs at TPL_PREEMPTION
 */
static err_t preemption_callback(void* ctx, event_t event) {
    yield();
    return NO_ERROR;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Handle yielding
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static err_t yield_interrupt(void* user_param, interrupt_context_t* ctx) {
    err_t err = NO_ERROR;

    CHECK_AND_RETHROW(sched_tick(ctx));

cleanup:
    return err;
}

static interrupt_handler_t handler = {
    .name = "Yield Interrupt",
    .vector = IPI_YIELD,
    .callback = yield_interrupt
};

void yield() {
    // can only yield under preemption tpl
    ASSERT(get_tpl() <= TPL_PREEMPTION);

    // pretty much a hack tbh
    static_assert(IPI_YIELD == 0x20);
    asm volatile("int $0x20" ::: "memory");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// idle thread
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static thread_t* CPU_LOCAL idle_thread;

// will sleep waiting for the next interrupt
static void idle_thread_func() {
    while (1) {
        /*
         * Wait for interrupt and once we got one simply
         * set the thread as waiting and yield, that will
         * make sure we don't ever schedule the idle thread
         */
        asm volatile("hlt" ::: "memory");
        m_current_thread->state = STATE_WAITING;
        yield();
    }
}

err_t init_sched() {
    err_t err = NO_ERROR;

    // for the first time register the interrupt
    if (handler.link.next == NULL) {
        interrupt_register(&handler);
    }

    // spawn an idle thread for cpu
    // need this to be a high tpl task
    thread_t* thread = NULL;
    CHECK_AND_RETHROW(spawn_thread(&kernel_process, (uintptr_t)idle_thread_func, allocate_stack(), &thread));
    idle_thread = thread;

    // setup the timer
    event_t event;
    CHECK_AND_RETHROW(create_event(TPL_PREEMPTION, preemption_callback, NULL, &event));
    CHECK_AND_RETHROW(set_timer(event, TIMER_PERIODIC, MS_TO_100NS(15)));

cleanup:
    return err;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// scheduling itself
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

err_t queue_thread(thread_t* thread) {
    err_t err = NO_ERROR;

    sched_lock();

    CHECK(thread != NULL);

    // only add if waiting
    // TODO: use compare exchange
    if (thread->state == STATE_WAITING) {
        thread->state = STATE_NORMAL;
        insert_tail_list(&m_threads_queue, &thread->scheduler_link);
    }

cleanup:
    sched_unlock();
    return err;
}

err_t sched_tick(interrupt_context_t* ctx) {
    err_t err = NO_ERROR;

    sched_lock();

    CHECK(ctx != NULL);

    // check if has something to run
    if (is_list_empty(&m_threads_queue)) {
        // make sure something runs
        ASSERT(m_current_thread != NULL);

        // if not running then it needs to be unscheduled
        // run the idle task instead
        if (m_current_thread->state != STATE_RUNNING) {
            // save the current context
            m_current_thread->cpu_context = *ctx;

            // schedule the idle thread
            m_current_thread = idle_thread;
            *ctx = idle_thread->cpu_context;
            vmm_set_handle(&kernel_process.vmm_handle);

            // no need for the temp stack or kernel stack
        }
    } else {
        thread_t* running = m_current_thread;

        // pop from the queue
        thread_t* to_run = CR(m_threads_queue.next, thread_t, scheduler_link);
        remove_entry_list(&to_run->scheduler_link);
        to_run->scheduler_link.next = NULL;
        ASSERT(to_run->state == STATE_NORMAL);

        // if we have something running save it's context
        if (running != NULL) {
            running->apic_id = 0;
            running->saved_stack = g_saved_stack;
            running->cpu_context = *ctx;
            save_simd_state(running->simd_state);

            if (running->state == STATE_RUNNING) {
                running->state = STATE_NORMAL;
                insert_tail_list(&m_threads_queue, &running->scheduler_link);
            } else if(running->state == STATE_DEAD) {
                TRACE("Thread %d has died, TODO kill it", running->tid);
            } else {
                // TODO: delete the thread if needed
            }
        }

        // load the new context
        *ctx = to_run->cpu_context;
        g_saved_stack = to_run->saved_stack;
        g_kernel_stack = to_run->kernel_stack;
        restore_simd_state(to_run->simd_state);

        // if not the same parent then assume different address space and switch handle
        vmm_set_handle(&to_run->parent->vmm_handle);

        // set as the running thread
        to_run->state = STATE_RUNNING;
        to_run->apic_id = get_apic_id();
        m_current_thread = to_run;
    }

cleanup:
    sched_unlock();
    return err;
}
