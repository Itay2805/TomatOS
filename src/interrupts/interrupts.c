#include <stddef.h>
#include <stdbool.h>

#include <util/debug.h>
#include <util/arch.h>
#include <util/error.h>

#include "idt.h"
#include "interrupts.h"

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

static void default_exception_handler(interrupt_context_t* regs) {
    // term_disable();

    // print error name
    const char* name = 0;
    if(regs->int_num <= sizeof(ISR_NAMES) / sizeof(char*)) {
        name = ISR_NAMES[regs->int_num];
    }
    if(name == 0) {
        debug_log("[-] Exception occurred: %x\n", regs->int_num);
    }else {
        debug_log("[-] Exception occurred: %s\n", name);
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
                debug_log("[-] Selector(processor=%s, table=%s, index=%d)\n",
                             PROCESSOR_NAME[regs->error_code & 0b1],
                             TABLE_NAME[(regs->error_code >> 1) & 0b11],
                             (int) (regs->error_code & 0xFFF8));
            }
            break;
        case 14: // page fault
            debug_log("[-] Reason: %s\n", PRESENT_NAMES[regs->error_code & 1]);
            debug_log("[-] Address: 0x%p\n", (void *)read_cr2());
            debug_log("[-] Mode: %s\n", USER_NAME[(regs->error_code >> 2) & 1]);
            if(((regs->error_code >> 4) & 1) != 0) {
                debug_log("[-] Operation: Instruction Fetch\n");
            }else if(((regs->error_code >> 3) & 1) != 0) {
                debug_log("[-] Operation: Reserved write\n");
            }else {
                debug_log("[-] Operation: %s\n", OPERATION_NAME[(regs->error_code >> 1) & 1]);
            }
            break;
        default:
            break;
    }

    // procid/lapic id
    // log_error("CPU #%d/%d", apic_get_processor_id(), lapic_get_id());

    // print registers
    char _cf = (char) ((regs->rflags.CF) != 0 ? 'C' : '-');
    char _pf = (char) ((regs->rflags.PF) != 0 ? 'P' : '-');
    char _af = (char) ((regs->rflags.AF) != 0 ? 'A' : '-');
    char _zf = (char) ((regs->rflags.ZF) != 0 ? 'Z' : '-');
    char _sf = (char) ((regs->rflags.SF) != 0 ? 'S' : '-');
    char _tf = (char) ((regs->rflags.TF) != 0 ? 'T' : '-');
    char _if = (char) ((regs->rflags.IF) != 0 ? 'I' : '-');

    int cpl = (int) (regs->rflags.IOPL);

    debug_log("[-] RAX=%016llx RBX=%016llx RCX=%016llx RDX=%016llx\n", regs->rax, regs->rbx, regs->rcx, regs->rdx);
    debug_log("[-] RSI=%016llx RDI=%016llx RBP=%016llx RSP=%016llx\n", regs->rsi, regs->rdi, regs->rbp, regs->rsp);
    debug_log("[-] R8 =%016llx R9 =%016llx R10=%016llx R11=%016llx\n", regs->r8, regs->r9, regs->r10, regs->r10);
    debug_log("[-] R12=%016llx R13=%016llx R14=%016llx R15=%016llx\n", regs->r12, regs->r13, regs->r14, regs->r15);
    debug_log("[-] RIP=%016llx RFL=%08lx [%c%c%c%c%c%c%c] CPL=%d\n", regs->rip, (uint32_t)regs->rflags.raw, _if, _tf, _sf, _zf, _af, _pf, _cf, cpl);
    debug_log("[-] CS =%04x DPL=%d\n", regs->cs & 0xFFF8, regs->cs & 0b11);
    debug_log("[-] DS =%04x DPL=%d\n", regs->ds & 0xFFF8, regs->ds & 0b11);
    debug_log("[-] SS =%04x DPL=%d\n", regs->ss & 0xFFF8, regs->ss & 0b11);
    debug_log("[-] CR0=%08x CR2=%016llx CR3=%016llx CR4=%08x\n", read_cr0(), read_cr2(), read_cr3(), read_cr4());

    idt_t idt;
    read_idtr(&idt);
    debug_log("[-] IDT=%016llx %08llx\n", idt.base, idt.base);

    debug_log("[-] :(\n");
    ASSERT(false);
}

void common_interrupt_handler(interrupt_context_t ctx) {
    error_t err = NO_ERROR;

    if(ctx.int_num < 0x20) {
        default_exception_handler(&ctx);
    }

cleanup:
    ASSERT(!IS_ERROR(err));
}
