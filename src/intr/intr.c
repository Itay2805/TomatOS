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

#include "intr.h"

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

static void default_interrupt_handler(interrupt_context_t *regs) {
    // term_disable();

    // print error name
    const char* name = 0;
    if(regs->int_num <= sizeof(ISR_NAMES) / sizeof(char*)) {
        name = ISR_NAMES[regs->int_num];
    }
    if(name == 0) {
        trace("[-] Unhandled interrupt: %x\n", regs->int_num);
    }else {
        if (regs->int_num == EXCEPTION_BREAKPOINT && __readcr2() == PANIC_CR2_MAGIC) {
            trace("[-] Got a kernel panic\n");
        } else {
            trace("[-] Exception occurred: %s\n", name);
        }
    }

    // print error if any
    switch(regs->int_num) {
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
    if (current_thread != NULL) {
        trace("[-] Process: pid=%d tid=%d\n", current_thread->parent->pid, current_thread->tid);
    }

    // print the tpl
    switch (get_tpl()) {
        case TPL_HIGH_LEVEL: trace("[-] Current TPL: TPL_HIGH_LEVEL\n"); break;
        case TPL_APPLICATION: trace("[-] Current TPL: TPL_APPLICATION\n"); break;
        case TPL_PREEMPTION: trace("[-] Current TPL: TPL_PREEMPTION\n"); break;
        case TPL_CALLBACK: trace("[-] Current TPL: TPL_CALLBACK\n"); break;
        case TPL_IO_CALLBACK: trace("[-] Current TPL: TPL_IO_CALLBACK\n"); break;
        case TPL_TIMER: trace("[-] Current TPL: TPL_TIMER\n"); break;
        case TPL_USER_HIGH: trace("[-] Current TPL: TPL_USER_HIGH\n"); break;

        default:
            trace("[-] Current TPL: %d\n", get_tpl());
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

    trace("[-] RAX=%016llx RBX=%016llx RCX=%016llx RDX=%016llx\n", regs->rax, regs->rbx, regs->rcx, regs->rdx);
    trace("[-] RSI=%016llx RDI=%016llx RBP=%016llx RSP=%016llx\n", regs->rsi, regs->rdi, regs->rbp, regs->rsp);
    trace("[-] R8 =%016llx R9 =%016llx R10=%016llx R11=%016llx\n", regs->r8, regs->r9, regs->r10, regs->r10);
    trace("[-] R12=%016llx R13=%016llx R14=%016llx R15=%016llx\n", regs->r12, regs->r13, regs->r14, regs->r15);
    trace("[-] RIP=%016llx <%s+%x> RFL=%08lx [%c%c%c%c%c%c%c] CPL=%d\n", regs->rip, name, off, (uint32_t)regs->rflags.raw, _if, _tf, _sf, _zf, _af, _pf, _cf, cpl);
    trace("[-] CS =%04x                  DPL=%d\n", regs->cs & 0xFFF8, regs->cs & 0b11);
    trace("[-] DS =%04x                  DPL=%d\n", regs->ds & 0xFFF8, regs->ds & 0b11);
    trace("[-] SS =%04x                  DPL=%d\n", regs->ss & 0xFFF8, regs->ss & 0b11);
    trace("[-] GS =     %016llx DPL= \n", __readmsr(MSR_CODE_IA32_GS_BASE));
    trace("[-] FS =     %016llx DPL= \n", __readmsr(MSR_CODE_IA32_FS_BASE));
    trace("[-] CR0=%08x CR2=%016llx CR3=%016llx CR4=%08x\n", __readcr0(), __readcr2(), __readcr3(), __readcr4());

    uintptr_t* base_ptr = (uintptr_t*)regs->rbp;
    trace("[-] Stack trace:\n");
    for (;;) {
        // check if the page is mapped
        if (!vmm_is_mapped(current_vmm_handle, (uintptr_t)base_ptr, sizeof(uintptr_t) * 2)) {
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
            trace("[-] \t[0x%016llx] <%s+%x>\n", ret_addr, name, off);
        }

        if (old_bp == 0) {
            break;
        }

        base_ptr = (void*)old_bp;
    }

    trace("[-] \n");
    trace("[-] :(\n");
    while(1) asm("hlt");
}

//////////////////////////////////////////////////////////////////
// Interrupt handling
//////////////////////////////////////////////////////////////////

#define INTERRUPT_VECTOR_SIZE (0xff - INT_ALLOCATION_BASE - 1)
static int interrupt_vector[INTERRUPT_VECTOR_SIZE];
static int index = 0;

static list_entry_t interrupt_handlers[256];

void interrupts_init() {
    // just initialize all the list entries
    for(int i = 0; i < 256; i++) {
        interrupt_handlers[i] = INIT_LIST_ENTRY(interrupt_handlers[i]);
    }
}

void common_interrupt_handler(interrupt_context_t ctx) {
    err_t err = NO_ERROR;

    if(is_list_empty(&interrupt_handlers[ctx.int_num])) {

        // just call the default handler if none is found
        default_interrupt_handler(&ctx);

    }else {

        // iterate and call all the handlers
        list_entry_t* handlers = &interrupt_handlers[ctx.int_num];
        for(list_entry_t* link = handlers->next; link != handlers; link = link->next) {
            interrupt_handler_t* handler = CR(link, interrupt_handler_t, link);
            CHECK_AND_RETHROW(handler->callback(handler->user_param, &ctx));
        }
    }

    // raise and lower tpl so we can call all callbacks
    restore_tpl(raise_tpl(TPL_HIGH_LEVEL));

    // the tpl can not be high level when exiting the interrupt handler!
    ASSERT(get_tpl() != TPL_HIGH_LEVEL);

cleanup:
    WARN(!IS_ERROR(err), "Got an error inside interrupt handler/callback");
}

//////////////////////////////////////////////////////////////////
// Interrupt manipulation
//////////////////////////////////////////////////////////////////

static uint8_t interrupt_allocate() {
    int vec = index;
    for(int i = index; i < INTERRUPT_VECTOR_SIZE; i++) {
        if(interrupt_vector[i] < interrupt_vector[vec]) {
            vec = i;

            // if has zero uses we can just return it
            if(interrupt_vector[vec] == 0) goto found;
        }
    }

    // we could not find a completely free one
    // try to search from the start for one
    if(index != 0) {
        for(int i = 0; i < index; i++) {
            if(interrupt_vector[i] < interrupt_vector[vec]) {
                vec = i;

                // if has zero uses we can just return it
                if(interrupt_vector[vec] == 0) goto found;
            }
        }
    }

    // if we could still not find a good one, just use whatever
    // we got

found:
    // increment the usage count, set the
    // new index to start from, and return
    // the correct number
    interrupt_vector[vec]++;
    index = (vec + 1) % INTERRUPT_VECTOR_SIZE;
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
    insert_tail_list(&interrupt_handlers[inthandler->vector], &inthandler->link);

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
    if(interrupt_vector[inthandler->vector] > 0) {
        interrupt_vector[inthandler->vector]--;
    }
}