BITS 64
SECTION .tboot.text

;
; Small stub to set up a quick n dirty higher half
;

EXTERN kernel_main
GLOBAL tboot_main
tboot_main:
    ;
    ; Note: This assumes there is less than 512GB, if there is
    ;       more then this might break stuff :shrug:
    ;

    ; get the cr3
    mov rax, cr3

    ; make sure we only have the address
    mov rbx, 0xFFFFFFFFFFFFF000
    and rax, rbx

    ; copy first pml4e to last pml4e
    mov rbx, [rax]
    mov [rax + 511 * 8], rbx

    ; copy the first pml3e to the
    ; last pml3e of the first pml4e
    mov rax, 0x000FFFFFFFFFF000
    and rbx, rax
    mov rax, [rbx]
    mov [rbx + 511 * 8], rax

    ; reload, just in case
    mov rax, cr3
    mov cr3, rax

    ; we can now call the actual higher half main
    jmp kernel_main