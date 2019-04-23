#ifndef COMMON_MAP_H
#define COMMON_MAP_H

////////////////////////////////////////////////////////////////////////////
// Map
////////////////////////////////////////////////////////////////////////////
//
// This is a map implementation, this is very helpful for
// any key-data structures.
//
// This implementation only works in the kernel has it uses the kernel's
// memory manager
//
// For string it is best to use a string interning table and use the pointer
// as the key, and try to save that pointer to continue access it.
//
// Taken from:
// https://github.com/pervognsen/bitwise/blob/master/ion/common.c
//
////////////////////////////////////////////////////////////////////////////

#include "stdint.h"

uint64_t hash_uint64(uint64_t x);
uint64_t hash_ptr(const void *ptr);
uint64_t hash_mix(uint64_t x, uint64_t y);
uint64_t hash_bytes(const void *ptr, size_t len);

typedef struct Map {
    uint64_t *keys;
    uint64_t *vals;
    size_t len;
    size_t cap;
} Map;

uint64_t map_get_uint64_from_uint64(Map *map, uint64_t key);
void map_grow(Map *map, size_t new_cap);
void map_put_uint64_from_uint64(Map *map, uint64_t key, uint64_t val);
void *map_get(Map *map, const void *key);
void map_put(Map *map, const void *key, void *val);
void *map_get_from_uint64(Map *map, uint64_t key);
void map_put_from_uint64(Map *map, uint64_t key, void *val);
uint64_t map_get_uint64(Map *map, void *key);
void map_put_uint64(Map *map, void *key, uint64_t val);

#endif
