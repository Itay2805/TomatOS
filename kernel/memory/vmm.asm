BITS 64
SECTION .text

; uint64_t vmm_get();
GLOBAL vmm_get
vmm_get:
    push rbp
    mov rbp, rsp

    mov rax, cr3

    mov rsp, rbp
    pop rbp
    ret

; void vmm_get(uint64_t);
GLOBAL vmm_set
vmm_set:
    push rbp
    mov rbp, rsp

    mov rax, rdi
    mov cr3, rax

    mov rsp, rbp
    pop rbp
    ret
