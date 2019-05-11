# TomatOS

TomatOS is a 64bit operating systems (currently just the kernel part of the os but still).

For specific information about the kernel you can go to the [kernel readme](kernel/README.md)

![Screenshot](screenshot.png)

## Features

* 64bit with all the basic stuff like pmm, vmm, mm...
* Processes (Both kernel ones and user ones)
* Syscalls (open, close read, write, etc..)
* High-res support (currently still a terminal interface tho)
* Cool looking logger
* An embedded shell (WIP)
* Unique resource abstraction system (basically URI based)

## Providers

providers are basically drivers in TomatOS, they provider with function to do stuff with resources

* [`zero:///`](kernel/providers/zero/README.md)
* [`stdio://<type>/`](kernel/providers/stdio/README.md)
* [`ata://<bus>:<port>/`](kernel/providers/ata/README.md)
* [`echfs://[<sub>]/<path>`](kernel/providers/echfs/README.md)
* [`ps2://<type>/`](kernel/providers/ps2/README.md)
* [`elf://[<sub>]/`](kernel/providers/elf/README.md)

## TODO

### Working on

these are things that I am going to actively work on

* Simple embedded shell

### Planned in the near future

these are things that I plan to implement after I finish working on what I worked till now

* write support for echfs
* Text editing program