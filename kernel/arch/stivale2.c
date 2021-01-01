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

    stivale2_tag_t* tag = g_stivale2_struct->tags;

    while (tag != NULL) {
        if (tag->identifier == id) {
            return tag;
        }
        tag = tag->next;
    }

    return NULL;
}

void stivale2_to_higher_half() {
    g_stivale2_struct = PHYS_TO_DIRECT(g_stivale2_struct);

    // no tags
    if (g_stivale2_struct->tags == NULL) {
        return;
    }

    // we have tags
    g_stivale2_struct->tags = PHYS_TO_DIRECT(g_stivale2_struct->tags);
    stivale2_tag_t* tag = g_stivale2_struct->tags;
    while (tag != NULL) {

        // convert the inner tag stuff
        switch (tag->identifier) {
            case STIVALE2_STRUCT_TAG_CMDLINE_IDENT: {
                stivale2_struct_tag_cmdline_t* cmdline = (stivale2_struct_tag_cmdline_t*)tag;
                if (cmdline->cmdline != NULL) {
                    cmdline->cmdline = PHYS_TO_DIRECT(cmdline->cmdline);
                }
            } break;
        }

        // break if null
        if (tag->next == NULL) {
            break;
        }

        // go next
        tag->next = PHYS_TO_DIRECT(tag->next);
        tag = tag->next;
    }
}

