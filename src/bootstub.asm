BITS 64
SECTION .tboot.text

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Small stub to set up a quick n dirty higher half
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
EXTERN kernel_main
EXTERN gdt
EXTERN gdt_fix_segments
GLOBAL tboot_main
tboot_main:
    ;
    ; Note: This assumes there is less than 512GB, if there is
    ;       more then this might break stuff :shrug:
    ;

    ; just in case
    cld
    cli

	; disable the wp bit
	mov rax, cr0
	and rax, ~(1 << 16)
	mov cr0, rax

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

    ; wait for it...
    mfence

	; load the gdt and fix the segments
	lgdt [gdt]
	call gdt_fix_segments

	; setup proper stack
	mov rsp, tmp_kernel_stack
	mov rbp, 0

	; we can now call the actual higher half main
	jmp kernel_main

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Temp kernel stack, 512kb
; this will be used until we do more advanced
; per-cpu-initialization and kick start
; the scheduler
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SECTION .bss
resb 1024 * 512
tmp_kernel_stack:

