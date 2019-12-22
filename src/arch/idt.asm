BITS 64

; GDT stuff
GDT_KERNEL_CODE equ 8
GDT_KERNEL_DATA equ 16
GDT_USER_DATA equ (24 | 3)
GDT_USER_CODE equ (32 | 3)

; the common handlers
EXTERN common_interrupt_handler

; export all the stubs
GLOBAL handle_interrupt_request_00
GLOBAL handle_interrupt_request_01
GLOBAL handle_interrupt_request_02
GLOBAL handle_interrupt_request_03
GLOBAL handle_interrupt_request_04
GLOBAL handle_interrupt_request_05
GLOBAL handle_interrupt_request_06
GLOBAL handle_interrupt_request_07
GLOBAL handle_interrupt_request_08
GLOBAL handle_interrupt_request_09
GLOBAL handle_interrupt_request_0a
GLOBAL handle_interrupt_request_0b
GLOBAL handle_interrupt_request_0c
GLOBAL handle_interrupt_request_0d
GLOBAL handle_interrupt_request_0e
GLOBAL handle_interrupt_request_0f
GLOBAL handle_interrupt_request_10
GLOBAL handle_interrupt_request_11
GLOBAL handle_interrupt_request_12
GLOBAL handle_interrupt_request_13
GLOBAL handle_interrupt_request_14
GLOBAL handle_interrupt_request_15
GLOBAL handle_interrupt_request_16
GLOBAL handle_interrupt_request_17
GLOBAL handle_interrupt_request_18
GLOBAL handle_interrupt_request_19
GLOBAL handle_interrupt_request_1a
GLOBAL handle_interrupt_request_1b
GLOBAL handle_interrupt_request_1c
GLOBAL handle_interrupt_request_1d
GLOBAL handle_interrupt_request_1e
GLOBAL handle_interrupt_request_1f
GLOBAL handle_interrupt_request_20
GLOBAL handle_interrupt_request_21
GLOBAL handle_interrupt_request_22
GLOBAL handle_interrupt_request_23
GLOBAL handle_interrupt_request_24
GLOBAL handle_interrupt_request_25
GLOBAL handle_interrupt_request_26
GLOBAL handle_interrupt_request_27
GLOBAL handle_interrupt_request_28
GLOBAL handle_interrupt_request_29
GLOBAL handle_interrupt_request_2a
GLOBAL handle_interrupt_request_2b
GLOBAL handle_interrupt_request_2c
GLOBAL handle_interrupt_request_2d
GLOBAL handle_interrupt_request_2e
GLOBAL handle_interrupt_request_2f
GLOBAL handle_interrupt_request_30
GLOBAL handle_interrupt_request_31
GLOBAL handle_interrupt_request_32
GLOBAL handle_interrupt_request_33
GLOBAL handle_interrupt_request_34
GLOBAL handle_interrupt_request_35
GLOBAL handle_interrupt_request_36
GLOBAL handle_interrupt_request_37
GLOBAL handle_interrupt_request_38
GLOBAL handle_interrupt_request_39
GLOBAL handle_interrupt_request_3a
GLOBAL handle_interrupt_request_3b
GLOBAL handle_interrupt_request_3c
GLOBAL handle_interrupt_request_3d
GLOBAL handle_interrupt_request_3e
GLOBAL handle_interrupt_request_3f
GLOBAL handle_interrupt_request_40
GLOBAL handle_interrupt_request_41
GLOBAL handle_interrupt_request_42
GLOBAL handle_interrupt_request_43
GLOBAL handle_interrupt_request_44
GLOBAL handle_interrupt_request_45
GLOBAL handle_interrupt_request_46
GLOBAL handle_interrupt_request_47
GLOBAL handle_interrupt_request_48
GLOBAL handle_interrupt_request_49
GLOBAL handle_interrupt_request_4a
GLOBAL handle_interrupt_request_4b
GLOBAL handle_interrupt_request_4c
GLOBAL handle_interrupt_request_4d
GLOBAL handle_interrupt_request_4e
GLOBAL handle_interrupt_request_4f
GLOBAL handle_interrupt_request_50
GLOBAL handle_interrupt_request_51
GLOBAL handle_interrupt_request_52
GLOBAL handle_interrupt_request_53
GLOBAL handle_interrupt_request_54
GLOBAL handle_interrupt_request_55
GLOBAL handle_interrupt_request_56
GLOBAL handle_interrupt_request_57
GLOBAL handle_interrupt_request_58
GLOBAL handle_interrupt_request_59
GLOBAL handle_interrupt_request_5a
GLOBAL handle_interrupt_request_5b
GLOBAL handle_interrupt_request_5c
GLOBAL handle_interrupt_request_5d
GLOBAL handle_interrupt_request_5e
GLOBAL handle_interrupt_request_5f
GLOBAL handle_interrupt_request_60
GLOBAL handle_interrupt_request_61
GLOBAL handle_interrupt_request_62
GLOBAL handle_interrupt_request_63
GLOBAL handle_interrupt_request_64
GLOBAL handle_interrupt_request_65
GLOBAL handle_interrupt_request_66
GLOBAL handle_interrupt_request_67
GLOBAL handle_interrupt_request_68
GLOBAL handle_interrupt_request_69
GLOBAL handle_interrupt_request_6a
GLOBAL handle_interrupt_request_6b
GLOBAL handle_interrupt_request_6c
GLOBAL handle_interrupt_request_6d
GLOBAL handle_interrupt_request_6e
GLOBAL handle_interrupt_request_6f
GLOBAL handle_interrupt_request_70
GLOBAL handle_interrupt_request_71
GLOBAL handle_interrupt_request_72
GLOBAL handle_interrupt_request_73
GLOBAL handle_interrupt_request_74
GLOBAL handle_interrupt_request_75
GLOBAL handle_interrupt_request_76
GLOBAL handle_interrupt_request_77
GLOBAL handle_interrupt_request_78
GLOBAL handle_interrupt_request_79
GLOBAL handle_interrupt_request_7a
GLOBAL handle_interrupt_request_7b
GLOBAL handle_interrupt_request_7c
GLOBAL handle_interrupt_request_7d
GLOBAL handle_interrupt_request_7e
GLOBAL handle_interrupt_request_7f
GLOBAL handle_interrupt_request_80
GLOBAL handle_interrupt_request_81
GLOBAL handle_interrupt_request_82
GLOBAL handle_interrupt_request_83
GLOBAL handle_interrupt_request_84
GLOBAL handle_interrupt_request_85
GLOBAL handle_interrupt_request_86
GLOBAL handle_interrupt_request_87
GLOBAL handle_interrupt_request_88
GLOBAL handle_interrupt_request_89
GLOBAL handle_interrupt_request_8a
GLOBAL handle_interrupt_request_8b
GLOBAL handle_interrupt_request_8c
GLOBAL handle_interrupt_request_8d
GLOBAL handle_interrupt_request_8e
GLOBAL handle_interrupt_request_8f
GLOBAL handle_interrupt_request_90
GLOBAL handle_interrupt_request_91
GLOBAL handle_interrupt_request_92
GLOBAL handle_interrupt_request_93
GLOBAL handle_interrupt_request_94
GLOBAL handle_interrupt_request_95
GLOBAL handle_interrupt_request_96
GLOBAL handle_interrupt_request_97
GLOBAL handle_interrupt_request_98
GLOBAL handle_interrupt_request_99
GLOBAL handle_interrupt_request_9a
GLOBAL handle_interrupt_request_9b
GLOBAL handle_interrupt_request_9c
GLOBAL handle_interrupt_request_9d
GLOBAL handle_interrupt_request_9e
GLOBAL handle_interrupt_request_9f
GLOBAL handle_interrupt_request_a0
GLOBAL handle_interrupt_request_a1
GLOBAL handle_interrupt_request_a2
GLOBAL handle_interrupt_request_a3
GLOBAL handle_interrupt_request_a4
GLOBAL handle_interrupt_request_a5
GLOBAL handle_interrupt_request_a6
GLOBAL handle_interrupt_request_a7
GLOBAL handle_interrupt_request_a8
GLOBAL handle_interrupt_request_a9
GLOBAL handle_interrupt_request_aa
GLOBAL handle_interrupt_request_ab
GLOBAL handle_interrupt_request_ac
GLOBAL handle_interrupt_request_ad
GLOBAL handle_interrupt_request_ae
GLOBAL handle_interrupt_request_af
GLOBAL handle_interrupt_request_b0
GLOBAL handle_interrupt_request_b1
GLOBAL handle_interrupt_request_b2
GLOBAL handle_interrupt_request_b3
GLOBAL handle_interrupt_request_b4
GLOBAL handle_interrupt_request_b5
GLOBAL handle_interrupt_request_b6
GLOBAL handle_interrupt_request_b7
GLOBAL handle_interrupt_request_b8
GLOBAL handle_interrupt_request_b9
GLOBAL handle_interrupt_request_ba
GLOBAL handle_interrupt_request_bb
GLOBAL handle_interrupt_request_bc
GLOBAL handle_interrupt_request_bd
GLOBAL handle_interrupt_request_be
GLOBAL handle_interrupt_request_bf
GLOBAL handle_interrupt_request_c0
GLOBAL handle_interrupt_request_c1
GLOBAL handle_interrupt_request_c2
GLOBAL handle_interrupt_request_c3
GLOBAL handle_interrupt_request_c4
GLOBAL handle_interrupt_request_c5
GLOBAL handle_interrupt_request_c6
GLOBAL handle_interrupt_request_c7
GLOBAL handle_interrupt_request_c8
GLOBAL handle_interrupt_request_c9
GLOBAL handle_interrupt_request_ca
GLOBAL handle_interrupt_request_cb
GLOBAL handle_interrupt_request_cc
GLOBAL handle_interrupt_request_cd
GLOBAL handle_interrupt_request_ce
GLOBAL handle_interrupt_request_cf
GLOBAL handle_interrupt_request_d0
GLOBAL handle_interrupt_request_d1
GLOBAL handle_interrupt_request_d2
GLOBAL handle_interrupt_request_d3
GLOBAL handle_interrupt_request_d4
GLOBAL handle_interrupt_request_d5
GLOBAL handle_interrupt_request_d6
GLOBAL handle_interrupt_request_d7
GLOBAL handle_interrupt_request_d8
GLOBAL handle_interrupt_request_d9
GLOBAL handle_interrupt_request_da
GLOBAL handle_interrupt_request_db
GLOBAL handle_interrupt_request_dc
GLOBAL handle_interrupt_request_dd
GLOBAL handle_interrupt_request_de
GLOBAL handle_interrupt_request_df
GLOBAL handle_interrupt_request_e0
GLOBAL handle_interrupt_request_e1
GLOBAL handle_interrupt_request_e2
GLOBAL handle_interrupt_request_e3
GLOBAL handle_interrupt_request_e4
GLOBAL handle_interrupt_request_e5
GLOBAL handle_interrupt_request_e6
GLOBAL handle_interrupt_request_e7
GLOBAL handle_interrupt_request_e8
GLOBAL handle_interrupt_request_e9
GLOBAL handle_interrupt_request_ea
GLOBAL handle_interrupt_request_eb
GLOBAL handle_interrupt_request_ec
GLOBAL handle_interrupt_request_ed
GLOBAL handle_interrupt_request_ee
GLOBAL handle_interrupt_request_ef
GLOBAL handle_interrupt_request_f0
GLOBAL handle_interrupt_request_f1
GLOBAL handle_interrupt_request_f2
GLOBAL handle_interrupt_request_f3
GLOBAL handle_interrupt_request_f4
GLOBAL handle_interrupt_request_f5
GLOBAL handle_interrupt_request_f6
GLOBAL handle_interrupt_request_f7
GLOBAL handle_interrupt_request_f8
GLOBAL handle_interrupt_request_f9
GLOBAL handle_interrupt_request_fa
GLOBAL handle_interrupt_request_fb
GLOBAL handle_interrupt_request_fc
GLOBAL handle_interrupt_request_fd
GLOBAL handle_interrupt_request_fe
GLOBAL handle_interrupt_request_ff

