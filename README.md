# TomatOS

TomatOS is a 64bit operating systems (currently just the kernel part of the os but still).

## Features

* 64bit with all the basics (gdt, pmm, vmm...)
* Uses [TomatBoot](https://github.com/TomatOrg/TomatBoot-UEFI)
* ACPI
    * Mostly reading tables
* APIC for interrupt handling
* PCI / PCIe support (with single abstraction layer)

### Working on
* Refactoring and getting this back to work

### TODO:
* Get processes back
* Implement the object system
* Readd LAI
* SMP:tm:
