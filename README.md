# TomatOS

***[Note: I have moved to work on another project as of that I will not continue working on this probably](https://github.com/kretlim/kretlim-kernel)***

TomatOS is a 64bit operating systems (currently just the kernel part of the os but still).

## Features

* 64bit with all the basics (gdt, pmm, vmm...)
* Boot abstraction 
    * allows to support both multiboot 1 and 2 (and any bootloader that can provide the basics we need)
* ACPI
    * AML Interpreter provided with [lai](https://github.com/qword-os/lai)
* APIC
* PCI / PCIe support (with single abstraction layer)
* Processes (Kernel and Usermode)
    * preserves fpu state
    * including simple preemptive FIFS scheduler
* SSE enabling is implemented but disabled (causes problem with saving context)