common_stub:
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

    ; store ds
    mov rax, ds
    push rax
    mov ax, GDT_KERNEL_DATA
    mov ds, ax
    mov es, ax
    mov ss, ax

    call common_interrupt_handler

    ; restore ds
    pop rax
    mov ds, ax
    mov es, ax
    mov ss, ax

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

    add rsp, 8*2

    iretq

handle_interrupt_request_00:
    push qword 0
    push qword 0x0
    jmp common_stub
handle_interrupt_request_01:
    push qword 0
    push qword 0x1
    jmp common_stub
handle_interrupt_request_02:
    push qword 0
    push qword 0x2
    jmp common_stub
handle_interrupt_request_03:
    push qword 0
    push qword 0x3
    jmp common_stub
handle_interrupt_request_04:
    push qword 0
    push qword 0x4
    jmp common_stub
handle_interrupt_request_05:
    push qword 0
    push qword 0x5
    jmp common_stub
handle_interrupt_request_06:
    push qword 0
    push qword 0x6
    jmp common_stub
handle_interrupt_request_07:
    push qword 0
    push qword 0x7
    jmp common_stub
handle_interrupt_request_08:
    ; error pushed by exception
    push qword 0x8
    jmp common_stub
handle_interrupt_request_09:
    push qword 0
    push qword 0x9
    jmp common_stub
handle_interrupt_request_0a:
    ; error pushed by exception
    push qword 0xa
    jmp common_stub
handle_interrupt_request_0b:
    ; error pushed by exception
    push qword 0xb
    jmp common_stub
handle_interrupt_request_0c:
    ; error pushed by exception
    push qword 0xc
    jmp common_stub
handle_interrupt_request_0d:
    ; error pushed by exception
    push qword 0xd
    jmp common_stub
handle_interrupt_request_0e:
    ; error pushed by exception
    push qword 0xe
    jmp common_stub
handle_interrupt_request_0f:
    push qword 0
    push qword 0xf
    jmp common_stub
handle_interrupt_request_10:
   ; error pushed by exception
    push qword 0x10
    jmp common_stub
handle_interrupt_request_11:
    push qword 0
    push qword 0x11
    jmp common_stub
handle_interrupt_request_12:
    push qword 0
    push qword 0x12
    jmp common_stub
handle_interrupt_request_13:
    push qword 0
    push qword 0x13
    jmp common_stub
handle_interrupt_request_14:
    push qword 0
    push qword 0x14
    jmp common_stub
handle_interrupt_request_15:
    push qword 0
    push qword 0x15
    jmp common_stub
handle_interrupt_request_16:
    push qword 0
    push qword 0x16
    jmp common_stub
