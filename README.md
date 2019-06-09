# TomatOS

TomatOS is a 64bit operating systems (currently just the kernel part of the os but still).

## Features

* 64bit with all the basics (gdt, pmm, vmm...)
* SSE enabling is implemented but disabled (causes problem with saving context)
* Processes (Kernel and Usermode)
    * preserves fpu state
    * including simple preemptive FIFS scheduler
* ACPI table parsing
    * no AML yet
* Uses APIC
* PCI / PCIe support (with single abstraction layer)
