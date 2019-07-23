# TomatOS

TomatOS is a 64bit operating systems (currently just the kernel part of the os but still).

The goal is to make TomatOS a quite modern hobby os in the sense that we are not going to support legacy hardware at 
all.

## Features

* 64bit with all the basics (gdt, pmm, vmm...)
* Uses UEFI for starting (with [TomatBoot](https://github.com/TomatOrg/TomatBoot-UEFI))
* ACPI
    * Mostly reading tables
* APIC for interrupt handling
* PCI support (MCFG only)

#### Roadmap
* Finish SMP support (basically stop being lazy and write real mode -> long mode trampoline, everything else around it 
  is finished already)
* Get processes back
* Implement back LAI

