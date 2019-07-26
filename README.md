# TomatOS

TomatOS is a 64bit operating systems (currently just the kernel part of the os but still).

The goal is to make TomatOS a quite modern hobby os in the sense that we are not going to support legacy hardware at 
all.

## Features

* 64bit with all the basics (gdt, pmm, vmm...)
* Uses UEFI for starting (with [TomatBoot](https://github.com/TomatOrg/TomatBoot-UEFI))
* ACPI
    * Reading tables
    * AML execution powered by [Lai](https://github.com/qword-os/lai)
    * SCI events handling (TODO: Actually make it respond to the events)
* APIC for interrupt handling
    * LAPIC timer for scheduling
* PCI support (MCFG only)
    * bar management
    * IRQ routing
* SMP (Just the frame, still gotta make the trampoline)
    * Seamless Per CPU storage
