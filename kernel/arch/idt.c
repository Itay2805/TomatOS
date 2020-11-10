#include <util/trace.h>
#include <arch/cpu.h>
#include <util/except.h>
#include "idt.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Interrupts without signals, this applies to:
//  - exceptions
//  - scheduler tick
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * The common handler that will push all registers and call
 * the true common handler with the system_context_t
 */
__attribute__((naked, used))
void common_exception_stub() {
    asm("cld\n"
        "pushq %rax\n"
        "pushq %rbx\n"
        "pushq %rcx\n"
        "pushq %rdx\n"
        "pushq %rsi\n"
        "pushq %rdi\n"
        "pushq %rbp\n"
        "pushq %r8\n"
        "pushq %r9\n"
        "pushq %r10\n"
        "pushq %r11\n"
        "pushq %r12\n"
        "pushq %r13\n"
        "pushq %r14\n"
        "pushq %r15\n"
        "movq %ds, %rax\n"
        "pushq %rax\n"
        "movw $16, %ax\n"
        "movw %ax, %ds\n"
        "movw %ax, %es\n"
        "movw %ax, %ss\n"
        "movq %rsp, %rdi\n"
        "call common_exception_handler\n"
        "popq %rax\n"
        "movw %ax, %ds\n"
        "movw %ax, %es\n"
        "popq %r15\n"
        "popq %r14\n"
        "popq %r13\n"
        "popq %r12\n"
        "popq %r11\n"
        "popq %r10\n"
        "popq %r9\n"
        "popq %r8\n"
        "popq %rbp\n"
        "popq %rdi\n"
        "popq %rsi\n"
        "popq %rdx\n"
        "popq %rcx\n"
        "popq %rbx\n"
        "popq %rax\n"
        "addq $16, %rsp\n"
        "iretq");
}

/**
 * Define exception stub that has no error
 */
#define EXCEPTION_STUB(num) \
    __attribute__((naked)) \
    void interrupt_handle_##num() { \
        asm("pushq $0\n" \
            "pushq $" #num "\n" \
            "jmp common_exception_stub"); \
    }

/**
 * Define exception stub that has an error code
 */
#define EXCEPTION_ERROR_STUB(num) \
    __attribute__((naked)) \
    void interrupt_handle_##num() { \
        asm("pushq $" #num "\n" \
            "jmp common_exception_stub"); \
    }

/**
 * Define all exception handlers
 * this includes the scheduler tick
 * interrupt
 */
EXCEPTION_STUB(0x00);
EXCEPTION_STUB(0x01);
EXCEPTION_STUB(0x02);
EXCEPTION_STUB(0x03);
EXCEPTION_STUB(0x04);
EXCEPTION_STUB(0x05);
EXCEPTION_STUB(0x06);
EXCEPTION_STUB(0x07);
EXCEPTION_ERROR_STUB(0x08);
EXCEPTION_STUB(0x09);
EXCEPTION_ERROR_STUB(0x0a);
EXCEPTION_ERROR_STUB(0x0b);
EXCEPTION_ERROR_STUB(0x0c);
EXCEPTION_ERROR_STUB(0x0d);
EXCEPTION_ERROR_STUB(0x0e);
EXCEPTION_STUB(0x0f);
EXCEPTION_ERROR_STUB(0x10);
EXCEPTION_STUB(0x11);
EXCEPTION_STUB(0x12);
EXCEPTION_STUB(0x13);
EXCEPTION_STUB(0x14);
EXCEPTION_STUB(0x15);
EXCEPTION_STUB(0x16);
EXCEPTION_STUB(0x17);
EXCEPTION_STUB(0x18);
EXCEPTION_STUB(0x19);
EXCEPTION_STUB(0x1a);
EXCEPTION_STUB(0x1b);
EXCEPTION_STUB(0x1c);
EXCEPTION_STUB(0x1d);
EXCEPTION_STUB(0x1e);
EXCEPTION_STUB(0x1f);
EXCEPTION_STUB(0x20);

