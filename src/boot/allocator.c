#include <common.h>
#include "allocator.h"

static char* free_space;

void boot_allocator_init(void* boot_info) {
    if(boot_info > (void *) 0x00000500 && boot_info < (void *) 0x00007BFF) {
        free_space = (char *) 0x00007E00;
    }else {
        free_space = (char *) 0x00000500;
    }
}

void* boot_allocate(size_t size) {
    char* alloc = free_space;
    free_space += ALIGN_UP(size, 8);
    return alloc;
}
