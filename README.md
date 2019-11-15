# TomatOS

TomatOS is a 64bit operating systems (currently just the kernel part of the os but still).

The goal of this project is to waste my time.

## Kernel Features

* 64bit with all the basics (gdt, pmm, vmm...)
* Uses UEFI for starting (with [TomatBoot](https://github.com/TomatOrg/TomatBoot-UEFI))
* ACPI
    * AML interpreter powered by [Lai](https://github.com/qword-os/lai)
* APIC
* SMP
* PCI
* AHCI - read only but write will be implemented soon
* Processes and Threads are supported
* GPT
* [EchFS](https://github.com/qword-os/echfs)

## Building it
First you need to clone the repo, make sure to do a recursive clone:
```
git clone git@github.com:TomatOrg/TomatOS.git --recurse-submodules
```

## TODO
* syscalls and usermode testing
