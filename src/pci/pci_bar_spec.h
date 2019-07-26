#ifndef PROJECT_PCI_BAR_SPEC_H
#define PROJECT_PCI_BAR_SPEC_H

#define PCI_BAR_TYPE_IO 1
#define PCI_BAR_TYPE_MEM 0

#define PCI_BAR_16BIT 1
#define PCI_BAR_32BIT 0
#define PCI_BAR_64BIT 2

#define PCI_BAR_TYPE(bar) (bar & BIT(0))
#define PCI_BAR_SIZE(bar) ((bar >> 1) & 2)

#define PCI_BAR_MEM_BASE(bar) (bar & ~0b1111)

#endif //PROJECT_PCI_BAR_SPEC_H
