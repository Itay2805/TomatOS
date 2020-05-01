#include <util/trace.h>
#include <util/except.h>

#include <stdbool.h>
#include <util/list.h>
#include <arch/msr.h>
#include <intr/apic/lapic.h>
#include <util/symlist.h>
#include <string.h>
#include <mm/vmm.h>
#include <proc/sched.h>
#include <util/def.h>
#include <mm/gdt.h>

#include "intr.h"

static list_entry_t g_interrupt_handlers[256];

//////////////////////////////////////////////////////////////////
// Default Exception handling
//////////////////////////////////////////////////////////////////

static const char* ISR_NAMES[] = {
        "#DE - Divide Error",
        "#DB - Debug",
        "NMI Interrupt",
        "#BP - Breakpoint",
        "#OF - Overflow",
        "#BR - BOUND Range Exceeded",
        "#UD - Invalid Opcode",
        "#NM - Device Not Available",
        "#DF - Double Fault",
        "Coprocessor Segment Overrun",
        "#TS - Invalid TSS",
        "#NP - Segment Not Present",
        "#SS - Stack Fault Fault",
        "#GP - General Protection",
        "#PF - Page-Fault",
        "Reserved",
        "#MF - x87 FPU Floating-Point Error",
        "#AC - Alignment Check",
        "#MC - Machine-Check",
        "#XM - SIMD floating-point",
        "#VE - Virtualization",
        "#CP - Control Protection",
};

static char* PROCESSOR_NAME[] = {
        "Internal",
        "External"
};

static char* TABLE_NAME[] = {
        "GDT",
        "IDT",
        "LDT",
        "IDT"
};

static char* PRESENT_NAMES[] = {
        "Non-Present page",
        "Page-Protection violation"
};

static char* OPERATION_NAME[] = {
        "Read",
        "Write"
};

static char* USER_NAME[] = {
        "Supervisor",
        "User",
};

