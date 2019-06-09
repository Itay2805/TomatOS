cmake_minimum_required(VERSION 3.10)

project(TomatKernel)

# Setup C
set(CMAKE_C_COMPILER /usr/bin/gcc)

# Setup using asm
enable_language(ASM)
set(CMAKE_ASM_COMPILER /usr/bin/nasm)
set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> <SOURCE> <FLAGS> -o <OBJECT>")

# Setup compilation flags
set(CMAKE_C_FLAGS "-z max-page-size=4096 -fPIC -Wall -std=gnu99 -nostdinc -fno-builtin -fno-stack-protector -mno-sse -mno-red-zone  -march=x86-64")
set(CMAKE_ASM_FLAGS "-F dwarf -f elf64")
set(CMAKE_EXE_LINKER_FLAGS "-z max-page-size=4096 -no-pie -nostdlib -nodefaultlibs -nostartfiles -T ${CMAKE_SOURCE_DIR}/config/multiboot.ld")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

# For debugging
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_C_FLAGS "-g -fno-omit-frame-pointer ${CMAKE_C_FLAGS}")
    set(CMAKE_ASM_FLAGS "-g ${CMAKE_ASM_FLAGS}")
endif ()

# For release
if (CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_C_FLAGS "-O3 ${CMAKE_C_FLAGS}")
    set(CMAKE_ASM_FLAGS "-O3 ${CMAKE_ASM_FLAGS}")
endif ()

set(COMMON_SOURCE_FILES
        lib/common/cpu/cr.h
        lib/common/cpu/cpuid.h
        lib/common/cpu/fpu.c
        lib/common/cpu/fpu.h
        lib/common/cpu/msr.h
        lib/common/cpu/rflags.h
        lib/common/buf.c
        lib/common/buf.h
        lib/common/common.h
        lib/common/elf64.h
        lib/common/elf_common.h
        lib/common/error.c
        lib/common/error.h
        lib/common/hash.h
        lib/common/map.c
        lib/common/map.h
        lib/common/vsnprintf.c
        lib/common/vsnprintf.h
)

set(LIBC_SOURCE_FILES
        lib/libc/stdarg.h
        lib/libc/stdbool.h
        lib/libc/stddef.h
        lib/libc/stdint.h
        lib/libc/ctype.h
        lib/libc/stdlib.h
        lib/libc/stdlib.c
        lib/libc/string.c
        lib/libc/string.h
)

# Setup kernel sources
set(KERNEL_SOURCE_FILES
        src/boot/multiboot.asm
        src/boot/multiboot.h

        src/drivers/acpi/acpi.c
        src/drivers/acpi/acpi.h
        src/drivers/acpi/madt.c
        src/drivers/acpi/madt.h
        src/drivers/acpi/mcfg.c
        src/drivers/acpi/mcfg.h
        src/drivers/acpi/rdsp.c
        src/drivers/acpi/rsdp.h
        src/drivers/acpi/rsdt.c
        src/drivers/acpi/rsdt.h

        src/drivers/apic/apic.c
        src/drivers/apic/apic.h
        src/drivers/apic/ioapic.c
        src/drivers/apic/ioapic.h
        src/drivers/apic/lapic.c
        src/drivers/apic/lapic.h

        src/drivers/pci/legacy.c
        src/drivers/pci/legacy.h
        src/drivers/pci/pci.c
        src/drivers/pci/pci.h
        src/drivers/pci/pcie.c
        src/drivers/pci/pcie.h

        src/drivers/pic8259/pic.c
        src/drivers/pic8259/pic.h

        src/drivers/vmdev/vmdev.c
        src/drivers/vmdev/vmdev.h

        src/drivers/rtc/rtc.h
        src/drivers/rtc/rtc.c

        src/drivers/portio.h

        src/interrupts/idt.c
        src/interrupts/idt.h
        src/interrupts/interrupts.h
        src/interrupts/irq.asm
        src/interrupts/irq.c
        src/interrupts/irq.h
        src/interrupts/isr.asm
        src/interrupts/isr.c
        src/interrupts/isr.h
        src/interrupts/timer.c
        src/interrupts/timer.h

        src/logger/logger.c
        src/logger/logger.h

        src/memory/gdt.c
        src/memory/gdt.h
        src/memory/pmm.c
        src/memory/pmm.h
        src/memory/vmm.c
        src/memory/vmm.h
        src/memory/mm.c
        src/memory/mm.h

        src/process/process.c
        src/process/process.h
        src/process/scheduler.c
        src/process/scheduler.h
        src/process/signal.asm
        src/process/signal.c
        src/process/signal.h
        src/process/syscalls.h
        src/process/thread.c
        src/process/thread.h

        src/main.c)

include_directories(src)
include_directories(lib/libc)
include_directories(lib/common)

add_executable(TomatKernel ${KERNEL_SOURCE_FILES} ${COMMON_SOURCE_FILES} ${LIBC_SOURCE_FILES})
add_custom_command(
        TARGET TomatKernel
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/iso/boot/grub
        COMMAND cp ${CMAKE_BINARY_DIR}/TomatKernel ${CMAKE_BINARY_DIR}/iso/boot/TomatKernel
        COMMAND cp ${CMAKE_SOURCE_DIR}/config/multiboot.grub.cfg ${CMAKE_BINARY_DIR}/iso/boot/grub/grub.cfg
        COMMAND grub-mkrescue -o ${CMAKE_SOURCE_DIR}/tomatos.iso ${CMAKE_BINARY_DIR}/iso
)
