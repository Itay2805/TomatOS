#include "arch/stivale2.h"
#include <util/defs.h>

stivale2_struct_t* g_stivale2_struct = NULL;

void* get_stivale2_tag(uint64_t id) {
    stivale2_tag_t* tag = g_stivale2_struct->tags;
    while (tag != NULL) {
        if (tag->identifier == id) {
            return tag;
        }
        tag = tag->next;
    }
    return NULL;
}

