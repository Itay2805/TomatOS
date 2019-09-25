#include "interrupts.h"

#include <stb/stb_ds.h>
#include <common/cpu/rflags.h>
#include <common/cpu/cr.h>
#include <interrupts/apic/lapic.h>
#include <interrupts/apic/apic.h>
#include <logger/term/term.h>

//////////////////////////////////////////////////////////////////
// Exception handling
//////////////////////////////////////////////////////////////////

static const char* ISR_NAMES[] = {
        "Divide by zero",
        "Debug",
        "Non-Maskable Interrupt (NMI)",
        "Breakpoint",
        "Overflow",
        "Bound Range Exceeded",
        "Invalid opcode",
        "Device Not Available",
        "Double Fault",
        "Co-processor segment overrun",
        "Invalid TSS",
        "Segment not presented",
        "Stack segment Fault",
        "General Protection Fault",
        "Page Fault",
        "Reserved (0x0f)",
        "x87 Floating Point Exception",
        "Alignment Check",
        "Machine Check",
        "SIMD Floating Point Exception",
        "Virtualization Exception",
        "Reserved (0x15)",
        "Reserved (0x16)",
        "Reserved (0x17)",
        "Reserved (0x18)",
        "Reserved (0x19)",
        "Reserved (0x1a)",
        "Reserved (0x1b)",
        "Reserved (0x1c)",
        "Reserved (0x1d)",
        "Security Exception",
        "Reserved (0x1f)",
        "Reserved (0x20)",
};

static void default_exception_handler(registers_t* regs) {
    // term_disable();

    // print error name
    const char* name = 0;
    if(regs->int_num <= sizeof(ISR_NAMES) / sizeof(char*)) {
        name = ISR_NAMES[regs->int_num];
    }
    if(name == 0) {
        log_critical("Exception occurred: %p", (void *)regs->int_num);
    }else {
        log_critical("Exception occurred: %s", name);
    }

    // term_set_background_color(COLOR_BLACK);

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

    // print error if any
    switch(regs->int_num) {
        case 10: // Invalid TSS
        case 11: // Segment Not Presented
        case 12: // Stack segment fault
        case 13: // General Protection Fault
            if(regs->error_code != 0) {
                log_critical("Selector(processor=%s, table=%s, index=%d)",
                             PROCESSOR_NAME[regs->error_code & 0b1],
                             TABLE_NAME[(regs->error_code >> 1) & 0b11],
                             (int) (regs->error_code & 0xFFF8));
            }
            break;
        case 14: // page fault
            log_critical("Reason: %s", PRESENT_NAMES[regs->error_code & 1]);
            log_critical("Address: 0x%p", (void *)get_cr2());
            log_critical("Mode: %s", USER_NAME[(regs->error_code >> 2) & 1]);
            if(((regs->error_code >> 4) & 1) != 0) {
                log_critical("Operation: Instruction Fetch");
            }else if(((regs->error_code >> 3) & 1) != 0) {
                log_critical("Operation: Reserved write");
            }else {
                log_critical("Operation: %s", OPERATION_NAME[(regs->error_code >> 1) & 1]);
            }
            break;
        default:
            break;
    }

    // procid/lapic id
    // log_error("CPU #%d/%d", apic_get_processor_id(), lapic_get_id());

    // print registers
    char _cf = (char) ((regs->rflags & FLAGS_CARRY) != 0 ? 'C' : '-');
    char _pf = (char) ((regs->rflags & FLAGS_PARITY) != 0 ? 'P' : '-');
    char _af = (char) ((regs->rflags & FLAGS_ADJUST) != 0 ? 'A' : '-');
    char _zf = (char) ((regs->rflags & FLAGS_ZERO) != 0 ? 'Z' : '-');
    char _sf = (char) ((regs->rflags & FLAGS_CARRY) != 0 ? 'S' : '-');
    char _tf = (char) ((regs->rflags & FLAGS_TRAP) != 0 ? 'T' : '-');
    char _if = (char) ((regs->rflags & FLAGS_INTERRUPT_ENABLE) != 0 ? 'I' : '-');

    int cpl = (int) (regs->rflags & FLAGS_IOPL_3);

    log_error("RAX=%016llx RBX=%016llx RCX=%016llx RDX=%016llx", regs->rax, regs->rbx, regs->rcx, regs->rdx);
    log_error("RSI=%016llx RDI=%016llx RBP=%016llx RSP=%016llx", regs->rsi, regs->rdi, regs->rbp, regs->rsp);
    log_error("R8 =%016llx R9 =%016llx R10=%016llx R11=%016llx", regs->r8, regs->r9, regs->r10, regs->r10);
    log_error("R12=%016llx R13=%016llx R14=%016llx R15=%016llx", regs->r12, regs->r13, regs->r14, regs->r15);
    log_error("RIP=%016llx RFL=%08lx [%c%c%c%c%c%c%c] CPL=%d", regs->rip, (uint32_t)regs->rflags,
              _if, _tf, _sf, _zf, _af, _pf, _cf, cpl);
    log_error("CS =%04x DPL=%d", regs->cs & 0xFFF8, regs->cs & 0b11);
    log_error("DS =%04x DPL=%d", regs->ds & 0xFFF8, regs->ds & 0b11);
    log_error("SS =%04x DPL=%d", regs->ss & 0xFFF8, regs->ss & 0b11);
    log_error("CR0=%08x CR2=%016llx CR3=%016llx CR4=%08x", get_cr0(), get_cr2(), get_cr3(), get_cr4());

//     only do the stack trace for kernel processes
    if(get_cr3() == kernel_address_space) {
        uintptr_t* bp = (uintptr_t *) regs->rbp;
        bp = (uintptr_t *)bp[0];
        if (bp) {
            log_error("call trace:");
            for (uintptr_t ip = bp[1]; bp; ip = bp[1], bp = (uintptr_t *)bp[0]) {
                log_error("\t%016lx", ip);
            }
        }
    }

    log_critical(":(");
    _cli();
    while(true) {
        _hlt();
    }
}