handle_interrupt_request_17:
    push qword 0
    push qword 0x17
    jmp common_stub
handle_interrupt_request_18:
    push qword 0
    push qword 0x18
    jmp common_stub
handle_interrupt_request_19:
    push qword 0
    push qword 0x19
    jmp common_stub
handle_interrupt_request_1a:
    push qword 0
    push qword 0x1a
    jmp common_stub
handle_interrupt_request_1b:
    push qword 0
    push qword 0x1b
    jmp common_stub
handle_interrupt_request_1c:
    push qword 0
    push qword 0x1c
    jmp common_stub
handle_interrupt_request_1d:
    push qword 0
    push qword 0x1d
    jmp common_stub
handle_interrupt_request_1e:
    ; error pushed by exception
    push qword 0x1e
    jmp common_stub
handle_interrupt_request_1f:
    push qword 0
    push qword 0x1f
    jmp common_stub
handle_interrupt_request_20:
    push qword 0
    push qword 0x20
    jmp common_stub
handle_interrupt_request_21:
    push qword 0
    push qword 0x21
    jmp common_stub
handle_interrupt_request_22:
    push qword 0
    push qword 0x22
    jmp common_stub
handle_interrupt_request_23:
    push qword 0
    push qword 0x23
    jmp common_stub
handle_interrupt_request_24:
    push qword 0
    push qword 0x24
    jmp common_stub
handle_interrupt_request_25:
    push qword 0
    push qword 0x25
    jmp common_stub
handle_interrupt_request_26:
    push qword 0
    push qword 0x26
    jmp common_stub
handle_interrupt_request_27:
    push qword 0
    push qword 0x27
    jmp common_stub
handle_interrupt_request_28:
    push qword 0
    push qword 0x28
    jmp common_stub
handle_interrupt_request_29:
    push qword 0
    push qword 0x29
    jmp common_stub
handle_interrupt_request_2a:
    push qword 0
    push qword 0x2a
    jmp common_stub
handle_interrupt_request_2b:
    push qword 0
    push qword 0x2b
    jmp common_stub
handle_interrupt_request_2c:
    push qword 0
    push qword 0x2c
    jmp common_stub
handle_interrupt_request_2d:
    push qword 0
    push qword 0x2d
    jmp common_stub
handle_interrupt_request_2e:
    push qword 0
    push qword 0x2e
    jmp common_stub
handle_interrupt_request_2f:
    push qword 0
    push qword 0x2f
    jmp common_stub
handle_interrupt_request_30:
    push qword 0
    push qword 0x30
    jmp common_stub
handle_interrupt_request_31:
    push qword 0
    push qword 0x31
    jmp common_stub
handle_interrupt_request_32:
    push qword 0
    push qword 0x32
    jmp common_stub
handle_interrupt_request_33:
    push qword 0
    push qword 0x33
    jmp common_stub
handle_interrupt_request_34:
    push qword 0
    push qword 0x34
    jmp common_stub
handle_interrupt_request_35:
    push qword 0
    push qword 0x35
    jmp common_stub
handle_interrupt_request_36:
    push qword 0
    push qword 0x36
    jmp common_stub
handle_interrupt_request_37:
    push qword 0
    push qword 0x37
    jmp common_stub
handle_interrupt_request_38:
    push qword 0
    push qword 0x38
    jmp common_stub
handle_interrupt_request_39:
    push qword 0
    push qword 0x39
    jmp common_stub
handle_interrupt_request_3a:
    push qword 0
    push qword 0x3a
    jmp common_stub
handle_interrupt_request_3b:
    push qword 0
    push qword 0x3b
    jmp common_stub
handle_interrupt_request_3c:
    push qword 0
    push qword 0x3c
    jmp common_stub
handle_interrupt_request_3d:
    push qword 0
    push qword 0x3d
    jmp common_stub
handle_interrupt_request_3e:
    push qword 0
    push qword 0x3e
    jmp common_stub
handle_interrupt_request_3f:
    push qword 0
    push qword 0x3f
    jmp common_stub
