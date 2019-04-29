#include "buf.h"

void *buf__grow(const void *buf, size_t new_len, size_t elem_size) {
    size_t new_cap = CLAMP_MIN(2 * buf_cap(buf), MAX(new_len, 16));
    size_t new_size = __builtin_offsetof(BufHdr, buf) + new_cap * elem_size;
    BufHdr *new_hdr;
    if (buf) {
        mm_reallocate(&kernel_memory_manager, buf__hdr(buf), new_size, (void **) &new_hdr);
    }
    else {
        mm_allocate(&kernel_memory_manager, new_size, (void **) &new_hdr);
        new_hdr->len = 0;
    }
    new_hdr->cap = new_cap;
    return new_hdr->buf;
}
