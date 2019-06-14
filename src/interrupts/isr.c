#include "isr.h"

#include <logger/logger.h>
#include <memory/vmm.h>
#include <cpu/cr.h>
#include <cpu/rflags.h>

#include "interrupts.h"
#include "idt.h"

interrupt_handler_f isr_handlers[32] = {0};

void isr_init() {
    log_debug("Setting exception stubs");

    idt_set_entry(ISR_DIVIDE_BY_ZERO, isr_divide_by_zero, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_DEBUG, isr_debug, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_NON_MASKABLE_INTERRUPT, isr_non_maskable_interrupt, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_BREAKPOINT, isr_breakpoint, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_OVERFLOW, isr_overflow, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_BOUND_RANGE_EXCEEDED, isr_bound_range_exceeded, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_INVALID_OPCODE, isr_invalid_opcode, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_DEVICE_NOT_AVAILABLE, isr_device_not_available, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_X87_FLOATING_POINT_EXCEPTION, isr_x87_floating_point_exception, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_MACHINE_CHECK, isr_machine_check, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_SIMD_FLOATING_POINT_EXCEPTION, isr_simd_floating_point_exception, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_VIRTUALIZATION_EXCEPTION, isr_virtualization_exception, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_DOUBLE_FAULT, isr_double_fault, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_INVALID_TSS, isr_invalid_tss, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_SEGMENT_NOT_PRESENTED, isr_segment_not_presented, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_STACK_SEGMENT_FAULT, isr_stack_segment_fault, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_GENERAL_PROTECTION_FAULT, isr_general_protection_fault, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_PAGE_FAULT, isr_page_fault, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_ALIGNMENT_CHECK, isr_alignment_check, IDT_INTERRUPT_GATE, 1);
    idt_set_entry(ISR_SECURITY_EXCEPTION, isr_security_exception, IDT_INTERRUPT_GATE, 1);
}

static const char* ISR_NAMES[] = {
        [ISR_DIVIDE_BY_ZERO] = "Divide by zero",
        [ISR_DEBUG] = "Debug",
        [ISR_NON_MASKABLE_INTERRUPT] = "Non-Maskable Interrupt (NMI)",
        [ISR_BREAKPOINT] = "Breakpoint",
        [ISR_OVERFLOW] = "Overflow",
        [ISR_BOUND_RANGE_EXCEEDED] = "Bound Range Exceeded",
        [ISR_INVALID_OPCODE] = "Invalid opcode",
        [ISR_DEVICE_NOT_AVAILABLE] = "Device Not Available",
        [ISR_DOUBLE_FAULT] = "Double Fault",
        [ISR_COPROCESSOR_SEGMENT_OVERRUN] = "Co-processor segment overrun",
        [ISR_INVALID_TSS] = "Invalid TSS",
        [ISR_SEGMENT_NOT_PRESENTED] = "Segment not presented",
        [ISR_STACK_SEGMENT_FAULT] = "Stack segment Fault",
        [ISR_GENERAL_PROTECTION_FAULT] = "General Protection Fault",
        [ISR_PAGE_FAULT] = "Page Fault",
        [ISR_X87_FLOATING_POINT_EXCEPTION] = "x87 Floating Point Exception",
        [ISR_ALIGNMENT_CHECK] = "Alignment Check",
        [ISR_MACHINE_CHECK] = "Machine Check",
        [ISR_SIMD_FLOATING_POINT_EXCEPTION] = "SIMD Floating Point Exception",
        [ISR_VIRTUALIZATION_EXCEPTION] = "Virtualization Exception",
        [ISR_SECURITY_EXCEPTION] = "Security Exception",
};

static void default_exception_handler(registers_t* regs) {
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
        case ISR_INVALID_TSS:
        case ISR_SEGMENT_NOT_PRESENTED:
        case ISR_STACK_SEGMENT_FAULT:
        case ISR_GENERAL_PROTECTION_FAULT:
            if(regs->error_code != 0) {
                log_critical("Selector(processor=%s, table=%s, index=%d)",
                             PROCESSOR_NAME[regs->error_code & 0b1],
                             TABLE_NAME[(regs->error_code >> 1) & 0b11],
                             (int) (regs->error_code & 0xFFF8) / 16);
            }
            break;
        case ISR_PAGE_FAULT:
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



    // print registers

    char _cf = (char) ((regs->rflags & FLAGS_CARRY) != 0 ? 'C' : '-');
    char _pf = (char) ((regs->rflags & FLAGS_PARITY) != 0 ? 'P' : '-');
    char _af = (char) ((regs->rflags & FLAGS_ADJUST) != 0 ? 'A' : '-');
    char _zf = (char) ((regs->rflags & FLAGS_ZERO) != 0 ? 'Z' : '-');
    char _sf = (char) ((regs->rflags & FLAGS_CARRY) != 0 ? 'S' : '-');
    char _tf = (char) ((regs->rflags & FLAGS_TRAP) != 0 ? 'T' : '-');
    char _if = (char) ((regs->rflags & FLAGS_INTERRUPT_ENABLE) != 0 ? 'I' : '-');

    int cpl = (int) (regs->rflags & FLAGS_IOPL_3);

    log_info("RAX=%016llx RBX=%016llx RCX=%016llx RDX=%016llx", regs->rax, regs->rbx, regs->rcx, regs->rdx);
    log_info("RSI=%016llx RDI=%016llx RBP=%016llx RSP=%016llx", regs->rsi, regs->rdi, regs->rbp, regs->rsp);
    log_info("R8 =%016llx R9 =%016llx R10=%016llx R11=%016llx", regs->r8, regs->r9, regs->r10, regs->r10);
    log_info("R12=%016llx R13=%016llx R14=%016llx R15=%016llx", regs->r12, regs->r13, regs->r14, regs->r15);
    log_info("RIP=%016llx RFL=%08lx [%c%c%c%c%c%c%c] CPL=%d", regs->rip, (uint32_t)regs->rflags,
             _if, _tf, _sf, _zf, _af, _pf, _cf, cpl);
    log_info("CS =%04x DPL=%d", regs->cs & 0xFFF8, regs->cs & 0b11);
    log_info("DS =%04x DPL=%d", regs->ds & 0xFFF8, regs->ds & 0b11);
    log_info("SS =%04x DPL=%d", regs->ss & 0xFFF8, regs->ss & 0b11);
    log_info("CR3=%016llx", regs->cr3);

    // only do the stack trace for kernel processes
//    if(regs->cr3 == kernel_address_space) {
//        uintptr_t* bp = (uintptr_t *) regs->rbp;
//        bp = (uintptr_t *)bp[0];
//        if (bp) {
//            log_info("call trace:");
//            for (uintptr_t ip = bp[1]; bp; ip = bp[1], bp = (uintptr_t *)bp[0]) {
//                log_info("\t%016lx", ip);
//            }
//        }
//    }

    log_critical(":(");
    _cli();
    while(true) {
        _hlt();
    }
}

void isr_common(registers_t regs) {
    // save the address space
    // and switch to kernel if needed
    regs.cr3 = vmm_get();
    if(regs.cr3 != kernel_address_space) {
        vmm_set(kernel_address_space);
    }

    // will simply set the stack so we will have a working space
    default_exception_handler(&regs);

    // restore the cr3 if needed
    if(regs.cr3 != kernel_address_space) {
        vmm_set(regs.cr3);
    }
}