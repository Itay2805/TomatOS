# TomatOS

TomatOS is a 64bit operating systems (currently just the kernel part of the os but still).

The goal of this project is to waste my time.

## Kernel Features

### Hardware
* 64bit with all the basics (gdt, pmm, vmm...)
* Uses UEFI for starting (with [TomatBoot](https://github.com/TomatOrg/TomatBoot-UEFI))
* ACPI
    * AML interpreter powered by [Lai](https://github.com/qword-os/lai)
    * Used to create the SCI handler and iterating PCI devices properly
* APIC
    * Full Local + IO APIC support
* PCI
    * full scan + objects that abstract everything
* SMP
    * including a simple scheduler

### Software
* Processes and Threads are supported

## TODO
* syscalls and usermode testing
