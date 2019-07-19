# Source Layout

## acpi
## drivers
This is a generic folder that will contain generic drivers the kernel will use, these drivers are not specific to 
one thing so they are put here.

This should be renamed to something else since the actual drivers are going to have a special structure rather than how 
these are just library api like for the device.

### rtc
This is a simple RTC driver, only supports getting the time, it is technically legacy but we support it since it is 
the only to get the current date and time without using some network based time protocols.

#### hpet (soon)
This is a simple (for now) HPET driver

for starters it will only support one hpet, and will give the ability to stall and will raise an interrupt every 1ms.

In the future this will be able to use multiple HPETs and will allocate and use the comparators dynamically to have 
more accurate and better optimized timers/sleeps 

## interrupts
Everything to do with interrupts, from the interrupt stub, allocations to exception handling.

This also contains the drivers to work with the APIC.

We do not support the PIC and the driver we have is only used to disable it.

## logger
This contains a logging system for easier debugging

The following loggers are supported
* `vmdev` - an E9 logger, also logs to port 0x504 (virtualbox)
* `term` - a framebuffer logger, logs to a terminal (basic one, no scrolling)

## memory
Contains the memory management code

* `pmm` - Physical memory manager, allows for allocating physical pages, right now a stack based allocator, should 
change to a bitmap one probably.
* `vmm` - Virtual Memory Manager - anything to do with virtual memory map, allows to map virtual pages
* `mm` - Memory Manager, basically a kernel heap for dynamic memory allocation, linked list based.

There is also code to deal with the GDT and TSS in this folder

## pci
This contains the code to deal with PCI/PCIe

This will manage everything from the devices, the bars to the interrupts of each PCI device. just to make it easier on 
the driver.

I am going to remove the support for portio based pci access and only support the mmio based pci access, with the 
reason being simplicity and any modern computer should support the mmio access.

## processes (soon)
This will handle everything to do with processes.

So it will have the process code, the thread code and the scheduler code. This will also have the syscall 
implementations and anything related to usermode.

## smp
This contains code related to SMP

right now only contains the per cpu storage stuff