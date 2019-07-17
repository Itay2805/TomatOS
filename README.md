# TomatOS

TomatOS is a 64bit operating systems (currently just the kernel part of the os but still).

## Features

* 64bit with all the basics (gdt, pmm, vmm...)
* Uses [TomatBoot](https://github.com/TomatOrg/TomatBoot-UEFI)
* ACPI
    * Mostly reading tables
    * AML Interpreter provided with [lai](https://github.com/qword-os/lai)
* APIC
* PCI / PCIe support (with single abstraction layer)
* Processes (Kernel and Usermode:tm:)
    * preserves fpu state
    * including simple preemptive FIFS scheduler
