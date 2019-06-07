#include "legacy.h"

bool pci_legacy_supported() {
    return false;
}

error_t pci_legacy_init() {
    return NO_ERROR;
}