static void on_kernel_exception(interrupt_context_t *regs, tpl_t was_tpl) {
    // print error name
    const char* name = 0;
    if (regs->int_num <= sizeof(ISR_NAMES) / sizeof(char*)) {
        name = ISR_NAMES[regs->int_num];
    }

    if (name == 0) {
        trace("[-] Unhandled interrupt: %lx\n", regs->int_num);
    }else {
        if (regs->int_num == EXCEPTION_BREAKPOINT && __readcr2() == PANIC_CR2_MAGIC) {
            trace("[-] Got a kernel panic\n");
        } else {
            trace("[-] Exception occurred: %s\n", name);
        }
    }

    // print error if any
    switch (regs->int_num) {
        case 10: // Invalid TSS
        case 11: // Segment Not Presented
        case 12: // Stack segment fault
        case 13: // General Protection Fault
            if(regs->error_code != 0) {
                trace("[-] Selector(processor=%s, table=%s, index=%d)\n",
                          PROCESSOR_NAME[regs->error_code & 0b1u],
                          TABLE_NAME[(regs->error_code >> 1u) & 0b11u],
                          (int) (regs->error_code & 0xFFF8u));
            }
            break;

        case 14: // page fault
            trace("[-] Reason: %s\n", PRESENT_NAMES[regs->error_code & 1u]);
            trace("[-] Address: 0x%p\n", (void *)__readcr2());
            trace("[-] Mode: %s\n", USER_NAME[(regs->error_code >> 2u) & 1u]);
            if(((regs->error_code >> 4u) & 1u) != 0) {
                trace("[-] Operation: Instruction Fetch\n");
            }else if(((regs->error_code >> 3u) & 1u) != 0) {
                trace("[-] Operation: Reserved write\n");
            }else {
                trace("[-] Operation: %s\n", OPERATION_NAME[(regs->error_code >> 1u) & 1u]);
            }
            break;

        default:
            break;
    }

    // thread info
    if (get_current_thread() != NULL) {
        trace("[-] Process: pid=%d tid=%d\n", get_current_process()->pid, get_current_thread()->tid);
    }

    // print the tpl
    switch (was_tpl) {
        // for defined values
        case TPL_HIGH_LEVEL: trace("[-] Current TPL: TPL_HIGH_LEVEL\n"); break;
        case TPL_APPLICATION: trace("[-] Current TPL: TPL_APPLICATION\n"); break;
        case TPL_PREEMPTION: trace("[-] Current TPL: TPL_PREEMPTION\n"); break;
        case TPL_CALLBACK: trace("[-] Current TPL: TPL_CALLBACK\n"); break;
        case TPL_IO_CALLBACK: trace("[-] Current TPL: TPL_IO_CALLBACK\n"); break;
        case TPL_TIMER: trace("[-] Current TPL: TPL_TIMER\n"); break;
        case TPL_USER_HIGH: trace("[-] Current TPL: TPL_USER_HIGH\n"); break;

        default:
            trace("[-] Current TPL: %d\n", was_tpl);
    }

    // print registers
    char _cf = (char) ((regs->rflags.CF) != 0 ? 'C' : '-');
    char _pf = (char) ((regs->rflags.PF) != 0 ? 'P' : '-');
    char _af = (char) ((regs->rflags.AF) != 0 ? 'A' : '-');
    char _zf = (char) ((regs->rflags.ZF) != 0 ? 'Z' : '-');
    char _sf = (char) ((regs->rflags.SF) != 0 ? 'S' : '-');
    char _tf = (char) ((regs->rflags.TF) != 0 ? 'T' : '-');
    char _if = (char) ((regs->rflags.IF) != 0 ? 'I' : '-');

    int cpl = (int) (regs->rflags.IOPL);

    size_t off = 0;
    name = symlist_name_from_address(&off, regs->rip);

    trace("[-] RAX=%016lx RBX=%016lx RCX=%016lx RDX=%016lx\n", regs->rax, regs->rbx, regs->rcx, regs->rdx);
    trace("[-] RSI=%016lx RDI=%016lx RBP=%016lx RSP=%016lx\n", regs->rsi, regs->rdi, regs->rbp, regs->rsp);
    trace("[-] R8 =%016lx R9 =%016lx R10=%016lx R11=%016lx\n", regs->r8, regs->r9, regs->r10, regs->r10);
    trace("[-] R12=%016lx R13=%016lx R14=%016lx R15=%016lx\n", regs->r12, regs->r13, regs->r14, regs->r15);
    if (off == 0 || name == 0) {
        trace("[-] RIP=%016lx RFL=%08x [%c%c%c%c%c%c%c] CPL=%d\n", regs->rip, (uint32_t)regs->rflags.raw, _if, _tf, _sf, _zf, _af, _pf, _cf, cpl);
    } else {
        trace("[-] RIP=%016lx <%s+%lx> RFL=%08x [%c%c%c%c%c%c%c] CPL=%d\n", regs->rip, name, off, (uint32_t)regs->rflags.raw, _if, _tf, _sf, _zf, _af, _pf, _cf, cpl);
    }
    trace("[-] CS =%04lx                  DPL=%ld\n", regs->cs & 0xFFF8, regs->cs & 0b11);
    trace("[-] DS =%04lx                  DPL=%ld\n", regs->ds & 0xFFF8, regs->ds & 0b11);
    trace("[-] SS =%04lx                  DPL=%ld\n", regs->ss & 0xFFF8, regs->ss & 0b11);
    // TODO: actually read the gs and fs registers
    trace("[-] GS =%04lx %016lx DPL=%ld \n", GDT_USER_DATA & 0xFFF8, __readmsr(MSR_IA32_GS_BASE), GDT_USER_DATA & 0b11);
    trace("[-] FS =%04lx %016lx DPL=%ld \n", GDT_USER_DATA & 0xFFF8, __readmsr(MSR_IA32_FS_BASE), GDT_USER_DATA & 0b11);
    trace("[-] CR0=%08lx CR2=%016lx CR3=%016lx CR4=%08lx\n", __readcr0().raw, __readcr2(), __readcr3(), __readcr4().raw);

    uintptr_t* base_ptr = (uintptr_t*)regs->rbp;
    trace("[-] Stack trace:\n");
    int pfdepth = 0;
    for (;;) {
        // check if the page is mapped
        if (!vmm_is_mapped(vmm_get_handle(), (uintptr_t)ALIGN_DOWN(base_ptr, 4096), sizeof(uintptr_t) * 2)) {
            break;
        }

        // if it is then we can trace it
        uintptr_t old_bp = base_ptr[0];
        uintptr_t ret_addr = base_ptr[1];

        if (ret_addr == 0) {
            break;
        }

        name = symlist_name_from_address(&off, ret_addr);
        if (name != NULL) {
            trace("[-] \t[0x%016lx] <%s+%lx>\n", ret_addr, name, off);
        }

        if (old_bp == 0) {
            break;
        }

        if (name != NULL && strcmp(name, "common_stub") == 0) {
            interrupt_context_t* old_ctx = (interrupt_context_t*)((uintptr_t)&base_ptr[2]);

            // trace the interrupt frame
            if (old_ctx->int_num <= sizeof(ISR_NAMES) / sizeof(char*)) {
                name = ISR_NAMES[old_ctx->int_num];
            } else {
                if (!is_list_empty(&g_interrupt_handlers[old_ctx->int_num])) {
                    name = CR(g_interrupt_handlers[old_ctx->int_num].next, interrupt_handler_t, link)->name;
                }
            }
            if (name != NULL) {
                trace("[-] \t{%s}\n", name);
            } else {
                trace("[-] \t{Interrupt #%lx}\n", old_ctx->int_num);
            }

            if (old_ctx->int_num == EXCEPTION_PAGE_FAULT) {
                pfdepth++;
                if (pfdepth >= 2) {
                    trace("[-] \tRecursive page fault detected, stopping stack trace\n");
                    break;
                }
            }

            // trace the address before
            name = symlist_name_from_address(&off, old_ctx->rip);
            trace("[-] \t[0x%016lx] <%s+%lx>\n", old_ctx->rip, name, off);

            // continue with the correct base pointer
            base_ptr = (uintptr_t*)old_ctx->rbp;
        } else {
            base_ptr = (void*)old_bp;
        }
    }

    trace("[-] \n");
    trace("[-] :(\n");

    // TODO: serial debug console?

    while(1) asm("hlt");
}

