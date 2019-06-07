# TomatOS

TomatOS is a 64bit operating systems (currently just the kernel part of the os but still).

## Features

* 64bit with all the basics:
    * idt (right now only handling exceptions, irqs will come once the APIC is supported)
    * gdt
    * pmm
    * vmm
    * mm
* Processes (Kernel and Usermode with signal support (gotta do more testing)) 
    * also no scheduler yet so testing was very limited
* ACPI table parsing (don't use them yet)