handle_interrupt_request_40:
    push qword 0
    push qword 0x40
    jmp common_stub
handle_interrupt_request_41:
    push qword 0
    push qword 0x41
    jmp common_stub
handle_interrupt_request_42:
    push qword 0
    push qword 0x42
    jmp common_stub
handle_interrupt_request_43:
    push qword 0
    push qword 0x43
    jmp common_stub
handle_interrupt_request_44:
    push qword 0
    push qword 0x44
    jmp common_stub
handle_interrupt_request_45:
    push qword 0
    push qword 0x45
    jmp common_stub
handle_interrupt_request_46:
    push qword 0
    push qword 0x46
    jmp common_stub
handle_interrupt_request_47:
    push qword 0
    push qword 0x47
    jmp common_stub
handle_interrupt_request_48:
    push qword 0
    push qword 0x48
    jmp common_stub
handle_interrupt_request_49:
    push qword 0
    push qword 0x49
    jmp common_stub
handle_interrupt_request_4a:
    push qword 0
    push qword 0x4a
    jmp common_stub
handle_interrupt_request_4b:
    push qword 0
    push qword 0x4b
    jmp common_stub
handle_interrupt_request_4c:
    push qword 0
    push qword 0x4c
    jmp common_stub
handle_interrupt_request_4d:
    push qword 0
    push qword 0x4d
    jmp common_stub
handle_interrupt_request_4e:
    push qword 0
    push qword 0x4e
    jmp common_stub
handle_interrupt_request_4f:
    push qword 0
    push qword 0x4f
    jmp common_stub
handle_interrupt_request_50:
    push qword 0
    push qword 0x50
    jmp common_stub
handle_interrupt_request_51:
    push qword 0
    push qword 0x51
    jmp common_stub
handle_interrupt_request_52:
    push qword 0
    push qword 0x52
    jmp common_stub
handle_interrupt_request_53:
    push qword 0
    push qword 0x53
    jmp common_stub
handle_interrupt_request_54:
    push qword 0
    push qword 0x54
    jmp common_stub
handle_interrupt_request_55:
    push qword 0
    push qword 0x55
    jmp common_stub
handle_interrupt_request_56:
    push qword 0
    push qword 0x56
    jmp common_stub
handle_interrupt_request_57:
    push qword 0
    push qword 0x57
    jmp common_stub
handle_interrupt_request_58:
    push qword 0
    push qword 0x58
    jmp common_stub
handle_interrupt_request_59:
    push qword 0
    push qword 0x59
    jmp common_stub
handle_interrupt_request_5a:
    push qword 0
    push qword 0x5a
    jmp common_stub
handle_interrupt_request_5b:
    push qword 0
    push qword 0x5b
    jmp common_stub
handle_interrupt_request_5c:
    push qword 0
    push qword 0x5c
    jmp common_stub
handle_interrupt_request_5d:
    push qword 0
    push qword 0x5d
    jmp common_stub
handle_interrupt_request_5e:
    push qword 0
    push qword 0x5e
    jmp common_stub
handle_interrupt_request_5f:
    push qword 0
    push qword 0x5f
    jmp common_stub
handle_interrupt_request_60:
    push qword 0
    push qword 0x60
    jmp common_stub
handle_interrupt_request_61:
    push qword 0
    push qword 0x61
    jmp common_stub
handle_interrupt_request_62:
    push qword 0
    push qword 0x62
    jmp common_stub
handle_interrupt_request_63:
    push qword 0
    push qword 0x63
    jmp common_stub
handle_interrupt_request_64:
    push qword 0
    push qword 0x64
    jmp common_stub
handle_interrupt_request_65:
    push qword 0
    push qword 0x65
    jmp common_stub
handle_interrupt_request_66:
    push qword 0
    push qword 0x66
    jmp common_stub
handle_interrupt_request_67:
    push qword 0
    push qword 0x67
    jmp common_stub
handle_interrupt_request_68:
    push qword 0
    push qword 0x68
    jmp common_stub
handle_interrupt_request_69:
    push qword 0
    push qword 0x69
    jmp common_stub
handle_interrupt_request_6a:
    push qword 0
    push qword 0x6a
    jmp common_stub
handle_interrupt_request_6b:
    push qword 0
    push qword 0x6b
    jmp common_stub
handle_interrupt_request_6c:
    push qword 0
    push qword 0x6c
    jmp common_stub
handle_interrupt_request_6d:
    push qword 0
    push qword 0x6d
    jmp common_stub
handle_interrupt_request_6e:
    push qword 0
    push qword 0x6e
    jmp common_stub
handle_interrupt_request_6f:
    push qword 0
    push qword 0x6f
    jmp common_stub
handle_interrupt_request_70:
    push qword 0
    push qword 0x70
    jmp common_stub
