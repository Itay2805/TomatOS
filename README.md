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

### Requirements
* clang-8
* ld.lld-8
* make
* The requirements for [TomatBoot](https://github.com/TomatOrg/TomatBoot-UEFI#requirements)
* The requirements for [image-builder](https://github.com/TomatOrg/image-builder#requirements)

All you need to do is the following
```shell script
git clone git@github.com:TomatOrg/TomatOS.git --recurse-submodules
cd TomatOS
make
```
this should provide you with a bootable image under `bin/tomatos.img` which you can use in a VM or flash into a disk
and boot it.
