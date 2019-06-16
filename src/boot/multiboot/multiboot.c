#include "multiboot.h"

#include <drivers/acpi/tables/rsdp.h>
#include <drivers/acpi/acpi.h>

#include <boot/allocator.h>
#include <boot/boot.h>
#include <string.h>

void multiboot_main(multiboot_info_t* info) {
    boot_allocator_init(info);

    boot_info_t* new = boot_allocate(sizeof(boot_info_t));

    /** Total memory **/
    {
        // we require to have the basic memory info
        if((info->flags & MULTIBOOT_INFO_MEMORY) == 0) return;
        new->total_mem = (info->mem_lower + info->mem_upper) * 1000;
    }

    /** Memory Map **/
    {
        // we require to have memory map
        if((info->flags & MULTIBOOT_INFO_MEM_MAP) == 0) return;

        // first we gotta count
        multiboot_memory_map_t* entries = (multiboot_memory_map_t*)(uintptr_t)info->mmap_addr;
        multiboot_memory_map_t* it;
        for(it = entries; (char*)it - (char*)entries < info->mmap_length; it++) {
            switch(it->type) {
                case MULTIBOOT_MEMORY_RESERVED:
                case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
                case MULTIBOOT_MEMORY_NVS:
                case MULTIBOOT_MEMORY_AVAILABLE:
                case MULTIBOOT_MEMORY_BADRAM: {
                    new->mmap.count++;
                } break;
                default:break;
            }
        }

        // allocate and fill the table
        new->mmap.entries = boot_allocate(new->mmap.count * sizeof(mmap_entry_t));
        size_t i = 0;
        for(it = entries; (char*)it - (char*)entries < info->mmap_length; it++, i++) {
            mmap_entry_t* entry = &new->mmap.entries[i];
            entry->addr = it->addr;
            entry->size = it->len;
            switch(it->type) {
                case MULTIBOOT_MEMORY_RESERVED: {
                    entry->type = MMAP_RESERVED;
                } break;
                case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE: {
                    entry->type = MMAP_ACPI_RELAIMABLE;
                } break;
                case MULTIBOOT_MEMORY_NVS: {
                    entry->type = MMAP_ACPI_NVS;
                } break;
                case MULTIBOOT_MEMORY_AVAILABLE: {
                    entry->type = MMAP_AVAILABLE;
                } break;
                case MULTIBOOT_MEMORY_BADRAM: {
                    entry->type = MMAP_BAD_MEMORY;
                } break;
                default:break;
            }
        }
    }

    /** Framebuffer **/
    {
        // framebuffer info
        if(info->flags & MULTIBOOT_INFO_FRAMEBUFFER_INFO) {
            // pass the framebuffer
            if(info->framebuffer.type == MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED) {
                // we don't support indexed mode
                new->framebuffer.type = FRAMEBUFFER_NOT_SUPPORTED;
            }else {
                new->framebuffer.type = info->framebuffer.type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB ? FRAMEBUFFER_GRAPHICS_MODE : FRAMEBUFFER_TEXT_MODE;
                new->framebuffer.addr = info->framebuffer.addr;
                new->framebuffer.height = info->framebuffer.height;
                new->framebuffer.width = info->framebuffer.width;
                new->framebuffer.bpp = info->framebuffer.bpp;
            }
        }else {
            // assume text mode
            new->framebuffer.type = FRAMEBUFFER_TEXT_MODE;
            new->framebuffer.addr = 0xb8000;
            new->framebuffer.height = 25;
            new->framebuffer.width = 80;
        }
    }

    /** RSDP **/
    {
        // we are going to assume this is a legacy system and search it
        for(char* cur = (char *) 0x000E0000; cur < (char*)(0x000FFFFF - sizeof(rsdp_t)); cur++) {
            // signature
            if(memcmp(((rsdp_t*)cur)->signature, "RSD PTR ", 8) != 0) continue;

            // checksum
            if(((rsdp_t*)cur)->revision == 1) {
                if(!acpi_validate_checksum(cur, sizeof(rsdp_t))) continue;
            }else {
                if(!acpi_validate_checksum(cur, ((rsdp2_t*)cur)->length)) continue;
            }

            // found rsdp
            new->rsdp_ptr = (uint64_t) cur;
            break;
        }
    }

    // call the kernel
    kernel_main(new);
}
