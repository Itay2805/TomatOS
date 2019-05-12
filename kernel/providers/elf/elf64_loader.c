#include <common/string.h>
#include "elf64_loader.h"

#include <common/klib.h>
#include <process/process.h>

#include "elf64.h"

static error_t load_exec(process_t* process, resource_t res, Elf64_Ehdr* header) {
    error_t err = NO_ERROR;
    Elf64_Phdr* phdr = kalloc(sizeof(Elf64_Phdr));
    char* buffer = NULL;

    CHECK(header->e_entry != NULL);
    process->threads[0]->cpu_state.rip = header->e_entry;

    CHECK(header->e_phoff != NULL);
    CHECK(header->e_phnum > 0);
    for(int i = 0; i < header->e_phnum; i++) {
        // read the header
        CHECK_AND_RETHROW(kseek(res, SEEK_START, header->e_phoff + header->e_phentsize * i));
        CHECK_AND_RETHROW(kread(res, phdr, sizeof(Elf64_Phdr), NULL));

        if(phdr->p_type == PT_NULL) continue;

        if(phdr->p_type == PT_LOAD) {
            // allocate the pages in memory
            int attrs = PAGE_ATTR_USER;
            if(phdr->p_flags & PF_W) attrs |= PAGE_ATTR_WRITE;
            if(phdr->p_flags & PF_X) attrs |= PAGE_ATTR_EXECUTE;
            for(uint64_t start = ALIGN_DOWN(phdr->p_vaddr, KB(4)); start < ALIGN_UP(phdr->p_vaddr + phdr->p_memsz, KB(4)); start += KB(4)) {
                vmm_allocate(process->address_space, (volatile void *) start, attrs);
            }

            // clear
            vmm_clear_user(process->address_space, (void *) phdr->p_vaddr, phdr->p_memsz);

            // only do this if has size on disk
            if(phdr->p_filesz > 0) {
                // get the blob from the file
                buffer = kalloc(phdr->p_filesz);
                CHECK_AND_RETHROW(kseek(res, SEEK_START, phdr->p_offset));
                CHECK_AND_RETHROW(kread(res, buffer, phdr->p_filesz, NULL));

                // copy the data from the file and free the buffer
                CHECK_AND_RETHROW(vmm_copy_to_user(process->address_space, buffer, (void*)phdr->p_vaddr, phdr->p_filesz));
                kfree(buffer);
                buffer = NULL;
            }
        }

    }

cleanup:
    if(buffer != NULL) {
        kfree(buffer);
    }
    if(phdr != NULL) {
        kfree(phdr);
    }
    return err;
}

error_t load_elf64(process_t* process, resource_t res) {
    error_t err = NO_ERROR;
    Elf64_Ehdr* header = kalloc(sizeof(Elf64_Ehdr));

    // read the header and check it
    CHECK_AND_RETHROW(kread(res, header, sizeof(Elf64_Ehdr), NULL));
    CHECK(header->e_ident[EI_MAG0] == ELFMAG0);
    CHECK(header->e_ident[EI_MAG1] == ELFMAG1);
    CHECK(header->e_ident[EI_MAG2] == ELFMAG2);
    CHECK(header->e_ident[EI_MAG3] == ELFMAG3);
    CHECK(header->e_ident[EI_CLASS] == ELFCLASS64);
    CHECK(header->e_ident[EI_DATA] == ELFDATA2LSB);
    CHECK(header->e_ident[EI_VERSION] == EV_CURRENT);
    CHECK(header->e_machine == EM_X86_64);

    if(header->e_type == ET_EXEC) {
        CHECK_AND_RETHROW(load_exec(process, res, header));
    }else {
        CHECK_FAIL();
    }

    // allocate the stack, will start from 0000800000000000 and will go downwards
    // we are going to allocate 5mb for the thread
    for(uint64_t start = 0x800000000000 - KB(4); start >= 0x800000000000 - KB(4) - MB(5); start -= KB(4)) {
        vmm_allocate(process->address_space, (volatile void *) start, PAGE_ATTR_WRITE | PAGE_ATTR_USER);
    }
    process->threads[0]->cpu_state.rsp = 0x800000000000 - KB(4);
    process->threads[0]->cpu_state.rbp = 0x800000000000 - KB(4);

cleanup:
    if(header != NULL) {
        kfree(header);
    }
    return err;
}