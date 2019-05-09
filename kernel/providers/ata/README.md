# ATA provider

This is a super simple implementation of an ATA driver (registers `ata` provider), it fully implements read, write, seek and tell.

This uses the domain and port fields of the resource descriptor, the domain being which bus (`primary`/`secondary`) and the port being which port on the bus to use.

## TODO

* use interrupts instead of busy waiting
* support lba48 instead of lba28
* add dma support (?)
