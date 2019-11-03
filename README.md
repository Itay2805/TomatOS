# TomatOS

TomatOS is a 64bit operating systems (currently just the kernel part of the os but still).

The goal of this project is to waste my time.

## Kernel Features

* 64bit with all the basics (gdt, pmm, vmm...)
* Uses UEFI for starting (with [TomatBoot](https://github.com/TomatOrg/TomatBoot-UEFI))
* ACPI
    * Using the tables to get relevant info
* APIC for interrupt handling
    * LAPIC timer for scheduling (soon™)
* SMP

## TODO
* Work on the object framework 
* Add Thread and Process objects
* Work on the scheduler 