static err_t default_interrupt_handler(interrupt_context_t* ctx, tpl_t tpl) {
    err_t err = NO_ERROR;
    thread_t* thread = get_current_thread();

    // if this is in a non-kernel thread check if it happened
    // from userspace or from syscall
    if (thread != NULL && thread->parent != &kernel_process) {
        bool from_usermode = thread != NULL && ctx->cs == GDT_USER_CODE;
        bool from_syscall = !from_usermode && thread != NULL && thread->syscall_ctx != NULL;

        switch (ctx->int_num) {
            //
            // TODO: Handle these properly once we have stuff like debugger
            //
            case EXCEPTION_DEBUG:
            case EXCEPTION_BREAKPOINT:
                if (from_usermode) {
                    on_user_exception(ctx);
                    goto cleanup;
                }
                break;

            //
            // for page faults we can either have a usermode or a syscall fault
            // TODO: maybe assume that a kernel pointer is a problem in kernel
            //       and panic?
            //
            case EXCEPTION_PAGE_FAULT: {
                if (from_usermode) {
                    on_user_exception(ctx);
                    goto cleanup;
                }
                if (from_syscall) {
                    on_syscall_exception(ctx);
                    goto cleanup;
                }
            } break;


            //
            // these exceptions can be raised by userspace normally
            //
            case EXCEPTION_DIVIDE_ERROR:
            case EXCEPTION_OVERFLOW:
            case EXCEPTION_BOUND:
            case EXCEPTION_INVALID_OPCODE:
            case EXCEPTION_STACK_FAULT:
            case EXCEPTION_GP_FAULT:
            case EXCEPTION_FP_ERROR:
            case EXCEPTION_ALIGNMENT_CHECK:
            case EXCEPTION_SIMD:
                if (from_usermode) {
                    on_user_exception(ctx);
                    goto cleanup;
                }
        }
    }

    // if we reached here then do a kernel exception
    on_kernel_exception(ctx, tpl);

cleanup:
    return err;
}

//////////////////////////////////////////////////////////////////
// Interrupt handling
//////////////////////////////////////////////////////////////////

#define INTERRUPT_VECTOR_SIZE (0xff - INT_ALLOCATION_BASE - 1)
static int g_interrupt_vectors[INTERRUPT_VECTOR_SIZE];
static int g_last_vec_index = 0;

