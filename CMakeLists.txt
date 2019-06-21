cmake_minimum_required(VERSION 3.10)

project(TomatKernel)

# Setup C
set(CMAKE_C_COMPILER /usr/bin/gcc)

# Setup using asm
enable_language(ASM)
set(CMAKE_ASM_COMPILER /usr/bin/nasm)
set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> <SOURCE> <FLAGS> -o <OBJECT>")

# Setup compilation flags
set(CMAKE_C_FLAGS "-g -z max-page-size=4096 -fPIC -Wall -std=gnu99 -nostdinc -fno-builtin -fno-stack-protector -mno-sse -mno-red-zone  -march=x86-64")
set(CMAKE_ASM_FLAGS "-g -F dwarf -f elf64")
set(CMAKE_EXE_LINKER_FLAGS "-z max-page-size=4096 -no-pie -nostdlib -nodefaultlibs -nostartfiles -T ${CMAKE_SOURCE_DIR}/config/multiboot.ld")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

# Lai source files
# TODO: Make lai optional (?)
set(LAI_SOURCE_FILES
        lib/lai/src/eval.c
        lib/lai/src/exec.c
        lib/lai/src/exec2.c
        lib/lai/src/libc.c
        lib/lai/src/ns.c
        lib/lai/src/opregion.c
        lib/lai/src/os_methods.c
        lib/lai/src/pciroute.c
        lib/lai/src/resource.c
        lib/lai/src/sci.c
        lib/lai/src/sleep.c
        lib/lai/src/vsnprintf.c
)

# Common library source files
set(COMMON_SOURCE_FILES
        lib/common/cpu/atomic.h
        lib/common/cpu/cr.h
        lib/common/cpu/cpuid.h
        lib/common/cpu/fpu.c
        lib/common/cpu/fpu.h
        lib/common/cpu/msr.h
        lib/common/cpu/rflags.h
        lib/common/locks/spinlock.c
        lib/common/locks/spinlock.h
        lib/common/locks/preemption.h
        lib/common/buf.c
        lib/common/buf.h
        lib/common/common.h
        lib/common/error.c
        lib/common/error.h
        lib/common/hash.h
        lib/common/map.c
        lib/common/map.h
        lib/common/vsnprintf.c
        lib/common/vsnprintf.h
)

# Libc source files
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

set(NAME_INCLUDE "default value" CACHE FILEPATH "description")

# By default we are going to build multiboot1 binary,
# but optionally we have the option to build multiboot 2 binaries
set(BOOT_MULTIBOOT_2 "")
IF(DEFINED BOOT_MULTIBOOT_2)
    # Multiboot 2 stuff
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__BOOT_MULTIBOOT_2__")
    set(BOOT_SOURCE_FILES
        src/boot/multiboot2/multiboot2.asm
        src/boot/multiboot2/multiboot2.h
        src/boot/multiboot2/multiboot2.c
        )

ELSE()
    # Multiboot 1 stuff
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__BOOT_MULTIBOOT_1__")
    set(BOOT_SOURCE_FILES
        src/boot/multiboot/multiboot.asm
        src/boot/multiboot/multiboot.h
        src/boot/multiboot/multiboot.c
        )
ENDIF()


# Setup kernel sources
set(KERNEL_SOURCE_FILES
        src/boot/allocator.c
        src/boot/allocator.h
        src/boot/boot.h

        src/drivers/acpi/acpi.c
        src/drivers/acpi/acpi.h
        src/drivers/acpi/lai_host.c
        src/drivers/acpi/tables/fadt.c
        src/drivers/acpi/tables/fadt.h
        src/drivers/acpi/tables/madt.c
        src/drivers/acpi/tables/madt.h
        src/drivers/acpi/tables/mcfg.c
        src/drivers/acpi/tables/mcfg.h
        src/drivers/acpi/tables/rdsp.c
        src/drivers/acpi/tables/rsdp.h
        src/drivers/acpi/tables/rsdt.c
        src/drivers/acpi/tables/rsdt.h

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

        src/drivers/term/font.c
        src/drivers/term/font.h
        src/drivers/term/graphics.c
        src/drivers/term/graphics.h
        src/drivers/term/term.c
        src/drivers/term/term.h
        src/drivers/term/text.c
        src/drivers/term/text.h

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
include_directories(lib/lai/include)
include_directories(lib/common)

add_executable(TomatKernel ${BOOT_SOURCE_FILES} ${KERNEL_SOURCE_FILES} ${COMMON_SOURCE_FILES} ${LIBC_SOURCE_FILES} ${LAI_SOURCE_FILES})

# Check what we need to do to create an image
IF(DEFINED BOOT_MULTIBOOT_2)
    add_custom_command(
        TARGET TomatKernel
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/iso/boot/grub
        COMMAND cp ${CMAKE_BINARY_DIR}/TomatKernel ${CMAKE_BINARY_DIR}/iso/boot/TomatKernel
        COMMAND cp ${CMAKE_SOURCE_DIR}/config/multiboot2.grub.cfg ${CMAKE_BINARY_DIR}/iso/boot/grub/grub.cfg
        COMMAND grub-mkrescue -o ${CMAKE_SOURCE_DIR}/tomatos.iso ${CMAKE_BINARY_DIR}/iso
    )
ELSE()
    add_custom_command(
        TARGET TomatKernel
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/iso/boot/grub
        COMMAND cp ${CMAKE_BINARY_DIR}/TomatKernel ${CMAKE_BINARY_DIR}/iso/boot/TomatKernel
        COMMAND cp ${CMAKE_SOURCE_DIR}/config/multiboot.grub.cfg ${CMAKE_BINARY_DIR}/iso/boot/grub/grub.cfg
        COMMAND grub-mkrescue -o ${CMAKE_SOURCE_DIR}/tomatos.iso ${CMAKE_BINARY_DIR}/iso
    )
ENDIF()
