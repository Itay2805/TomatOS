BITS 32
DEFAULT rel

; ------------------------------------
; Multiboot Header values
; ------------------------------------
MAGIC equ 0x1badb002
FLAGS equ ((1 << 0) | (1 << 1))
CHECKSUM equ -(MAGIC + FLAGS)

; (1 << 2)

; ------------------------------------
; Actual Multiboot header
; ------------------------------------
SECTION .multiboot.header
ALIGN 8
multiboot_header:
dd MAGIC
dd FLAGS
dd CHECKSUM

; these are the address fields, we just ignore these
dd 0 ; header_addr
dd 0 ; load_addr
dd 0 ; load_end_addr
dd 0 ; bss_end_addr
dd 0 ; entry_addr

; these are the graphics fields
dd 0    ; video mode
dd 1024 ; width
dd 768  ; height
dd 32   ; depth (bpp)

; ====================================
;   Multiboot entry
; ====================================

KERNEL_VIRTUAL_ADDRESS equ 0xFFFFFFFFC0000000
PML4_KERNEL_OFFSET equ ((KERNEL_VIRTUAL_ADDRESS >> 39) & 0x1FF)
PDPE_KERNEL_OFFSET equ ((KERNEL_VIRTUAL_ADDRESS >> 30) & 0x1FF)

SECTION .multiboot.text

GLOBAL multiboot_main
multiboot_main:

    ; TODO: Check that we were loaded from multiboot

    ; TODO: Check we have PSE & long mode support

    ; set the first pml4 entry (mapping first gb to the last gb in virtual space)
    ; sets the address of the pdpe
    ; and then set some flags

    ; Lower half (1GB to keep the multiboot code running)
    ; pml4
    mov eax, boot_pdpe_lower_half
    or eax, 0x3 ; PRESENT | READ_WRITE
    mov [boot_pml4], eax
    ; pdpe (Physical is first GB)
    mov dword [boot_pdpe_lower_half], 0x83 ; PRESENT | READ_WRITE | PAGE_SIZE

    ; Higher half (where the kernel will stay)
    ; pml4
    mov eax, boot_pdpe_higher_half
    or eax, 0x3 ; PRESENT | READ_WRITE
    mov [boot_pml4 + PML4_KERNEL_OFFSET * 8], eax
    ; pdpe (Physical is first GB)
	mov dword [boot_pdpe_higher_half + PDPE_KERNEL_OFFSET * 8], 0x83 ; PRESENT | READ_WRITE | PAGE_SIZE

    ; set the pml4
    mov eax, boot_pml4
    mov cr3, eax

    ; Enable PAE-paging
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Enter long mode compatability mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; Jump to the kernel using real long mode (64bit)
    lgdt [gdt64]

    mov ax, 16
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 8:multiboot_64bit

BITS 64
    extern kernel_main
    multiboot_64bit:
        mov rsp, kernel_stack
        mov rbp, kernel_stack
        mov rdi, rbx

        call kernel_main

        ; halt if we reach here
        cli
        hlt

; ====================================
;   64Bit GDT
; ====================================
SECTION .multiboot.data

; --------------------------------
; The GDT64 table
; --------------------------------

gdt64_start:

    ; --------------------------------
    ; NULL entry
    ; --------------------------------
    dw 0x0000
    dw 0x0000
    db 0x00
    db 00000000b
    db 00000000b
    db 0x00

    ; --------------------------------
    ; Kernel code segment (8)
    ; --------------------------------
    dw 0x0000
    dw 0x0000
    db 0x00
    db 10011010b
    db 00100000b
    db 0x00

    ; --------------------------------
    ; Kernel data segment (16)
    ; --------------------------------
    dw 0x0000
    dw 0x0000
    db 0x00
    db 10010010b
    db 00000000b
    db 0x00

gdt64_end:

; --------------------------------
; The actual pointer
; --------------------------------
GLOBAL gdt64
gdt64:
    dw gdt64_end - gdt64_start - 1
    dq gdt64_start

; ====================================
;   Paging (1GB identity mapping)
; ====================================
SECTION .multiboot.bss nobits

ALIGNB 4096
GLOBAL boot_pml4
boot_pml4:
resq 512

; Lower half
ALIGNB 4096
GLOBAL boot_pdpe_lower_half
boot_pdpe_lower_half:
RESQ 512

; Higher half
ALIGNB 4096
GLOBAL boot_pdpe_higher_half
boot_pdpe_higher_half:
RESQ 512

; ====================================
;   Stack
; ====================================
SECTION .bss
GLOBAL kernel_stack

RESB 2 * 1024 * 1024
kernel_stack: