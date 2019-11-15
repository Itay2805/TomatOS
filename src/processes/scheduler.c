#include <interrupts/apic/lapic.h>
#include <interrupts/interrupts.h>
#include <smp/percpu_storage.h>
#include <util/list.h>

#include "thread.h"
#include "process.h"
#include "scheduler.h"

static volatile lock_t scheduler_queue_lock = {0};
static list_entry_t scheduler_queue = INIT_LIST_ENTRY(scheduler_queue);

/**
 * Load the context of the thread we want to run
 *
 * it will load everything including the cpu, fpu, segment registers and others
 */
static void load_context(interrupt_context_t* context, thread_t* thread) {
    // copy the registers (This is shit)
    context->rip = thread->cpu_state.rip;
    context->rdi = thread->cpu_state.rdi;
    context->rsi = thread->cpu_state.rsi;
    context->rbp = thread->cpu_state.rbp;
    context->rsp = thread->cpu_state.rsp;
    context->rdx = thread->cpu_state.rdx;
    context->rcx = thread->cpu_state.rcx;
    context->rbx = thread->cpu_state.rbx;
    context->rax = thread->cpu_state.rax;
    context->r8 = thread->cpu_state.r8;
    context->r9 = thread->cpu_state.r9;
    context->r10 = thread->cpu_state.r10;
    context->r11 = thread->cpu_state.r11;
    context->r12 = thread->cpu_state.r12;
    context->r13 = thread->cpu_state.r13;
    context->r14 = thread->cpu_state.r14;
    context->r15 = thread->cpu_state.r15;

    // rflags
    context->rflags = thread->cpu_state.rflags;

    // the segment registers
    if(thread->parent == kernel_process) {
        context->cs = GDT_KERNEL_CODE;
        context->ss = GDT_KERNEL_DATA;
        context->ds = GDT_KERNEL_DATA;
    }else {
        context->cs = GDT_USER_CODE;
        context->ss = GDT_USER_DATA;
        context->ds = GDT_USER_DATA;
    }

    // the gs base
    write_msr(MSR_CODE_IA32_GS_BASE, thread->cpu_state.gs_base);

    // load the vmm context
    vmm_set_handle(&thread->parent->vmm_handle);
}

/**
 * Save the interrupt context into the thread context, used
 * by the scheduler to save the context
 *
 * @param context   [IN]    The context to save from
 * @param thread    [IN]    The thread to save the context to
 */
static void save_context(interrupt_context_t *context, thread_t *thread) {
    thread->cpu_state.rip = context->rip;
    thread->cpu_state.rdi = context->rdi;
    thread->cpu_state.rsi = context->rsi;
    thread->cpu_state.rbp = context->rbp;
    thread->cpu_state.rsp = context->rsp;
    thread->cpu_state.rdx = context->rdx;
    thread->cpu_state.rcx = context->rcx;
    thread->cpu_state.rbx = context->rbx;
    thread->cpu_state.rax = context->rax;
    thread->cpu_state.r8 = context->r8;
    thread->cpu_state.r9 = context->r9;
    thread->cpu_state.r10 = context->r10;
    thread->cpu_state.r11 = context->r11;
    thread->cpu_state.r12 = context->r12;
    thread->cpu_state.r13 = context->r13;
    thread->cpu_state.r14 = context->r14;
    thread->cpu_state.r15 = context->r15;
}

/**
 * This will process a single scheduler tick, the context will be loaded and saved into the context pointer
 *
 * if there is nothing to run it will just run an idle thread
 *
 * @param context   [IN/OUT] The context of the running thread
 */
static error_t scheduler_tick(interrupt_context_t* context, void* user_param) {
    thread_t* thread;

    // lock the scheduler
    acquire_lock(&scheduler_queue_lock);

    thread_t* running_thread = get_percpu_storage()->running_thread;

    // check if there is nothing new to run
    if(scheduler_queue.next == &scheduler_queue && running_thread != NULL && running_thread->state == THREAD_STATE_RUNNING) {
        // if so just cleanup
        goto cleanup;
    }

    // save the context of the running thread
    if(running_thread != NULL) {
        acquire_lock(&running_thread->lock);

        save_context(context, running_thread);

        // set to normal if the thread was running
        if(running_thread->state == THREAD_STATE_RUNNING) {
            running_thread->state = THREAD_STATE_NORMAL;

            // add it back to the scheduling queue
            insert_tail_list(&scheduler_queue, &running_thread->scheduler_link);
        }

        release_lock(&running_thread->lock);
    }

    // choose the next thread
    if(scheduler_queue.next != &scheduler_queue) {
        // dequeue the thread
        thread = CR(scheduler_queue.next, thread_t, scheduler_link);
        remove_entry_list(scheduler_queue.next);

        // load the context of the thread to run
        acquire_lock(&thread->lock);
        load_context(context, thread);
        thread->state = THREAD_STATE_RUNNING;
        release_lock(&thread->lock);

        // set the current thread to running
        get_percpu_storage()->running_thread = thread;
    }else {

        // load the context of the idle thread
        load_context(context, get_percpu_storage()->idle_thread);
        get_percpu_storage()->running_thread = get_percpu_storage()->idle_thread;

    }

cleanup:

    // send back and eoi
    lapic_send_eoi();

    // unlock the register
    release_lock(&scheduler_queue_lock);

    return NO_ERROR;
}

/**
 * Will initialize and startup the scheduler
 *
 * @param ctx   [IN]
 */
static error_t scheduler_startup(interrupt_context_t* ctx, void* user_param) {
    error_t err = NO_ERROR;
    debug_log("[+] \tScheduler started #%d\n", lapic_get_id());

    lapic_set_timer(5, IPI_SCHEDULER_RESCHEDULE);

    // this will handle the sending back of an eoi
    CHECK_AND_RETHROW(scheduler_tick(ctx, user_param));

cleanup:
    return NO_ERROR;
}

/**
 * This is the idle thread loop, it is running while the cpu has nothing else to do
 */
static void idle_thread_loop() {
    while(true) {
        asm("hlt");
    }
}

static interrupt_handler_t reschedule_handler = {
    .vector = IPI_SCHEDULER_RESCHEDULE,
    .callback = scheduler_tick,
    .name = "Scheduler tick/reschedule",
};

static interrupt_handler_t scheduler_startup_handler = {
    .vector = IPI_SCHEDULER_STARTUP,
    .callback = scheduler_startup,
    .name = "Scheduler startup",
};

static bool registered_once = false;

void per_cpu_scheduler_init() {
    // create the idle thread of this thread
    get_percpu_storage()->idle_thread = new(Thread(), kernel_process, idle_thread_loop);

    // register the interrupt handlers
    if(!registered_once) {
        // only need to register once for the fact that we share the
        // idt on all cpus
        interrupt_register(&reschedule_handler);
        interrupt_register(&scheduler_startup_handler);

        registered_once = true;
    }
}

void scheduler_queue_thread(thread_t* thread) {
    ASSERT(thread != NULL);
    acquire_lock(&scheduler_queue_lock);
    insert_tail_list(&scheduler_queue, &thread->scheduler_link);
    release_lock(&scheduler_queue_lock);
}
