#ifndef PROJECT_PCI_BAR_SPEC_H
#define PROJECT_PCI_BAR_SPEC_H

#define PCI_BAR_TYPE_IO 1
#define PCI_BAR_TYPE_MEM 0
#define PCI_BAR_TYPE(bar) (bar & BIT(0))

// Memory bar
#define PCI_BAR_MEMORY_WIDTH_32BIT 0
#define PCI_BAR_MEMORY_WIDTH_64BIT 2
#define PCI_BAR_MEMORY_WIDTH(bar) ((bar >> 1) & 2)
#define PCI_BAR_MEMORY_BASE(bar) (bar & ~0b1111)

// IO Bar
#define PCI_BAR_IO_BASE(bar) (bar & ~0b11)

#endif //PROJECT_PCI_BAR_SPEC_H
