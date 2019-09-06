# TomatOS

TomatOS is a 64bit operating systems (currently just the kernel part of the os but still).

The goal is to make TomatOS a quite modern hobby os in the sense that we are not going to support legacy hardware at 
all.

## Features

* 64bit with all the basics (gdt, pmm, vmm...)
* Uses UEFI for starting (with [TomatBoot](https://github.com/TomatOrg/TomatBoot-UEFI))
* ACPI
    * Using the tables to get relevant info
    * AML execution powered by [Lai](https://github.com/qword-os/lai)
    * SCI events handling (only handles shutdown)
* APIC for interrupt handling
    * LAPIC timer for scheduling
* PCI support (MCFG only)
	* using acpi to get the root bridges
    * bar management
    * IRQ routing (kinda broken currently, needs fixing)
* ~~SMP~~ (only the frame, TODO)

### Drivers

* AHCI (WIP)
	* with support for SATA devices
	
### Roadmap
* finish AHCI SATA support
* add GPT partitioning support
* add echfs filesystem (at least read-only)
* ???
* profit!