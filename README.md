# TomatOS

TomatOS is a 64bit operating systems (currently just the kernel part of the os but still).

The goal is to make TomatOS a quite modern hobby os in the sense that we are not going to support legacy hardware at 
all.

## Features

* 64bit with all the basics (gdt, pmm, vmm...)
* Uses [TomatBoot](https://github.com/TomatOrg/TomatBoot-UEFI)
* ACPI
    * Mostly reading tables
* APIC for interrupt handling
* PCI / PCIe support (with single abstraction layer)

#### TODO
* Add SMP and do the lapic stuff per core
* Remove the portio PCI support
* Get back processes, schedule with the lapic timer
* Add back LAI for doing cool AML stuff