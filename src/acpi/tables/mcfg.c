#include <util/debug.h>
#include <memory/vmm.h>
#include "mcfg.h"
#include "rsdt.h"

acpi_mcfg_t* acpi_mcfg;


void mcfg_init() {

    acpi_mcfg = (acpi_mcfg_t*)rsdt_search("MCFG", 0);
    ASSERT(acpi_mcfg != NULL);

    debug_log("[+] \tMCFG - %p\n", DIRECT_TO_PHYSICAL(acpi_mcfg));

}
