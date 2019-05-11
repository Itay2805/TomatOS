# ELF provider

The elf provider allows for loading elf files into a new process.

Currently this only supports static elf loading and will not do any relocation related work.

## Subresource requirements
The subresource has to be readable and seekable.

## TODO
* Allow to choose the address space (new process or current process as library)
