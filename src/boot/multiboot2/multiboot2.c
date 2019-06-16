#include "multiboot2.h"

#include <drivers/acpi/tables/rsdp.h>
#include <drivers/acpi/acpi.h>

#include <boot/allocator.h>
#include <boot/boot.h>
#include <string.h>
#include <common.h>

static mmap_type_t mmap_types[] = {
    [MULTIBOOT_MEMORY_AVAILABLE] = MMAP_AVAILABLE,
    [MULTIBOOT_MEMORY_RESERVED] = MMAP_RESERVED,
    [MULTIBOOT_MEMORY_ACPI_RECLAIMABLE] = MMAP_ACPI_RELAIMABLE,
    [MULTIBOOT_MEMORY_NVS] = MMAP_ACPI_NVS,
    [MULTIBOOT_MEMORY_BADRAM] = MMAP_BAD_MEMORY
};

void multiboot_main(char* addr) {
    boot_allocator_init(addr);

    boot_info_t* new = boot_allocate(sizeof(boot_info_t));
    memset(new, 0, sizeof(boot_info_t));

    // fill the default info
    new->framebuffer.type = FRAMEBUFFER_TEXT_MODE;
    new->framebuffer.addr = 0xb8000;
    new->framebuffer.height = 25;
    new->framebuffer.width = 80;

    // iterate all the tags
    struct multiboot_tag* it;
    for (it = (struct multiboot_tag *) (addr + 8);
         it->type != MULTIBOOT_TAG_TYPE_END;
         it = (struct multiboot_tag *) ((multiboot_uint8_t *) it
                                         + ((it->size + 7) & ~7))) {
        switch(it->type) {
            case MULTIBOOT_TAG_TYPE_ACPI_NEW: {
                struct multiboot_tag_new_acpi* new_acpi = (struct multiboot_tag_new_acpi *) it;
                new->rsdp_ptr = boot_allocate(((rsdp2_t*)&new_acpi->rsdp)->length);
                memcpy(new->rsdp_ptr, &new_acpi->rsdp, ((rsdp2_t*)&new_acpi->rsdp)->length);
            } break;

            case MULTIBOOT_TAG_TYPE_ACPI_OLD: {
                // only set if not set to the new one
                struct multiboot_tag_old_acpi* old_acpi = (struct multiboot_tag_old_acpi *) it;
                if(new->rsdp_ptr == 0) {
                    new->rsdp_ptr = boot_allocate(sizeof(rsdp_t));
                    memcpy(new->rsdp_ptr, &old_acpi->rsdp, sizeof(rsdp_t));
                }
            } break;

            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO: {
                struct multiboot_tag_basic_meminfo* meminfo = (struct multiboot_tag_basic_meminfo *) it;
                new->total_mem = (meminfo->mem_lower + meminfo->mem_upper) * KB(1);
            } break;

            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
                struct multiboot_tag_framebuffer* framebuffer = (struct multiboot_tag_framebuffer *) it;
                if(framebuffer->common.framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB) {
                    new->framebuffer.type = FRAMEBUFFER_GRAPHICS_MODE;
                }else if(framebuffer->common.framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT) {
                    new->framebuffer.type = FRAMEBUFFER_TEXT_MODE;
                }else {
                    new->framebuffer.type = FRAMEBUFFER_NOT_SUPPORTED;
                }
                new->framebuffer.addr = framebuffer->common.framebuffer_addr;
                new->framebuffer.width = framebuffer->common.framebuffer_width;
                new->framebuffer.height = framebuffer->common.framebuffer_height;
                new->framebuffer.bpp = framebuffer->common.framebuffer_bpp;
            } break;

            case MULTIBOOT_TAG_TYPE_MMAP: {
                multiboot_memory_map_t *mmap;

                for (mmap = ((struct multiboot_tag_mmap *) it)->entries;
                     (multiboot_uint8_t *) mmap
                     < (multiboot_uint8_t *) it + it->size;
                     mmap = (multiboot_memory_map_t *)
                             ((unsigned long) mmap
                              + ((struct multiboot_tag_mmap *) it)->entry_size)) {
                    switch(mmap->type) {
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

                new->mmap.entries = boot_allocate(new->mmap.count * sizeof(mmap_entry_t));
                size_t i = 0;
                for (mmap = ((struct multiboot_tag_mmap *) it)->entries;
                     (multiboot_uint8_t *) mmap
                     < (multiboot_uint8_t *) it + it->size;
                     mmap = (multiboot_memory_map_t *)
                             ((unsigned long) mmap
                              + ((struct multiboot_tag_mmap *) it)->entry_size), i++) {
                    switch(mmap->type) {
                        case MULTIBOOT_MEMORY_RESERVED:
                        case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
                        case MULTIBOOT_MEMORY_NVS:
                        case MULTIBOOT_MEMORY_AVAILABLE:
                        case MULTIBOOT_MEMORY_BADRAM: {
                            mmap_entry_t* entry = &new->mmap.entries[i];
                            entry->addr = mmap->addr;
                            entry->size = mmap->len;
                            entry->type = mmap_types[mmap->type];
                        } break;
                        default:break;
                    }
                }
            } break;

            default:break;
        }
    }

    // make sure we have memory information
    if(new->total_mem == 0) return;
    if(new->mmap.entries == 0) return;

    if(new->rsdp_ptr == 0) {
        // the tags did not give us the acpi table! lets try and find it ourselves
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
            new->rsdp_ptr = cur;
            break;
        }
    }

    // call the kernel
    kernel_main(new);
}
