BITS 64

; GDT stuff
GDT_KERNEL_CODE equ 8
GDT_KERNEL_DATA equ 16
GDT_USER_DATA equ (24 | 3)
GDT_USER_CODE equ (32 | 3)

TEMP_STACK_STORE equ 8
SYSCALL_STACK equ 0

EXTERN common_syscall_handler

GLOBAL handle_syscall_request
handle_syscall_request:
    ; switch the stacks
    cli
    mov qword [gs:TEMP_STACK_STORE], rsp
    mov rsp, qword [gs:SYSCALL_STACK]
    push qword [gs:TEMP_STACK_STORE]
    sti

    ; TODO: push all registers

    call common_syscall_handler

    ; TODO: pop all registers

    ; restore the stack
    pop rsp
    sysret