static const char* g_exception_name[] = {
    "#DE - Divide Error",
    "#DB - Debug",
    "Non Maskable Interrupt",
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
    "#GP - General Protection Fault",
    "#PF - Page-Fault",
    "Reserved",
    "#MF - x87 FPU Floating-Point Error",
    "#AC - Alignment Check",
    "#MC - Machine-Check",
    "#XM - SIMD Floating-Point Exception",
    "#VE - Virtualization Exception",
    "#CP - Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "#SX - Security Exception",
    "Reserved"
};

static const char* g_pf_reason[] = {
        "Supervisory process tried to read a non-present page entry",
        "Supervisory process tried to read a page and caused a protection fault",
        "Supervisory process tried to write to a non-present page entry",
        "Supervisory process tried to write a page and caused a protection fault",
        "User process tried to read a non-present page entry",
        "User process tried to read a page and caused a protection fault",
        "User process tried to write to a non-present page entry",
        "User process tried to write a page and caused a protection fault"
};

static void default_exception_handler(system_context_t* ctx) {
    if (!ticket_try_lock(&g_trace_lock)) {
        ticket_unlock(&g_trace_lock);
    }

    ERROR("");
    ERROR("****************************************************");
    ERROR("Exception occurred: ", g_exception_name[ctx->int_num]);
    ERROR("****************************************************");
    ERROR("");
    if (ctx->int_num == 0xE) {
        if (ctx->error_code & BIT3) {
            ERROR("one or more page directory entries contain reserved bits which are set to 1");
        } else {
            ERROR(g_pf_reason[ctx->error_code & 0b111]);
        }
        ERROR("");
    }
//    ERROR("Cpu: #%d", g_cpu_id);
//    if (g_current_process != NULL) {
//        ERROR("Process: `%s` (#%d)", g_current_process->name, g_current_process->pid);
//    }
//    if (g_current_thread != NULL) {
//        ERROR("Thread: `%s` (#%d)", g_current_thread->name, g_current_thread->tid);
//    }
//    ERROR("");

    ERROR("RAX=", (void*)ctx->rax, " RBX=", (void*)ctx->rbx, " RCX=", (void*)ctx->rcx, " RDX=", (void*)ctx->rdx);
    ERROR("RSI=", (void*)ctx->rsi, " RDI=", (void*)ctx->rdi, " RBP=", (void*)ctx->rbp, " RSP=", (void*)ctx->rsp);
    ERROR("R8 =", (void*)ctx->r8 , " R9 =", (void*)ctx->r9 , " R10=", (void*)ctx->r10, " R11=", (void*)ctx->r11);
    ERROR("R12=", (void*)ctx->r12, " R13=", (void*)ctx->r13, " R14=", (void*)ctx->r14, " R15=", (void*)ctx->r15);
    ERROR("RIP=", (void*)ctx->rip, " RFL=", (void*)ctx->rflags.raw);
    // TODO: ERROR("CR0=%08x CR2=%016llx CR3=%016llx CR4=%08x", __readcr0().raw, __readcr2(), __readcr3(), __readcr4().raw);

    ERROR("");
//    if (g_exception_count == 0) {
//        g_exception_count++;
//        ERROR("Stack trace:");
//        debug_trace_stack((void*)ctx->rbp);
//        ERROR("");
//    }
    ERROR("Halting :(");
    while(1)
        __hlt();
}

