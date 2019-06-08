# TomatOS

TomatOS is a 64bit operating systems (currently just the kernel part of the os but still).

## Features

* 64bit with all the basics (gdt, pmm, vmm...)
* Processes (Kernel and Usermode with signal support (gotta do more testing)) 
    * no scheduler yet so testing was very limited
* ACPI table parsing
    * no AML yet
* APIC support (Local APIC initialization only for now)
* PCI / PCIe support (with single abstraction layer)
