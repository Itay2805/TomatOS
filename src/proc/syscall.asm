
EXTERN g_kernel_stack
EXTERN g_saved_stack

EXTERN syscall_common_handler

GLOBAL syscall_entry
syscall_entry:
    ; set the rsp
    mov [gs:g_saved_stack], rsp
    mov rsp, [gs:g_kernel_stack]

    ; push sys_num
    push rax

    ; push rflags
    push r11
    ; push rip
    push rcx

    ; push the return error code
    push rbx

    ; push arg0-arg6
    push rdi
    push rsi
    push rdx
    push r10
    push r8
    push r9

    ; push saved registers
    push r12
    push r13
    push r14
    push r15
    push rbp

    call syscall_common_handler

    ; pop saved registers
    pop rbp
    pop rbx
    pop r15
    pop r14
    pop r13
    pop r12

    ; pop args
    pop r9
    pop r8
    pop r10
    pop rdx
    pop rsi
    pop rdi

    ; pop rip and rflags
    pop rcx
    pop r11

    ; pop return value
    pop rax

    ; restore stack
    mov rsp, [gs:g_saved_stack]

    ; return from syscall
    o64 sysret
