#include <buf.h>
#include <memory/vmm.h>
#include "signal.h"
#include "thread.h"

error_t signal_route(signal_handler_t* handler) {
    error_t err = NO_ERROR;

    CHECK_ERROR(handler, ERROR_INVALID_ARGUMENT);
    CHECK_ERROR(handler->thread, ERROR_INVALID_ARGUMENT);

    // TODO: Make sure the start address is in user space

    thread_t* thread = handler->thread;

    switch(handler->type) {

        case SIGNAL_NONE: {
            // no routing
        } break;

        case SIGNAL_CALLBACK: {
            // callback handler
            // TODO: What fpu state?

            // save cpu state as needed
            // red zone
            uint64_t original_rsp = thread->state.cpu.rsp;
            thread->state.cpu.rsp -= 128;

            uint64_t* stack = PHYSICAL_ADDRESS(thread->state.cpu.rsp);
            // TODO: Check that the stack is alright

            // we want to eventually return to where we were
            stack[0] = thread->state.cpu.rip; thread->state.cpu.rsp -= 8;

            // save the needed registers to the stack
            stack[-1] = original_rsp; thread->state.cpu.rsp -= 8;
            stack[-2] = thread->state.cpu.rax; thread->state.cpu.rsp -= 8;
            stack[-3] = thread->state.cpu.rdi; thread->state.cpu.rsp -= 8;
            stack[-4] = thread->state.cpu.rsi; thread->state.cpu.rsp -= 8;
            stack[-5] = thread->state.cpu.rdx; thread->state.cpu.rsp -= 8;
            stack[-6] = thread->state.cpu.rcx; thread->state.cpu.rsp -= 8;
            stack[-7] = thread->state.cpu.r8; thread->state.cpu.rsp -= 8;
            stack[-8] = thread->state.cpu.r9; thread->state.cpu.rsp -= 8;
            stack[-9] = thread->state.cpu.r10; thread->state.cpu.rsp -= 8;
            stack[-10] = thread->state.cpu.r11; thread->state.cpu.rsp -= 8;

            // we want to return to the restoration stub
            stack[-11] = (uint64_t) signal_return; thread->state.cpu.rsp -= 8;

            // set the rip to the handler
            thread->state.cpu.rip = (uint64_t) handler->start;
        } break;

        case SIGNAL_THREAD: {
            // just create the thread
            thread_create(thread->parent, handler->start, (void*)handler->value, NULL);
        } break;

        default: {
            CHECK_FAIL_ERROR_TRACE(ERROR_INVALID_ARGUMENT, "Attempted to route a signal of unknown type");
        } break;
    }

cleanup:
    return err;
}