//////////////////////////////////////////////////////////////////
// Common interrupt handling
//////////////////////////////////////////////////////////////////

typedef struct int_value {
    interrupt_handler_f handler;
    void* context;
} int_value_t;

static struct interrupt_entry {
    uint8_t key;
    int_value_t* value;
}* interrupt_handlers = NULL;

error_t interrupts_init() {
    error_t err = NO_ERROR;

    CHECK_AND_RETHROW(apic_init());

cleanup:
    return err;
}

error_t interrupt_register(uint8_t vector, interrupt_handler_f handler, void* context) {
    // get the array of handlers from vector
    ptrdiff_t i = hmgeti(interrupt_handlers, (uint64_t)vector);
    if(i == -1) {
        hmput(interrupt_handlers, (uint64_t)vector, 0);
        i = hmgeti(interrupt_handlers, (uint64_t)vector);
    }

    // push the handler
    int_value_t value = {
            .context = context,
            .handler = handler,
    };
    arrpush(interrupt_handlers[i].value, value);

    return NO_ERROR;
}

void common_interrupt_handler(registers_t regs) {
    error_t err = {0};

    ptrdiff_t hi = -1;
    if(interrupt_handlers != NULL) {
        hi = hmgeti(interrupt_handlers, regs.int_num);
    }

    if(hi != -1) {
        // we got a handler, call all handlers
        int_value_t* handlers = interrupt_handlers[hi].value;
        for(int i = 0; i < arrlen(handlers); i++) {
            CHECK_AND_RETHROW(handlers[i].handler(&regs, handlers[i].context));
        }
    }else {
        if(regs.int_num <= 0x20) {
            // this is an exception, call default exception handler
            default_exception_handler(&regs);
        }

        // no handler, don't do anything
        // TODO: Make check the lapic and see if eoi is needed
    }

cleanup:
    if(err != NO_ERROR) {
        // TODO: Panic
    }
}

//////////////////////////////////////////////////////////////////
// Interrupt allocation
//////////////////////////////////////////////////////////////////

#define INTERRUPT_VECTOR_SIZE (0xff - INTERRUPTS_ALLOCATION_BASE - 1)
static int interrupt_vector[INTERRUPT_VECTOR_SIZE];
static int index = 0;

uint8_t interrupt_allocate() {
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
    return (uint8_t) (vec + INTERRUPTS_ALLOCATION_BASE);
}

void interrupt_set(uint8_t vector) {
    if(vector <= INTERRUPTS_ALLOCATION_BASE || vector >= 0xf0) return;
    vector -= INTERRUPTS_ALLOCATION_BASE;
    interrupt_vector[vector]++;
}

void interrupt_free(uint8_t vector) {
    if(vector <= INTERRUPTS_ALLOCATION_BASE || vector >= 0xf0) return;
    vector -= INTERRUPTS_ALLOCATION_BASE;
    if(interrupt_vector[vector] > 0) {
        interrupt_vector[vector]--;
    }
}