handle_interrupt_request_71:
    push qword 0
    push qword 0x71
    jmp common_stub
handle_interrupt_request_72:
    push qword 0
    push qword 0x72
    jmp common_stub
handle_interrupt_request_73:
    push qword 0
    push qword 0x73
    jmp common_stub
handle_interrupt_request_74:
    push qword 0
    push qword 0x74
    jmp common_stub
handle_interrupt_request_75:
    push qword 0
    push qword 0x75
    jmp common_stub
handle_interrupt_request_76:
    push qword 0
    push qword 0x76
    jmp common_stub
handle_interrupt_request_77:
    push qword 0
    push qword 0x77
    jmp common_stub
handle_interrupt_request_78:
    push qword 0
    push qword 0x78
    jmp common_stub
handle_interrupt_request_79:
    push qword 0
    push qword 0x79
    jmp common_stub
handle_interrupt_request_7a:
    push qword 0
    push qword 0x7a
    jmp common_stub
handle_interrupt_request_7b:
    push qword 0
    push qword 0x7b
    jmp common_stub
handle_interrupt_request_7c:
    push qword 0
    push qword 0x7c
    jmp common_stub
handle_interrupt_request_7d:
    push qword 0
    push qword 0x7d
    jmp common_stub
handle_interrupt_request_7e:
    push qword 0
    push qword 0x7e
    jmp common_stub
handle_interrupt_request_7f:
    push qword 0
    push qword 0x7f
    jmp common_stub
handle_interrupt_request_80:
    push qword 0
    push qword 0x80
    jmp common_stub
handle_interrupt_request_81:
    push qword 0
    push qword 0x81
    jmp common_stub
handle_interrupt_request_82:
    push qword 0
    push qword 0x82
    jmp common_stub
handle_interrupt_request_83:
    push qword 0
    push qword 0x83
    jmp common_stub
handle_interrupt_request_84:
    push qword 0
    push qword 0x84
    jmp common_stub
handle_interrupt_request_85:
    push qword 0
    push qword 0x85
    jmp common_stub
handle_interrupt_request_86:
    push qword 0
    push qword 0x86
    jmp common_stub
handle_interrupt_request_87:
    push qword 0
    push qword 0x87
    jmp common_stub
handle_interrupt_request_88:
    push qword 0
    push qword 0x88
    jmp common_stub
handle_interrupt_request_89:
    push qword 0
    push qword 0x89
    jmp common_stub
handle_interrupt_request_8a:
    push qword 0
    push qword 0x8a
    jmp common_stub
handle_interrupt_request_8b:
    push qword 0
    push qword 0x8b
    jmp common_stub
handle_interrupt_request_8c:
    push qword 0
    push qword 0x8c
    jmp common_stub
handle_interrupt_request_8d:
    push qword 0
    push qword 0x8d
    jmp common_stub
handle_interrupt_request_8e:
    push qword 0
    push qword 0x8e
    jmp common_stub
handle_interrupt_request_8f:
    push qword 0
    push qword 0x8f
    jmp common_stub
handle_interrupt_request_90:
    push qword 0
    push qword 0x90
    jmp common_stub
handle_interrupt_request_91:
    push qword 0
    push qword 0x91
    jmp common_stub
handle_interrupt_request_92:
    push qword 0
    push qword 0x92
    jmp common_stub
handle_interrupt_request_93:
    push qword 0
    push qword 0x93
    jmp common_stub
handle_interrupt_request_94:
    push qword 0
    push qword 0x94
    jmp common_stub
handle_interrupt_request_95:
    push qword 0
    push qword 0x95
    jmp common_stub
handle_interrupt_request_96:
    push qword 0
    push qword 0x96
    jmp common_stub
handle_interrupt_request_97:
    push qword 0
    push qword 0x97
    jmp common_stub
handle_interrupt_request_98:
    push qword 0
    push qword 0x98
    jmp common_stub
handle_interrupt_request_99:
    push qword 0
    push qword 0x99
    jmp common_stub
handle_interrupt_request_9a:
    push qword 0
    push qword 0x9a
    jmp common_stub
handle_interrupt_request_9b:
    push qword 0
    push qword 0x9b
    jmp common_stub
handle_interrupt_request_9c:
    push qword 0
    push qword 0x9c
    jmp common_stub
handle_interrupt_request_9d:
    push qword 0
    push qword 0x9d
    jmp common_stub
handle_interrupt_request_9e:
    push qword 0
    push qword 0x9e
    jmp common_stub
handle_interrupt_request_9f:
    push qword 0
    push qword 0x9f
    jmp common_stub
handle_interrupt_request_a0:
    push qword 0
    push qword 0xa0
    jmp common_stub
