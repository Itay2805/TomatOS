#include <graphics/term.h>
#include <common/stdbool.h>
#include <memory/vmm.h>
#include <common/kernel_info.h>
#include <cpu/rflags.h>
#include <cpu/control.h>
#include <common/string.h>
#include <common/common.h>
#include <process/scheduler.h>
#include <process/process.h>
#include <common/logging.h>
#include "isr.h"

#include "idt.h"

interrupt_handler_f isr_handlers[32] = {0};

void isr_init() {
    LOG_DEBUG("Setting exception stubs");

    idt_set_entry(ISR_DIVIDE_BY_ZERO, isr_divide_by_zero, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_DEBUG, isr_debug, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_NON_MASKABLE_INTERRUPT, isr_non_maskable_interrupt, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_BREAKPOINT, isr_breakpoint, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_OVERFLOW, isr_overflow, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_BOUND_RANGE_EXCEEDED, isr_bound_range_exceeded, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_INVALID_OPCODE, isr_invalid_opcode, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_DEVICE_NOT_AVAILABLE, isr_device_not_available, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_X87_FLOATING_POINT_EXCEPTION, isr_x87_floating_point_exception, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_MACHINE_CHECK, isr_machine_check, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_SIMD_FLOATING_POINT_EXCEPTION, isr_simd_floating_point_exception, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_VIRTUALIZATION_EXCEPTION, isr_virtualization_exception, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_DOUBLE_FAULT, isr_double_fault, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_INVALID_TSS, isr_invalid_tss, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_SEGMENT_NOT_PRESENTED, isr_segment_not_presented, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_STACK_SEGMENT_FAULT, isr_stack_segment_fault, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_GENERAL_PROTECTION_FAULT, isr_general_protection_fault, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_PAGE_FAULT, isr_page_fault, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_ALIGNMENT_CHECK, isr_alignment_check, IDT_INTERRUPT_GATE);
    idt_set_entry(ISR_SECURITY_EXCEPTION, isr_security_exception, IDT_INTERRUPT_GATE);
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
    // set a valid address space
    vmm_set(kernel_address_space);

    // print error name
    const char* name = 0;
    if(regs->int_num <= sizeof(ISR_NAMES) / sizeof(char*)) {
        name = ISR_NAMES[regs->int_num];
    }
    if(name == 0) {
        LOG_CRITICAL("Exception occurred: %p", (void *)regs->int_num);
    }else {
        LOG_CRITICAL("Exception occurred: %s", name);
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
                LOG_CRITICAL("Selector(processor=%s, table=%s, index=%d)",
                        PROCESSOR_NAME[regs->error_code & 0b1],
                        TABLE_NAME[(regs->error_code >> 1) & 0b11],
                        (int) (regs->error_code & 0xFFF8) / 16);
            }
            break;
        case ISR_PAGE_FAULT:
            LOG_CRITICAL("Reason: %s", PRESENT_NAMES[regs->error_code & 1]);
            LOG_CRITICAL("Address: 0x%p", (void *)get_cr2());
            LOG_CRITICAL("Mode: %s", USER_NAME[(regs->error_code >> 2) & 1]);
            if(((regs->error_code >> 4) & 1) != 0) {
                LOG_CRITICAL("Operation: Instruction Fetch");
            }else if(((regs->error_code >> 3) & 1) != 0) {
                LOG_CRITICAL("Operation: Reserved write");
            }else {
                LOG_CRITICAL("Operation: %s", OPERATION_NAME[(regs->error_code >> 1) & 1]);
            }
            break;
        default:
            break;
    }

    // print process stuff
    if(running_thread == NULL) {
        LOG_INFO("pid=N/A");
        LOG_INFO("tid=N/A");
    }else {
        LOG_INFO("pid=%d", running_thread->parent->pid);
        LOG_INFO("tid=%d", (int) running_thread->tid);
    }

    // print registers
    LOG_INFO("rip=0x%p rsp=0x%p rbp=0x%p", (void *)regs->rip, (void *)regs->rsp, (void *)regs->rbp);
    LOG_INFO("rdi=0x%p rsi=0x%p", (void *)regs->rdi, (void *)regs->rsi);
    LOG_INFO("rax=0x%p rbx=0x%p rcx=0x%p rdx=0x%p", (void *)regs->rax, (void *)regs->rbx, (void *)regs->rcx, (void *)regs->rdx);
    LOG_INFO("r8 =0x%p r9 =0x%p r10=0x%p r11=0x%p", (void *)regs->r8, (void *)regs->r9, (void *)regs->r10, (void *)regs->r11);
    LOG_INFO("r12=0x%p r13=0x%p r14=0x%p r15=0x%p", (void *)regs->r12, (void *)regs->r13, (void *)regs->r14, (void *)regs->r15);

    LOG_INFO("cs=0x%x ss=0x%x ds=0x%x", (int)regs->cs, (int)regs->ss, (int)regs->ds);
    

    // print flags
    // for extended information look:
    // https://en.wikipedia.org/wiki/FLAGS_register
    LOG_INFO("FLAGS: 0x%p", (void*)regs->rflags);

    LOG_CRITICAL(":(");
    cli();
    while(true) {
        hlt();
    }
}

void isr_common(registers_t regs) {
    // will simply set the stack so we will have a working space
    default_exception_handler(&regs);
}
