#ifndef TOMATKERNEL_ELF64_LOADER_H
#define TOMATKERNEL_ELF64_LOADER_H

#include <resource/resource.h>
#include <memory/vmm.h>
#include <process/process.h>

error_t load_elf64(process_t* process, resource_t res);

#endif //TOMATKERNEL_ELF64_LOADER_H