handle_interrupt_request_a1:
    push qword 0
    push qword 0xa1
    jmp common_stub
handle_interrupt_request_a2:
    push qword 0
    push qword 0xa2
    jmp common_stub
handle_interrupt_request_a3:
    push qword 0
    push qword 0xa3
    jmp common_stub
handle_interrupt_request_a4:
    push qword 0
    push qword 0xa4
    jmp common_stub
handle_interrupt_request_a5:
    push qword 0
    push qword 0xa5
    jmp common_stub
handle_interrupt_request_a6:
    push qword 0
    push qword 0xa6
    jmp common_stub
handle_interrupt_request_a7:
    push qword 0
    push qword 0xa7
    jmp common_stub
handle_interrupt_request_a8:
    push qword 0
    push qword 0xa8
    jmp common_stub
handle_interrupt_request_a9:
    push qword 0
    push qword 0xa9
    jmp common_stub
handle_interrupt_request_aa:
    push qword 0
    push qword 0xaa
    jmp common_stub
handle_interrupt_request_ab:
    push qword 0
    push qword 0xab
    jmp common_stub
handle_interrupt_request_ac:
    push qword 0
    push qword 0xac
    jmp common_stub
handle_interrupt_request_ad:
    push qword 0
    push qword 0xad
    jmp common_stub
handle_interrupt_request_ae:
    push qword 0
    push qword 0xae
    jmp common_stub
handle_interrupt_request_af:
    push qword 0
    push qword 0xaf
    jmp common_stub
handle_interrupt_request_b0:
    push qword 0
    push qword 0xb0
    jmp common_stub
handle_interrupt_request_b1:
    push qword 0
    push qword 0xb1
    jmp common_stub
handle_interrupt_request_b2:
    push qword 0
    push qword 0xb2
    jmp common_stub
handle_interrupt_request_b3:
    push qword 0
    push qword 0xb3
    jmp common_stub
handle_interrupt_request_b4:
    push qword 0
    push qword 0xb4
    jmp common_stub
handle_interrupt_request_b5:
    push qword 0
    push qword 0xb5
    jmp common_stub
handle_interrupt_request_b6:
    push qword 0
    push qword 0xb6
    jmp common_stub
handle_interrupt_request_b7:
    push qword 0
    push qword 0xb7
    jmp common_stub
handle_interrupt_request_b8:
    push qword 0
    push qword 0xb8
    jmp common_stub
handle_interrupt_request_b9:
    push qword 0
    push qword 0xb9
    jmp common_stub
handle_interrupt_request_ba:
    push qword 0
    push qword 0xba
    jmp common_stub
handle_interrupt_request_bb:
    push qword 0
    push qword 0xbb
    jmp common_stub
handle_interrupt_request_bc:
    push qword 0
    push qword 0xbc
    jmp common_stub
handle_interrupt_request_bd:
    push qword 0
    push qword 0xbd
    jmp common_stub
handle_interrupt_request_be:
    push qword 0
    push qword 0xbe
    jmp common_stub
handle_interrupt_request_bf:
    push qword 0
    push qword 0xbf
    jmp common_stub
handle_interrupt_request_c0:
    push qword 0
    push qword 0xc0
    jmp common_stub
handle_interrupt_request_c1:
    push qword 0
    push qword 0xc1
    jmp common_stub
handle_interrupt_request_c2:
    push qword 0
    push qword 0xc2
    jmp common_stub
handle_interrupt_request_c3:
    push qword 0
    push qword 0xc3
    jmp common_stub
handle_interrupt_request_c4:
    push qword 0
    push qword 0xc4
    jmp common_stub
handle_interrupt_request_c5:
    push qword 0
    push qword 0xc5
    jmp common_stub
handle_interrupt_request_c6:
    push qword 0
    push qword 0xc6
    jmp common_stub
handle_interrupt_request_c7:
    push qword 0
    push qword 0xc7
    jmp common_stub
handle_interrupt_request_c8:
    push qword 0
    push qword 0xc8
    jmp common_stub
handle_interrupt_request_c9:
    push qword 0
    push qword 0xc9
    jmp common_stub
handle_interrupt_request_ca:
    push qword 0
    push qword 0xca
    jmp common_stub
handle_interrupt_request_cb:
    push qword 0
    push qword 0xcb
    jmp common_stub
handle_interrupt_request_cc:
    push qword 0
    push qword 0xcc
    jmp common_stub
handle_interrupt_request_cd:
    push qword 0
    push qword 0xcd
    jmp common_stub
handle_interrupt_request_ce:
    push qword 0
    push qword 0xce
    jmp common_stub
handle_interrupt_request_cf:
    push qword 0
    push qword 0xcf
    jmp common_stub
handle_interrupt_request_d0:
    push qword 0
    push qword 0xd0
    jmp common_stub
