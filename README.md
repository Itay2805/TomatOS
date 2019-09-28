# TomatOS

TomatOS is a 64bit operating systems (currently just the kernel part of the os but still).

The goal of this project is to waste my time.

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
	* got port iteration
* RTL8169 (WIP)
    * got buffer receiving (not tested yet cause need to fix stuff in the scheduler kickstart on real hardware)
* Network stack (WIP)
    * got the frame itself with the arp server, just so I can test how it looks
