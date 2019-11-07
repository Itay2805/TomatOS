#include <util/list.h>
#include <smp/percpu_storage.h>
#include <interrupts/interrupts.h>
#include "scheduler.h"
#include "thread.h"

static lock_t scheduler_queue_lock = {0};
static list_entry_t scheduler_queue = { &scheduler_queue, &scheduler_queue };

/**
 * Load the context of the thread we want to run
 *
 * it will load everything including the cpu, fpu, segment registers and others
 */
static void load_context(interrupt_context_t* context, thread_t* thread) {
    // copy the registers (This is shit)
    context->rip = thread->state.cpu.rip;
    context->rdi = thread->state.cpu.rdi;
    context->rsi = thread->state.cpu.rsi;
    context->rbp = thread->state.cpu.rbp;
    context->rsp = thread->state.cpu.rsp;
    context->rdx = thread->state.cpu.rdx;
    context->rcx = thread->state.cpu.rcx;
    context->rbx = thread->state.cpu.rbx;
    context->rax = thread->state.cpu.rax;
    context->r8 = thread->state.cpu.r8;
    context->r9 = thread->state.cpu.r9;
    context->r10 = thread->state.cpu.r10;
    context->r11 = thread->state.cpu.r11;
    context->r12 = thread->state.cpu.r12;
    context->r13 = thread->state.cpu.r13;
    context->r14 = thread->state.cpu.r14;
    context->r15 = thread->state.cpu.r15;

    // rflags
    context->rflags = thread->state.cpu.rflags;

    // the segment registers
    if(thread->parent == &kernel_process) {
        context->cs = GDT_KERNEL_CODE;
        context->ss = GDT_KERNEL_DATA;
        context->ds = GDT_KERNEL_DATA;
    }else {
        context->cs = GDT_USER_CODE;
        context->ss = GDT_USER_DATA;
        context->ds = GDT_USER_DATA;
    }

    // the gs base
    write_msr(MSR_CODE_IA32_GS_BASE, thread->state.cpu.gs_base);

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
    thread->state.cpu.rip = context->rip;
    thread->state.cpu.rdi = context->rdi;
    thread->state.cpu.rsi = context->rsi;
    thread->state.cpu.rbp = context->rbp;
    thread->state.cpu.rsp = context->rsp;
    thread->state.cpu.rdx = context->rdx;
    thread->state.cpu.rcx = context->rcx;
    thread->state.cpu.rbx = context->rbx;
    thread->state.cpu.rax = context->rax;
    thread->state.cpu.r8 = context->r8;
    thread->state.cpu.r9 = context->r9;
    thread->state.cpu.r10 = context->r10;
    thread->state.cpu.r11 = context->r11;
    thread->state.cpu.r12 = context->r12;
    thread->state.cpu.r13 = context->r13;
    thread->state.cpu.r14 = context->r14;
    thread->state.cpu.r15 = context->r15;
}

/**
 * This will process a single scheduler tick, the context will be loaded and saved into the context pointer
 *
 * if there is nothing to run it will just run an idle thread
 *
 * @param context   [IN/OUT] The context of the running thread
 */
static void scheduler_tick(interrupt_context_t* context) {
    thread_t* thread;

    // save the context of the running thread
    if(get_percpu_storage()->running_thread != NULL) {
        aquire_lock(&get_percpu_storage()->running_thread->lock);
        save_context(context, get_percpu_storage()->running_thread);
        release_lock(&get_percpu_storage()->running_thread->lock);
    }

    // choose the next thread
    if(scheduler_queue.next != &scheduler_queue) {

        // get the thread to schedule
        aquire_lock(&scheduler_queue_lock);
        thread = CR(scheduler_queue.next, thread_t, scheduler_link);
        remove_entry_list(scheduler_queue.next);
        // TODO: put the running thread back into the queue
        release_lock(&scheduler_queue_lock);

        // load the context of the thread to run
        aquire_lock(&thread->lock);
        load_context(context, thread);
        release_lock(&thread->lock);

        // set the current thread to running
        get_percpu_storage()->running_thread = thread;
    }else {

        // load the context of the idle thread
        load_context(context, &get_percpu_storage()->idle_thread);

    }
}

/**
 * This is the idle thread loop, it is running while the cpu has nothing else to do
 */
static void idle_thread_loop() {
    while(true) {
        asm("hlt");
    }
}

void per_cpu_scheduler_init() {
    // create the idle thread of this thread
    thread_init(&get_percpu_storage()->idle_thread, &kernel_process);
    get_percpu_storage()->idle_thread.state.cpu.rip = (uint64_t)&idle_thread_loop;
}
