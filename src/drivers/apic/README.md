# APIC
This is a newer interrupt controller meant to replace the legacy PIC.

For now we are only going to support using the APIC.

This driver gives basic interface to talk with the I/O APIC as well with the Local APIC. It also implements setting up 
the timer interrupts and mapping of GSIs to IDT entries.