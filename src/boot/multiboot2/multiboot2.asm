BITS 32
DEFAULT rel

; ------------------------------------
; Multiboot Header values
; ------------------------------------
MAGIC equ 0xE85250D6
ARCHITECTURE equ 0
HEADER_LENGTH equ multiboot_header_end - multiboot_header
CHECKSUM equ -(MAGIC + ARCHITECTURE + HEADER_LENGTH)

; ------------------------------------
; Actual Multiboot header
; ------------------------------------
SECTION .multiboot.header
ALIGN 8
multiboot_header:
dd MAGIC
dd ARCHITECTURE
dd HEADER_LENGTH
dd CHECKSUM

; ------------------------------------
; Information request
; ------------------------------------
ALIGN 8
.inf_req_start:
dw 1                                ; type
dw 0                                ; flags
dd .inf_req_end - .inf_req_start    ; size

; tags we want
dd 4 ; basic memory
dd 6 ; memory map
dd 8 ; framebuffer info
dd 14 ; old RSDP
dd 15 ; new RSDP
.inf_req_end:

; ------------------------------------
; Console flags
; ------------------------------------
ALIGN 8
.console_flags:
dw 4                        ; type
dw 0                        ; flags
dd 12                       ; size
dd ((1 << 0) | (1 << 1))    ; console_flags

; ------------------------------------
; Framebuffer tag
; ------------------------------------
ALIGN 8
.framebuffer_tag:
dw 5    ; type
dw 0    ; flags
dd 20   ; size
dd 0    ; width (no preference)
dd 0    ; height (no preference)
dd 32   ; bpp (we only support 32bpp)

; ------------------------------------
; Termination tag
; ------------------------------------
ALIGN 8
.termination_tag:
dw 0    ; type
dw 0    ; flags
dd 8    ; size

multiboot_header_end:

; ====================================
;   Multiboot entry
; ====================================

KERNEL_VIRTUAL_ADDRESS equ 0xFFFFFFFFC0000000
PML4_KERNEL_OFFSET equ ((KERNEL_VIRTUAL_ADDRESS >> 39) & 0x1FF)
PDPE_KERNEL_OFFSET equ ((KERNEL_VIRTUAL_ADDRESS >> 30) & 0x1FF)

SECTION .multiboot.text

GLOBAL multiboot_entry
multiboot_entry:

; Make sure this is a good multiboot 2
.check:
    cmp eax, 0x36d76289
    je .continue
.continue:
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
    extern multiboot_main
    multiboot_64bit:
        mov rsp, kernel_stack
        mov rbp, kernel_stack
        mov rdi, rbx

        call multiboot_main

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