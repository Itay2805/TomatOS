# ACPI

Or **A**dvanced **C**onfiguration and **P**ower **I**nterface is a standard that allows the operating system to 
interface with the firmware of the computer and get much more control over here.

The main goal of this driver is to get information about the firmware and be used to implement many more drivers.

## Tables
The acpi tables are used to provide alot more information about the hardware and firmware of the device.

We are using the following tables

### RDSP
The root table, if it is presented we know the computer supports ACPI

###  RSDT/XSDT
The table contains pointer to all other tables

### MADT/ACIC
This table is used to get information about the APIC

### MCFG
This table is used for getting information about PCIe. 

Note that this table is optional, and if not found we will simply use the legacy PCI interface.

## AML
The ACPI Machine Language, basically byte code we need to run in order to talk with the firmware successfully.

**This is still a work in progress**