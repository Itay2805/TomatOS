#ifndef TOMATKERNEL_ELF_H
#define TOMATKERNEL_ELF_H

#include <common/except.h>
#include <memory/vmm.h>
#include <resource/resource.h>
#include "process/process.h"

error_t elf_provider_init();

#endif //TOMATKERNEL_ELF_H
