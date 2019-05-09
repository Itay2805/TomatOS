
GLOBAL dispatch_resource_call_trampoline
dispatch_resource_call_trampoline:
    ; TODO: Move this to thread local storage that only the kernel can access
    push rdi ; process_t*
    push rsi ; tid

    ; does a call to rax, but makes so we can clear out rax
    call rax

    ; get the process and tid
    pop rsi
    pop rdi
    mov rdx, rax

    ; call SYSCALL_PROVIDER_HANDLER_FINISHED
    mov rax, 11
    int 0x80

    .sleep:
        jmp .sleep