BITS 64

EXTERN isr_common

GLOBAL isr_divide_by_zero
GLOBAL isr_debug
GLOBAL isr_non_maskable_interrupt
GLOBAL isr_breakpoint
GLOBAL isr_overflow
GLOBAL isr_bound_range_exceeded
GLOBAL isr_invalid_opcode
GLOBAL isr_device_not_available
GLOBAL isr_x87_floating_point_exception
GLOBAL isr_machine_check
GLOBAL isr_simd_floating_point_exception
GLOBAL isr_virtualization_exception

GLOBAL isr_double_fault
GLOBAL isr_invalid_tss
GLOBAL isr_segment_not_presented
GLOBAL isr_stack_segment_fault
GLOBAL isr_general_protection_fault
GLOBAL isr_page_fault
GLOBAL isr_alignment_check
GLOBAL isr_security_exception

isr_common_stub:
     push rax
     push rbx
     push rcx
     push rdx
     push rsi
     push rdi
     push rbp
     push r8
     push r9
     push r10
     push r11
     push r12
     push r13
     push r14
     push r15

     ; store ds
     mov rax, ds
     push rax
     mov ax, 16
     mov ds, ax
     mov es, ax
     mov gs, ax
     mov fs, ax
     mov ss, ax

    call isr_common

    ; restore ds
    pop rax
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; ignore the int and error code
    add rsp, 16

    iretq


isr_divide_by_zero:
    push qword 0
    push qword 0
    jmp isr_common_stub

isr_debug:
    push qword 0
    push qword 1
    jmp isr_common_stub

isr_non_maskable_interrupt:
    push qword 0
    push qword 2
    jmp isr_common_stub

isr_breakpoint:
    push qword 0
    push qword 3
    jmp isr_common_stub

isr_overflow:
    push qword 0
    push qword 4
    jmp isr_common_stub

isr_bound_range_exceeded:
    push qword 0
    push qword 5
    jmp isr_common_stub

isr_invalid_opcode:
    push qword 0
    push qword 6
    jmp isr_common_stub

isr_device_not_available:
    push qword 0
    push qword 7
    jmp isr_common_stub

isr_x87_floating_point_exception:
    push qword 0
    push qword 16
    jmp isr_common_stub

isr_machine_check:
    push qword 0
    push qword 18
    jmp isr_common_stub

isr_simd_floating_point_exception:
    push qword 0
    push qword 19
    jmp isr_common_stub

isr_virtualization_exception:
    push qword 0
    push qword 20
    jmp isr_common_stub

isr_double_fault:
    push qword 8
    jmp isr_common_stub

isr_invalid_tss:
    push qword 10
    jmp isr_common_stub

isr_segment_not_presented:
    push qword 11
    jmp isr_common_stub

isr_stack_segment_fault:
    push qword 12
    jmp isr_common_stub

isr_general_protection_fault:
    push qword 13
    jmp isr_common_stub

isr_page_fault:
    push qword 14
    jmp isr_common_stub

isr_alignment_check:
    push qword 17
    jmp isr_common_stub

isr_security_exception:
    push qword 30
    jmp isr_common_stub
