#ifndef TOMATKERNEL_ISR_H
#define TOMATKERNEL_ISR_H

#define ISR_DIVIDE_BY_ZERO                0
#define ISR_DEBUG                         1
#define ISR_NON_MASKABLE_INTERRUPT        2
#define ISR_BREAKPOINT                    3
#define ISR_OVERFLOW                      4
#define ISR_BOUND_RANGE_EXCEEDED          5
#define ISR_INVALID_OPCODE                6
#define ISR_DEVICE_NOT_AVAILABLE          7
#define ISR_DOUBLE_FAULT                  8
#define ISR_COPROCESSOR_SEGMENT_OVERRUN   9
#define ISR_INVALID_TSS                   10
#define ISR_SEGMENT_NOT_PRESENTED         11
#define ISR_STACK_SEGMENT_FAULT           12
#define ISR_GENERAL_PROTECTION_FAULT      13
#define ISR_PAGE_FAULT                    14
// reserved
#define ISR_X87_FLOATING_POINT_EXCEPTION  16
#define ISR_ALIGNMENT_CHECK               17
#define ISR_MACHINE_CHECK                 18
#define ISR_SIMD_FLOATING_POINT_EXCEPTION 19
#define ISR_VIRTUALIZATION_EXCEPTION      20
// reserved
#define ISR_SECURITY_EXCEPTION            30


/**
 * Will initialize the exception interrupts
 *
 * Note: This will be called by the idt_init function, do not call this manually
 */
void isr_init();

///-------------------------------------------------------------------------
// Exception Interrupt stubs, these will call the common exception handler
///-------------------------------------------------------------------------

// exceptions without error codes
extern void isr_divide_by_zero();
extern void isr_debug();
extern void isr_non_maskable_interrupt();
extern void isr_breakpoint();
extern void isr_overflow();
extern void isr_bound_range_exceeded();
extern void isr_invalid_opcode();
extern void isr_device_not_available();
extern void isr_x87_floating_point_exception();
extern void isr_machine_check();
extern void isr_simd_floating_point_exception();
extern void isr_virtualization_exception();

// exceptions with error codes
extern void isr_double_fault();
extern void isr_invalid_tss();
extern void isr_segment_not_presented();
extern void isr_stack_segment_fault();
extern void isr_general_protection_fault();
extern void isr_page_fault();
extern void isr_alignment_check();
extern void isr_security_exception();

#endif //TOMATKERNEL_ISR_H
