# PCI
This driver is an abstraction layer between Legacy PCI and PCIe. 

This should support an interface for any driver that needs to communicate with a PCI device.

## PCIe
If the MCFG ACPI table is found, we are going to initialize the PCIe driver. This provides a fast mmio based 
communication with the PCI configuration space.

## Legacy PCI
IF PCIe is not found, we are going to fallback to legacy pci, which uses port io to communicate with the PCI config
space.