#include <elf64.h>
#include <arch/paging.h>
#include <string.h>
#include "elf.h"

err_t load_elf(file_t file, vmm_handle_t* handle, elf_info_t* info) {
    err_t err = NO_ERROR;

    CHECK_ERROR(file != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(handle != NULL, ERROR_INVALID_PARAM);
    CHECK_ERROR(info != NULL, ERROR_INVALID_PARAM);


    Elf64_Ehdr ehdr;
    CHECK_AND_RETHROW(file_read(file, &ehdr, sizeof(ehdr)));

    // verify this is an elf file
    CHECK(IS_ELF(ehdr));

    // verify the elf type
    CHECK(ehdr.e_ident[EI_VERSION] == EV_CURRENT);
    CHECK(ehdr.e_ident[EI_CLASS] == ELFCLASS64);
    CHECK(ehdr.e_ident[EI_DATA] == ELFDATA2LSB);

    // Load from section headers
    Elf64_Phdr phdr;
    for (int i = 0; i < ehdr.e_phnum; i++) {
        CHECK_AND_RETHROW(file_seek(file, ehdr.e_phoff + ehdr.e_phentsize * i, SEEK_SET));
        CHECK_AND_RETHROW(file_read(file, &phdr, sizeof(Elf64_Phdr)));

        switch (phdr.p_type) {
            // normal section
            case PT_LOAD:
                // ignore empty sections
                if (phdr.p_memsz == 0) continue;

                // allocate the address, map as read write for starters
                uintptr_t base = phdr.p_vaddr;
                CHECK_AND_RETHROW(vmm_user_allocate(handle, ALLOCATE_ADDRESS, &base, SIZE_TO_PAGES(phdr.p_memsz), PAGE_SUPERVISOR_READWRITE));
                CHECK_AND_RETHROW(file_seek(file, phdr.p_offset, SEEK_SET));
                CHECK_AND_RETHROW(file_read(file, (void*)base, phdr.p_filesz));
                memset((void*)(base + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);

                // set the correct permissions
                page_perms_t perms = PAGE_READONLY;
                if (phdr.p_flags & PF_X) perms |= PAGE_EXECUTE;
                if (phdr.p_flags & PF_W) perms |= PAGE_WRITE;
                vmm_set_perms(handle, base, phdr.p_memsz, perms);

                // ignore entry
            default:
                break;
        }
    }

    // copy the entry
    info->entry = ehdr.e_entry;

cleanup:
    return err;
}
