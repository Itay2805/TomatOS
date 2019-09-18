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

    ; only need to modify the ds and es segments
    mov rax, ds
    push rax
    mov ax, GDT_KERNEL_DATA
    mov ds, ax
    mov es, ax

    call common_syscall_handler

    pop rax
    mov ds, ax
    mov es, ax

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

    ; restore the stack
    pop rsp
    sysret