__attribute__((used))
void common_exception_handler(system_context_t* ctx) {
    default_exception_handler(ctx);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define IDT structures and set the idt up
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * All interrupt handler entries
 */
static idt_entry_t g_idt_entries[0xFF + 1];

/**
 * The idt
 */
idt_t g_idt = {
    .limit = sizeof(g_idt_entries) - 1,
    .base = g_idt_entries
};

static void set_idt_entry(uint8_t i, void(*handler)(), int ist) {
    g_idt_entries[i].handler_low = (uint16_t) ((uintptr_t)handler & 0xFFFF);
    g_idt_entries[i].handler_high = (uint64_t) ((uintptr_t)handler >> 16);
    g_idt_entries[i].gate_type = IDT_TYPE_INTERRUPT_32;
    g_idt_entries[i].selector = GDT_KERNEL_CODE;
    g_idt_entries[i].present = 1;
    g_idt_entries[i].ring = 0;
    g_idt_entries[i].ist = 0;
    // TODO: don't ignore ist value
//    g_idt_entries[i].ist = (uint64_t) ist;
}

void init_idt() {
    // setup full idt
#ifndef __IN_EDITOR__
    set_idt_entry(0x0, interrupt_handle_0x00, 1);
    set_idt_entry(0x1, interrupt_handle_0x01, 1);
    set_idt_entry(0x2, interrupt_handle_0x02, 2);
    set_idt_entry(0x3, interrupt_handle_0x03, 1);
    set_idt_entry(0x4, interrupt_handle_0x04, 1);
    set_idt_entry(0x5, interrupt_handle_0x05, 1);
    set_idt_entry(0x6, interrupt_handle_0x06, 1);
    set_idt_entry(0x7, interrupt_handle_0x07, 1);
    set_idt_entry(0x8, interrupt_handle_0x08, 3);
    set_idt_entry(0x9, interrupt_handle_0x09, 1);
    set_idt_entry(0xa, interrupt_handle_0x0a, 1);
    set_idt_entry(0xb, interrupt_handle_0x0b, 1);
    set_idt_entry(0xc, interrupt_handle_0x0c, 3);
    set_idt_entry(0xd, interrupt_handle_0x0d, 3);
    set_idt_entry(0xe, interrupt_handle_0x0e, 3);
    set_idt_entry(0xf, interrupt_handle_0x0f, 1);
    set_idt_entry(0x10, interrupt_handle_0x10, 1);
    set_idt_entry(0x11, interrupt_handle_0x11, 1);
    set_idt_entry(0x12, interrupt_handle_0x12, 1);
    set_idt_entry(0x13, interrupt_handle_0x13, 1);
    set_idt_entry(0x14, interrupt_handle_0x14, 1);
    set_idt_entry(0x15, interrupt_handle_0x15, 1);
    set_idt_entry(0x16, interrupt_handle_0x16, 1);
    set_idt_entry(0x17, interrupt_handle_0x17, 1);
    set_idt_entry(0x18, interrupt_handle_0x18, 1);
    set_idt_entry(0x19, interrupt_handle_0x19, 1);
    set_idt_entry(0x1a, interrupt_handle_0x1a, 1);
    set_idt_entry(0x1b, interrupt_handle_0x1b, 1);
    set_idt_entry(0x1c, interrupt_handle_0x1c, 1);
    set_idt_entry(0x1d, interrupt_handle_0x1d, 1);
    set_idt_entry(0x1e, interrupt_handle_0x1e, 1);
    set_idt_entry(0x1f, interrupt_handle_0x1f, 1);
    set_idt_entry(0x20, interrupt_handle_0x20, 0);
//    set_idt_entry(0x21, interrupt_handle_0x21, 0);
//    set_idt_entry(0x22, interrupt_handle_0x22, 0);
//    set_idt_entry(0x23, interrupt_handle_0x23, 0);
//    set_idt_entry(0x24, interrupt_handle_0x24, 0);
//    set_idt_entry(0x25, interrupt_handle_0x25, 0);
//    set_idt_entry(0x26, interrupt_handle_0x26, 0);
//    set_idt_entry(0x27, interrupt_handle_0x27, 0);
//    set_idt_entry(0x28, interrupt_handle_0x28, 0);
//    set_idt_entry(0x29, interrupt_handle_0x29, 0);
//    set_idt_entry(0x2a, interrupt_handle_0x2a, 0);
//    set_idt_entry(0x2b, interrupt_handle_0x2b, 0);
//    set_idt_entry(0x2c, interrupt_handle_0x2c, 0);
//    set_idt_entry(0x2d, interrupt_handle_0x2d, 0);
//    set_idt_entry(0x2e, interrupt_handle_0x2e, 0);
//    set_idt_entry(0x2f, interrupt_handle_0x2f, 0);
//    set_idt_entry(0x30, interrupt_handle_0x30, 0);
//    set_idt_entry(0x31, interrupt_handle_0x31, 0);
//    set_idt_entry(0x32, interrupt_handle_0x32, 0);
//    set_idt_entry(0x33, interrupt_handle_0x33, 0);
//    set_idt_entry(0x34, interrupt_handle_0x34, 0);
//    set_idt_entry(0x35, interrupt_handle_0x35, 0);
//    set_idt_entry(0x36, interrupt_handle_0x36, 0);
//    set_idt_entry(0x37, interrupt_handle_0x37, 0);
//    set_idt_entry(0x38, interrupt_handle_0x38, 0);
//    set_idt_entry(0x39, interrupt_handle_0x39, 0);
//    set_idt_entry(0x3a, interrupt_handle_0x3a, 0);
//    set_idt_entry(0x3b, interrupt_handle_0x3b, 0);
//    set_idt_entry(0x3c, interrupt_handle_0x3c, 0);
//    set_idt_entry(0x3d, interrupt_handle_0x3d, 0);
//    set_idt_entry(0x3e, interrupt_handle_0x3e, 0);
//    set_idt_entry(0x3f, interrupt_handle_0x3f, 0);
//    set_idt_entry(0x40, interrupt_handle_0x40, 0);
//    set_idt_entry(0x41, interrupt_handle_0x41, 0);
//    set_idt_entry(0x42, interrupt_handle_0x42, 0);
//    set_idt_entry(0x43, interrupt_handle_0x43, 0);
//    set_idt_entry(0x44, interrupt_handle_0x44, 0);
//    set_idt_entry(0x45, interrupt_handle_0x45, 0);
//    set_idt_entry(0x46, interrupt_handle_0x46, 0);
//    set_idt_entry(0x47, interrupt_handle_0x47, 0);
//    set_idt_entry(0x48, interrupt_handle_0x48, 0);
//    set_idt_entry(0x49, interrupt_handle_0x49, 0);
//    set_idt_entry(0x4a, interrupt_handle_0x4a, 0);
//    set_idt_entry(0x4b, interrupt_handle_0x4b, 0);
//    set_idt_entry(0x4c, interrupt_handle_0x4c, 0);
//    set_idt_entry(0x4d, interrupt_handle_0x4d, 0);
//    set_idt_entry(0x4e, interrupt_handle_0x4e, 0);
//    set_idt_entry(0x4f, interrupt_handle_0x4f, 0);
//    set_idt_entry(0x50, interrupt_handle_0x50, 0);
//    set_idt_entry(0x51, interrupt_handle_0x51, 0);
//    set_idt_entry(0x52, interrupt_handle_0x52, 0);
//    set_idt_entry(0x53, interrupt_handle_0x53, 0);
//    set_idt_entry(0x54, interrupt_handle_0x54, 0);
//    set_idt_entry(0x55, interrupt_handle_0x55, 0);
//    set_idt_entry(0x56, interrupt_handle_0x56, 0);
//    set_idt_entry(0x57, interrupt_handle_0x57, 0);
//    set_idt_entry(0x58, interrupt_handle_0x58, 0);
//    set_idt_entry(0x59, interrupt_handle_0x59, 0);
//    set_idt_entry(0x5a, interrupt_handle_0x5a, 0);
//    set_idt_entry(0x5b, interrupt_handle_0x5b, 0);
//    set_idt_entry(0x5c, interrupt_handle_0x5c, 0);
//    set_idt_entry(0x5d, interrupt_handle_0x5d, 0);
//    set_idt_entry(0x5e, interrupt_handle_0x5e, 0);
//    set_idt_entry(0x5f, interrupt_handle_0x5f, 0);
//    set_idt_entry(0x60, interrupt_handle_0x60, 0);
//    set_idt_entry(0x61, interrupt_handle_0x61, 0);
//    set_idt_entry(0x62, interrupt_handle_0x62, 0);
//    set_idt_entry(0x63, interrupt_handle_0x63, 0);
//    set_idt_entry(0x64, interrupt_handle_0x64, 0);
//    set_idt_entry(0x65, interrupt_handle_0x65, 0);
//    set_idt_entry(0x66, interrupt_handle_0x66, 0);
//    set_idt_entry(0x67, interrupt_handle_0x67, 0);
//    set_idt_entry(0x68, interrupt_handle_0x68, 0);
//    set_idt_entry(0x69, interrupt_handle_0x69, 0);
//    set_idt_entry(0x6a, interrupt_handle_0x6a, 0);
//    set_idt_entry(0x6b, interrupt_handle_0x6b, 0);
//    set_idt_entry(0x6c, interrupt_handle_0x6c, 0);
//    set_idt_entry(0x6d, interrupt_handle_0x6d, 0);
//    set_idt_entry(0x6e, interrupt_handle_0x6e, 0);
//    set_idt_entry(0x6f, interrupt_handle_0x6f, 0);
//    set_idt_entry(0x70, interrupt_handle_0x70, 0);
//    set_idt_entry(0x71, interrupt_handle_0x71, 0);
//    set_idt_entry(0x72, interrupt_handle_0x72, 0);
//    set_idt_entry(0x73, interrupt_handle_0x73, 0);
//    set_idt_entry(0x74, interrupt_handle_0x74, 0);
//    set_idt_entry(0x75, interrupt_handle_0x75, 0);
//    set_idt_entry(0x76, interrupt_handle_0x76, 0);
//    set_idt_entry(0x77, interrupt_handle_0x77, 0);
//    set_idt_entry(0x78, interrupt_handle_0x78, 0);
//    set_idt_entry(0x79, interrupt_handle_0x79, 0);
//    set_idt_entry(0x7a, interrupt_handle_0x7a, 0);
//    set_idt_entry(0x7b, interrupt_handle_0x7b, 0);
//    set_idt_entry(0x7c, interrupt_handle_0x7c, 0);
//    set_idt_entry(0x7d, interrupt_handle_0x7d, 0);
//    set_idt_entry(0x7e, interrupt_handle_0x7e, 0);
//    set_idt_entry(0x7f, interrupt_handle_0x7f, 0);
//    set_idt_entry(0x80, interrupt_handle_0x80, 0);
//    set_idt_entry(0x81, interrupt_handle_0x81, 0);
//    set_idt_entry(0x82, interrupt_handle_0x82, 0);
//    set_idt_entry(0x83, interrupt_handle_0x83, 0);
//    set_idt_entry(0x84, interrupt_handle_0x84, 0);
//    set_idt_entry(0x85, interrupt_handle_0x85, 0);
//    set_idt_entry(0x86, interrupt_handle_0x86, 0);
//    set_idt_entry(0x87, interrupt_handle_0x87, 0);
//    set_idt_entry(0x88, interrupt_handle_0x88, 0);
//    set_idt_entry(0x89, interrupt_handle_0x89, 0);
//    set_idt_entry(0x8a, interrupt_handle_0x8a, 0);
//    set_idt_entry(0x8b, interrupt_handle_0x8b, 0);
//    set_idt_entry(0x8c, interrupt_handle_0x8c, 0);
//    set_idt_entry(0x8d, interrupt_handle_0x8d, 0);
//    set_idt_entry(0x8e, interrupt_handle_0x8e, 0);
//    set_idt_entry(0x8f, interrupt_handle_0x8f, 0);
//    set_idt_entry(0x90, interrupt_handle_0x90, 0);
//    set_idt_entry(0x91, interrupt_handle_0x91, 0);
//    set_idt_entry(0x92, interrupt_handle_0x92, 0);
//    set_idt_entry(0x93, interrupt_handle_0x93, 0);
//    set_idt_entry(0x94, interrupt_handle_0x94, 0);
//    set_idt_entry(0x95, interrupt_handle_0x95, 0);
//    set_idt_entry(0x96, interrupt_handle_0x96, 0);
//    set_idt_entry(0x97, interrupt_handle_0x97, 0);
//    set_idt_entry(0x98, interrupt_handle_0x98, 0);
//    set_idt_entry(0x99, interrupt_handle_0x99, 0);
//    set_idt_entry(0x9a, interrupt_handle_0x9a, 0);
//    set_idt_entry(0x9b, interrupt_handle_0x9b, 0);
//    set_idt_entry(0x9c, interrupt_handle_0x9c, 0);
//    set_idt_entry(0x9d, interrupt_handle_0x9d, 0);
//    set_idt_entry(0x9e, interrupt_handle_0x9e, 0);
//    set_idt_entry(0x9f, interrupt_handle_0x9f, 0);
//    set_idt_entry(0xa0, interrupt_handle_0xa0, 0);
//    set_idt_entry(0xa1, interrupt_handle_0xa1, 0);
//    set_idt_entry(0xa2, interrupt_handle_0xa2, 0);
//    set_idt_entry(0xa3, interrupt_handle_0xa3, 0);
//    set_idt_entry(0xa4, interrupt_handle_0xa4, 0);
//    set_idt_entry(0xa5, interrupt_handle_0xa5, 0);
//    set_idt_entry(0xa6, interrupt_handle_0xa6, 0);
//    set_idt_entry(0xa7, interrupt_handle_0xa7, 0);
//    set_idt_entry(0xa8, interrupt_handle_0xa8, 0);
//    set_idt_entry(0xa9, interrupt_handle_0xa9, 0);
//    set_idt_entry(0xaa, interrupt_handle_0xaa, 0);
//    set_idt_entry(0xab, interrupt_handle_0xab, 0);
//    set_idt_entry(0xac, interrupt_handle_0xac, 0);
//    set_idt_entry(0xad, interrupt_handle_0xad, 0);
//    set_idt_entry(0xae, interrupt_handle_0xae, 0);
//    set_idt_entry(0xaf, interrupt_handle_0xaf, 0);
//    set_idt_entry(0xb0, interrupt_handle_0xb0, 0);
//    set_idt_entry(0xb1, interrupt_handle_0xb1, 0);
//    set_idt_entry(0xb2, interrupt_handle_0xb2, 0);
//    set_idt_entry(0xb3, interrupt_handle_0xb3, 0);
//    set_idt_entry(0xb4, interrupt_handle_0xb4, 0);
//    set_idt_entry(0xb5, interrupt_handle_0xb5, 0);
//    set_idt_entry(0xb6, interrupt_handle_0xb6, 0);
//    set_idt_entry(0xb7, interrupt_handle_0xb7, 0);
//    set_idt_entry(0xb8, interrupt_handle_0xb8, 0);
//    set_idt_entry(0xb9, interrupt_handle_0xb9, 0);
//    set_idt_entry(0xba, interrupt_handle_0xba, 0);
//    set_idt_entry(0xbb, interrupt_handle_0xbb, 0);
//    set_idt_entry(0xbc, interrupt_handle_0xbc, 0);
//    set_idt_entry(0xbd, interrupt_handle_0xbd, 0);
//    set_idt_entry(0xbe, interrupt_handle_0xbe, 0);
//    set_idt_entry(0xbf, interrupt_handle_0xbf, 0);
//    set_idt_entry(0xc0, interrupt_handle_0xc0, 0);
//    set_idt_entry(0xc1, interrupt_handle_0xc1, 0);
//    set_idt_entry(0xc2, interrupt_handle_0xc2, 0);
//    set_idt_entry(0xc3, interrupt_handle_0xc3, 0);
//    set_idt_entry(0xc4, interrupt_handle_0xc4, 0);
//    set_idt_entry(0xc5, interrupt_handle_0xc5, 0);
//    set_idt_entry(0xc6, interrupt_handle_0xc6, 0);
//    set_idt_entry(0xc7, interrupt_handle_0xc7, 0);
//    set_idt_entry(0xc8, interrupt_handle_0xc8, 0);
//    set_idt_entry(0xc9, interrupt_handle_0xc9, 0);
//    set_idt_entry(0xca, interrupt_handle_0xca, 0);
//    set_idt_entry(0xcb, interrupt_handle_0xcb, 0);
//    set_idt_entry(0xcc, interrupt_handle_0xcc, 0);
//    set_idt_entry(0xcd, interrupt_handle_0xcd, 0);
//    set_idt_entry(0xce, interrupt_handle_0xce, 0);
//    set_idt_entry(0xcf, interrupt_handle_0xcf, 0);
//    set_idt_entry(0xd0, interrupt_handle_0xd0, 0);
//    set_idt_entry(0xd1, interrupt_handle_0xd1, 0);
//    set_idt_entry(0xd2, interrupt_handle_0xd2, 0);
//    set_idt_entry(0xd3, interrupt_handle_0xd3, 0);
//    set_idt_entry(0xd4, interrupt_handle_0xd4, 0);
//    set_idt_entry(0xd5, interrupt_handle_0xd5, 0);
//    set_idt_entry(0xd6, interrupt_handle_0xd6, 0);
//    set_idt_entry(0xd7, interrupt_handle_0xd7, 0);
//    set_idt_entry(0xd8, interrupt_handle_0xd8, 0);
//    set_idt_entry(0xd9, interrupt_handle_0xd9, 0);
//    set_idt_entry(0xda, interrupt_handle_0xda, 0);
//    set_idt_entry(0xdb, interrupt_handle_0xdb, 0);
//    set_idt_entry(0xdc, interrupt_handle_0xdc, 0);
//    set_idt_entry(0xdd, interrupt_handle_0xdd, 0);
//    set_idt_entry(0xde, interrupt_handle_0xde, 0);
//    set_idt_entry(0xdf, interrupt_handle_0xdf, 0);
//    set_idt_entry(0xe0, interrupt_handle_0xe0, 0);
//    set_idt_entry(0xe1, interrupt_handle_0xe1, 0);
//    set_idt_entry(0xe2, interrupt_handle_0xe2, 0);
//    set_idt_entry(0xe3, interrupt_handle_0xe3, 0);
//    set_idt_entry(0xe4, interrupt_handle_0xe4, 0);
//    set_idt_entry(0xe5, interrupt_handle_0xe5, 0);
//    set_idt_entry(0xe6, interrupt_handle_0xe6, 0);
//    set_idt_entry(0xe7, interrupt_handle_0xe7, 0);
//    set_idt_entry(0xe8, interrupt_handle_0xe8, 0);
//    set_idt_entry(0xe9, interrupt_handle_0xe9, 0);
//    set_idt_entry(0xea, interrupt_handle_0xea, 0);
//    set_idt_entry(0xeb, interrupt_handle_0xeb, 0);
//    set_idt_entry(0xec, interrupt_handle_0xec, 0);
//    set_idt_entry(0xed, interrupt_handle_0xed, 0);
//    set_idt_entry(0xee, interrupt_handle_0xee, 0);
//    set_idt_entry(0xef, interrupt_handle_0xef, 0);
//    set_idt_entry(0xf0, interrupt_handle_0xf0, 0);
//    set_idt_entry(0xf1, interrupt_handle_0xf1, 0);
//    set_idt_entry(0xf2, interrupt_handle_0xf2, 0);
//    set_idt_entry(0xf3, interrupt_handle_0xf3, 0);
//    set_idt_entry(0xf4, interrupt_handle_0xf4, 0);
//    set_idt_entry(0xf5, interrupt_handle_0xf5, 0);
//    set_idt_entry(0xf6, interrupt_handle_0xf6, 0);
//    set_idt_entry(0xf7, interrupt_handle_0xf7, 0);
//    set_idt_entry(0xf8, interrupt_handle_0xf8, 0);
//    set_idt_entry(0xf9, interrupt_handle_0xf9, 0);
//    set_idt_entry(0xfa, interrupt_handle_0xfa, 0);
//    set_idt_entry(0xfb, interrupt_handle_0xfb, 0);
//    set_idt_entry(0xfc, interrupt_handle_0xfc, 0);
//    set_idt_entry(0xfd, interrupt_handle_0xfd, 0);
//    set_idt_entry(0xfe, interrupt_handle_0xfe, 0);
//    set_idt_entry(0xff, interrupt_handle_0xff, 0);
#endif

    // TODO: put as __lidt
    asm volatile ("lidt %0" : : "m" (g_idt));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Context switching
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: fpu context

void init_context(system_context_t* target, bool kernel) {
    *target = (system_context_t) {
        .ds = kernel ? GDT_KERNEL_DATA : GDT_USER_DATA,
        .ss = kernel ? GDT_KERNEL_DATA : GDT_USER_DATA,
        .cs = kernel ? GDT_KERNEL_CODE : GDT_USER_CODE,
        .rflags = (IA32_RFLAGS) {
            .always_one = 1,
            .IF = 1,
            .ID = 1,
        }
    };
}

// TODO: this
void save_context(system_context_t* curr) {
//    g_current_thread->system_context = *curr;
}

void restore_context(system_context_t* curr) {
//    *curr = g_current_thread->system_context;
//    set_address_space(&g_current_process->address_space);
}