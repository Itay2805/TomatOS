BITS 64

EXTERN syscall_handler
EXTERN kernel_stack

GLOBAL syscall_handler_stub
syscall_handler_stub:
    ; save the return and rflags
    push rcx
    push r11

    ; save the user stack
    push rsp
    push rbp

    ; set the syscall to use the kernel stack
    mov rsp, kernel_stack
    mov rbp, kernel_stack

    ; call the actual handler
    call syscall_handler

    ; restore the user stack
    pop rbp
    pop rsp

    ; restore the rip and rflags
    pop r11
    pop rcx

    sysret