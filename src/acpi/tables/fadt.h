#ifndef TOMATKERNEL_FADT_H
#define TOMATKERNEL_FADT_H

#include <acpispec/tables.h>

#include "tables.h"
#include "rsdt.h"

extern acpi_fadt_t* acpi_fadt;
extern acpi_aml_t* acpi_dsdt;

void fadt_init();

#endif //TOMATKERNEL_FADT_H
