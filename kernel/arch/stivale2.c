#include "arch/stivale2.h"
#include <util/defs.h>
#include <mem/mm.h>
#include <util/trace.h>

stivale2_struct_t* g_stivale2_struct = NULL;

void* get_stivale2_tag(uint64_t id) {
    if (g_stivale2_struct == NULL) {
        WARN("Tried to get a stivale2 tag after early init");
        return NULL;
    }

    stivale2_tag_t* tag = (stivale2_tag_t*)g_stivale2_struct->tags;

    while (tag != NULL) {
        if (tag->identifier == id) {
            return tag;
        }
        tag = (stivale2_tag_t*)tag->next;
    }

    return NULL;
}