handle_interrupt_request_d1:
    push qword 0
    push qword 0xd1
    jmp common_stub
handle_interrupt_request_d2:
    push qword 0
    push qword 0xd2
    jmp common_stub
handle_interrupt_request_d3:
    push qword 0
    push qword 0xd3
    jmp common_stub
handle_interrupt_request_d4:
    push qword 0
    push qword 0xd4
    jmp common_stub
handle_interrupt_request_d5:
    push qword 0
    push qword 0xd5
    jmp common_stub
handle_interrupt_request_d6:
    push qword 0
    push qword 0xd6
    jmp common_stub
handle_interrupt_request_d7:
    push qword 0
    push qword 0xd7
    jmp common_stub
handle_interrupt_request_d8:
    push qword 0
    push qword 0xd8
    jmp common_stub
handle_interrupt_request_d9:
    push qword 0
    push qword 0xd9
    jmp common_stub
handle_interrupt_request_da:
    push qword 0
    push qword 0xda
    jmp common_stub
handle_interrupt_request_db:
    push qword 0
    push qword 0xdb
    jmp common_stub
handle_interrupt_request_dc:
    push qword 0
    push qword 0xdc
    jmp common_stub
handle_interrupt_request_dd:
    push qword 0
    push qword 0xdd
    jmp common_stub
handle_interrupt_request_de:
    push qword 0
    push qword 0xde
    jmp common_stub
handle_interrupt_request_df:
    push qword 0
    push qword 0xdf
    jmp common_stub
handle_interrupt_request_e0:
    push qword 0
    push qword 0xe0
    jmp common_stub
handle_interrupt_request_e1:
    push qword 0
    push qword 0xe1
    jmp common_stub
handle_interrupt_request_e2:
    push qword 0
    push qword 0xe2
    jmp common_stub
handle_interrupt_request_e3:
    push qword 0
    push qword 0xe3
    jmp common_stub
handle_interrupt_request_e4:
    push qword 0
    push qword 0xe4
    jmp common_stub
handle_interrupt_request_e5:
    push qword 0
    push qword 0xe5
    jmp common_stub
handle_interrupt_request_e6:
    push qword 0
    push qword 0xe6
    jmp common_stub
handle_interrupt_request_e7:
    push qword 0
    push qword 0xe7
    jmp common_stub
handle_interrupt_request_e8:
    push qword 0
    push qword 0xe8
    jmp common_stub
handle_interrupt_request_e9:
    push qword 0
    push qword 0xe9
    jmp common_stub
handle_interrupt_request_ea:
    push qword 0
    push qword 0xea
    jmp common_stub
handle_interrupt_request_eb:
    push qword 0
    push qword 0xeb
    jmp common_stub
handle_interrupt_request_ec:
    push qword 0
    push qword 0xec
    jmp common_stub
handle_interrupt_request_ed:
    push qword 0
    push qword 0xed
    jmp common_stub
handle_interrupt_request_ee:
    push qword 0
    push qword 0xee
    jmp common_stub
handle_interrupt_request_ef:
    push qword 0
    push qword 0xef
    jmp common_stub
handle_interrupt_request_f0:
    push qword 0
    push qword 0xf0
    jmp common_stub
handle_interrupt_request_f1:
    push qword 0
    push qword 0xf1
    jmp common_stub
handle_interrupt_request_f2:
    push qword 0
    push qword 0xf2
    jmp common_stub
handle_interrupt_request_f3:
    push qword 0
    push qword 0xf3
    jmp common_stub
handle_interrupt_request_f4:
    push qword 0
    push qword 0xf4
    jmp common_stub
handle_interrupt_request_f5:
    push qword 0
    push qword 0xf5
    jmp common_stub
handle_interrupt_request_f6:
    push qword 0
    push qword 0xf6
    jmp common_stub
handle_interrupt_request_f7:
    push qword 0
    push qword 0xf7
    jmp common_stub
handle_interrupt_request_f8:
    push qword 0
    push qword 0xf8
    jmp common_stub
handle_interrupt_request_f9:
    push qword 0
    push qword 0xf9
    jmp common_stub
handle_interrupt_request_fa:
    push qword 0
    push qword 0xfa
    jmp common_stub
handle_interrupt_request_fb:
    push qword 0
    push qword 0xfb
    jmp common_stub
handle_interrupt_request_fc:
    push qword 0
    push qword 0xfc
    jmp common_stub
handle_interrupt_request_fd:
    push qword 0
    push qword 0xfd
    jmp common_stub
handle_interrupt_request_fe:
    push qword 0
    push qword 0xfe
    jmp common_stub
handle_interrupt_request_ff:
    push qword 0
    push qword 0xff
    jmp common_stub