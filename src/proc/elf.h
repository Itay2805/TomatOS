#ifndef __PROC_ELF_H__
#define __PROC_ELF_H__

#include <compo/fs/fs.h>
#include <mm/vmm.h>

typedef struct elf_info {
    uintptr_t entry;
} elf_info_t;

err_t load_elf(file_t file, vmm_handle_t* handle, elf_info_t* info);

#endif //__PROC_ELF_H__
