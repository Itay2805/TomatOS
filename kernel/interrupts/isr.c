#include <graphics/term.h>
#include <common/stdbool.h>
#include <memory/vmm.h>
#include <common/kernel_info.h>
#include <process/rflags.h>
#include "isr.h"

#include "idt.h"

interrupt_handler_f isr_handlers[32] = {0};

void isr_init() {
    term_write("[isr_init] Setting exception stubs\n");

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

extern uint64_t get_cr2();

static void default_exception_handler(registers_t* regs) {
    // set a valid address space
    // address_space_t curr = vmm_get();
    vmm_set(kernel_address_space);

    // clear screen
    term_set_background_color(COLOR_BLACK);
    term_set_text_color(COLOR_WHITE);
    term_clear();
    term_set_cursor_pos(0, 0);
    term_set_background_color(COLOR_RED);
    term_set_text_color(COLOR_WHITE);

    // print error name
    const char* name = ISR_NAMES[regs->int_num];
    if(name == 0) {
        term_print("Exception occurred: %p\n\n", (void *)regs->int_num);
    }else {
        term_print("Exception occurred: %s\n\n", name);
    }

    term_set_background_color(COLOR_BLACK);

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
            term_print("Selector(processor=%s, table=%s, index=%d)\n",
                       PROCESSOR_NAME[regs->error_code & 0b1],
                       TABLE_NAME[(regs->error_code >> 1) & 0b11],
                       (int) ((regs->error_code >> 3) & 0xFFF8));
            break;
        case ISR_PAGE_FAULT:
            term_print("Reason: %s\n", PRESENT_NAMES[regs->error_code & 1]);
            term_print("Address: 0x%p\n", (void *)get_cr2());
            term_print("Mode: %s\n", USER_NAME[(regs->error_code >> 2) & 1]);
            if(((regs->error_code >> 4) & 1) != 0) {
                term_print("Operation: Instruction Fetch\n");
            }else if(((regs->error_code >> 3) & 1) != 0) {
                term_write("Operation: Reserved write\n");
            }else {
                term_print("Operation: %s\n", OPERATION_NAME[(regs->error_code >> 1) & 1]);
            }
            break;
        default:
            break;
    }
    term_write("\n");

    // print registers
    term_print("rip=0x%p rsp=0x%p rbp=0x%p\n", (void *)regs->rip, (void *)regs->rsp, (void *)regs->rbp);
    term_print("rdi=0x%p rsi=0x%p\n", (void *)regs->rdi, (void *)regs->rsi);
    term_print("rax=0x%p rbx=0x%p rcx=0x%p rdx=0x%p\n", (void *)regs->rax, (void *)regs->rbx, (void *)regs->rcx, (void *)regs->rdx);
    term_print("r8 =0x%p r9 =0x%p r10=0x%p r11=0x%p\n", (void *)regs->r8, (void *)regs->r9, (void *)regs->r10, (void *)regs->r11);
    term_print("r12=0x%p r13=0x%p r14=0x%p r15=0x%p\n", (void *)regs->r12, (void *)regs->r13, (void *)regs->r14, (void *)regs->r15);

    // print flags
    // for extended information look:
    // https://en.wikipedia.org/wiki/FLAGS_register
    term_print("\nFLAGS:\n");
    if((regs->rflags & FLAGS_CARRY) != 0) term_print("CF ");
    if((regs->rflags & FLAGS_PARITY) != 0) term_print("PF ");
    if((regs->rflags & FLAGS_ADJUST) != 0) term_print("AF ");
    if((regs->rflags & FLAGS_ZERO) != 0) term_print("ZF ");
    if((regs->rflags & FLAGS_SIGN) != 0) term_print("SF ");
    if((regs->rflags & FLAGS_TRAP) != 0) term_print("TF ");
    if((regs->rflags & FLAGS_INTERRUPT_ENABLE) != 0) term_print("IF ");
    if((regs->rflags & FLAGS_DIRECTION) != 0) term_print("DF ");
    if((regs->rflags & FLAGS_OVERFLOW) != 0) term_print("OF ");
    int iopl = (int) ((regs->rflags >> 12) & 3);
    term_print("IOPL%d ", iopl);
    if((regs->rflags & FLAGS_NESTED_TASK) != 0) term_print("NT ");

    term_print("\n\nEFLAGS:\n");
    if((regs->rflags & EFLAGS_RESUME) != 0) term_print("RF ");
    if((regs->rflags & EFLAGS_V8086) != 0) term_print("VM ");
    if((regs->rflags & ISR_ALIGNMENT_CHECK) != 0) term_print("AC ");
    if((regs->rflags & EFLAGS_VIRTUAL_INTERRUPT) != 0) term_print("VIF ");
    if((regs->rflags & EFLAGS_VIRTUAL_INTERRUPT_PENDING) != 0) term_print("VIP ");
    if((regs->rflags & EFLAGS_CPUID) != 0) term_print("ID ");

    term_print("\n\nRFLAGS:\n");

    term_write("\n\nhalting...");
    cli();
    while(true) {
        hlt();
    }
}

void isr_common(registers_t regs) {
    // will simply set the stack so we will have a working space
    register void* rsp asm ("rsp");
    rsp = KERNEL_STACK;
    ((void)rsp);
    default_exception_handler(&regs);
}