void interrupts_init() {
    // just initialize all the list entries
    for(int i = 0; i < ARRAY_LEN(g_interrupt_handlers); i++) {
        g_interrupt_handlers[i] = INIT_LIST_ENTRY(g_interrupt_handlers[i]);
    }
}

void common_interrupt_handler(interrupt_context_t ctx) {
    err_t err = NO_ERROR;

    // interrupt handlers always run at a high level
    // raise the tpl and save the old one in the thread so
    // we can restore it later
    thread_t* thread = get_current_thread();
    tpl_t oldtpl = raise_tpl(TPL_HIGH_LEVEL);

    if (LIKELY(thread != NULL)) {
        thread->thread_tpl = oldtpl;
    }

    if(is_list_empty(&g_interrupt_handlers[ctx.int_num])) {

        // just call the default handler if none is found
        default_interrupt_handler(&ctx, oldtpl);

    }else {
        // iterate and call all the handlers
        list_entry_t* handlers = &g_interrupt_handlers[ctx.int_num];
        for(list_entry_t* link = handlers->next; link != handlers; link = link->next) {
            interrupt_handler_t* handler = CR(link, interrupt_handler_t, link);
            CHECK_AND_RETHROW(handler->callback(handler->user_param, &ctx));
        }
    }

cleanup:
    WARN(!IS_ERROR(err), "Got an error inside interrupt handler/callback");

    // if the thread is dead then just yield
    // make sure there is actually a thread
    // running (when we startup threading we
    // won't have this initialized)
    if (LIKELY(get_current_thread() != NULL) && get_current_thread()->state == STATE_DEAD) {
        yield();
    }

    // get the correct tpl to restore
    thread = get_current_thread();
    if (LIKELY(thread != NULL)) {
        oldtpl = thread->thread_tpl;
    }

    // restore the tpl back to normal and return
    restore_tpl(oldtpl);

}

//////////////////////////////////////////////////////////////////
// Interrupt manipulation
//////////////////////////////////////////////////////////////////

static uint8_t interrupt_allocate() {
    int vec = g_last_vec_index;
    for(int i = g_last_vec_index; i < INTERRUPT_VECTOR_SIZE; i++) {
        if(g_interrupt_vectors[i] < g_interrupt_vectors[vec]) {
            vec = i;

            // if has zero uses we can just return it
            if(g_interrupt_vectors[vec] == 0) goto found;
        }
    }

    // we could not find a completely free one
    // try to search from the start for one
    if(g_last_vec_index != 0) {
        for(int i = 0; i < g_last_vec_index; i++) {
            if(g_interrupt_vectors[i] < g_interrupt_vectors[vec]) {
                vec = i;

                // if has zero uses we can just return it
                if(g_interrupt_vectors[vec] == 0) goto found;
            }
        }
    }

    // if we could still not find a good one, just use whatever
    // we got

found:
    // increment the usage count, set the
    // new index to start from, and return
    // the correct number
    g_interrupt_vectors[vec]++;
    g_last_vec_index = (vec + 1) % INTERRUPT_VECTOR_SIZE;
    return (uint8_t) (vec + INT_ALLOCATION_BASE);
}

void interrupt_register(interrupt_handler_t* inthandler) {
    ASSERT(inthandler != NULL);
    ASSERT(inthandler->callback != NULL);

    // allocate an interrupt number if needed
    if(inthandler->vector == -1) {
        inthandler->vector = interrupt_allocate();
    }

    ASSERT(0 <= inthandler->vector && inthandler->vector <= 255);

    // insert it
    insert_tail_list(&g_interrupt_handlers[inthandler->vector], &inthandler->link);

    if(inthandler->name == NULL) {
        inthandler->name = "Unknown";
    }

    TRACE("registered interrupt #%d (%s)", inthandler->vector, inthandler->name);
}

void interrupt_free(interrupt_handler_t* inthandler) {
    // remove it
    remove_entry_list(&inthandler->link);

    // check if need to free it from the allocations
    if(inthandler->vector <= INT_ALLOCATION_BASE || inthandler->vector >= 0xf0) return;
    inthandler -= INT_ALLOCATION_BASE;
    if(g_interrupt_vectors[inthandler->vector] > 0) {
        g_interrupt_vectors[inthandler->vector]--;
    }
}