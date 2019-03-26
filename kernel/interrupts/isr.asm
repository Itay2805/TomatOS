BITS 64

GLOBAL get_cr2
get_cr2:
    mov rax, cr2
    ret

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

GLOBAL isr_common
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
     xor rax, rax
     mov ax, ds
     push rax
     mov ax, 16
     mov ds, ax
     mov es, ax
     mov fs, ax
     mov gs, ax

    call isr_common

    ; restore ds
    pop rax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

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

    sti
    iretq


isr_divide_by_zero:
    cli
    push qword 0
    push qword 0
    jmp isr_common_stub

isr_debug:
    cli
    push qword 0
    push qword 1
    jmp isr_common_stub

isr_non_maskable_interrupt:
    cli
    push qword 0
    push qword 2
    jmp isr_common_stub

isr_breakpoint:
    cli
    push qword 0
    push qword 3
    jmp isr_common_stub

isr_overflow:
    cli
    push qword 0
    push qword 4
    jmp isr_common_stub

isr_bound_range_exceeded:
    cli
    push qword 0
    push qword 5
    jmp isr_common_stub

isr_invalid_opcode:
    cli
    push qword 0
    push qword 6
    jmp isr_common_stub

isr_device_not_available:
    cli
    push qword 0
    push qword 7
    jmp isr_common_stub

isr_x87_floating_point_exception:
    cli
    push qword 0
    push qword 16
    jmp isr_common_stub

isr_machine_check:
    cli
    push qword 0
    push qword 18
    jmp isr_common_stub

isr_simd_floating_point_exception:
    cli
    push qword 0
    push qword 19
    jmp isr_common_stub

isr_virtualization_exception:
    cli
    push qword 0
    push qword 20
    jmp isr_common_stub

isr_double_fault:
    cli
    push qword 8
    jmp isr_common_stub

isr_invalid_tss:
    cli
    push qword 10
    jmp isr_common_stub

isr_segment_not_presented:
    cli
    push qword 11
    jmp isr_common_stub

isr_stack_segment_fault:
    cli
    push qword 12
    jmp isr_common_stub

isr_general_protection_fault:
    cli
    push qword 13
    jmp isr_common_stub

isr_page_fault:
    cli
    push qword 14
    jmp isr_common_stub

isr_alignment_check:
    cli
    push qword 17
    jmp isr_common_stub

isr_security_exception:
    cli
    push qword 30
    jmp isr_common_stub