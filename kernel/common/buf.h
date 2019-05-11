#ifndef COMMON_BUF_H
#define COMMON_BUF_H

////////////////////////////////////////////////////////////////////////////
// Stretchy Buffer
////////////////////////////////////////////////////////////////////////////
//
// This is a stretchy buffer implementation, this is very helpful for
// dynamic arrays.
//
// This implementation only works in the kernel has it uses the kernel's
// memory manager
//
// Taken from:
// https://github.com/pervognsen/bitwise/blob/master/ion/common.c
//
////////////////////////////////////////////////////////////////////////////

// Todo: stop using __builtin_offsetof

#include <memory/mm.h>
#include <kernel.h>
#include "stdint.h"
#include "common.h"

typedef struct BufHdr {
    size_t len;
    size_t cap;
    char buf[];
} BufHdr;

#define buf__hdr(b) ((BufHdr *)((char *)(b) - __builtin_offsetof(BufHdr, buf)))

#define buf_len(b) ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b) ((b) ? buf__hdr(b)->cap : 0)
#define buf_end(b) ((b) + buf_len(b))
#define buf_sizeof(b) ((b) ? buf_len(b)*sizeof(*b) : 0)
#define buf_resize(b, s) ((b) ? buf__hdr(b)->len = s : 0)

#define buf_free(b) ((b) ? (kfree(buf__hdr(b)), (b) = NULL) : 0)
#define buf_fit(b, n) ((n) <= buf_cap(b) ? 0 : ((b) = buf__grow((b), (n), sizeof(*(b)), __FILENAME__, __LINE__)))
#define buf_push(b, ...) (buf_fit((b), 1 + buf_len(b)), (b)[buf__hdr(b)->len++] = (__VA_ARGS__))
#define buf_clear(b) ((b) ? buf__hdr(b)->len = 0 : 0)

void *buf__grow(const void *buf, size_t new_len, size_t elem_size, const char* filename, int line);
#endif
