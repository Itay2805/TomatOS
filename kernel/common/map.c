#include <memory/mm.h>
#include <kernel.h>
#include "map.h"
#include "common.h"
#include "string.h"

uint64_t hash_uint64(uint64_t x) {
    x *= 0xff51afd7ed558ccd;
    x ^= x >> 32;
    return x;
}

uint64_t hash_ptr(const void *ptr) {
    return hash_uint64((uintptr_t)ptr);
}

uint64_t hash_mix(uint64_t x, uint64_t y) {
    x ^= y;
    x *= 0xff51afd7ed558ccd;
    x ^= x >> 32;
    return x;
}

uint64_t hash_bytes(const void *ptr, size_t len) {
    uint64_t x = 0xcbf29ce484222325;
    const char *buf = (const char *)ptr;
    for (size_t i = 0; i < len; i++) {
        x ^= buf[i];
        x *= 0x100000001b3;
        x ^= x >> 32;
    }
    return x;
}

uint64_t map_get_uint64_from_uint64(map_t *map, uint64_t key) {
    if (map->len == 0) {
        return 0;
    }
    size_t i = (size_t)hash_uint64(key);
    for (;;) {
        i &= map->cap - 1;
        if (map->keys[i] == key) {
            return map->vals[i];
        }
        else if (!map->keys[i]) {
            return 0;
        }
        i++;
    }
    return 0;
}

void map_grow(map_t *map, size_t new_cap) {
    new_cap = CLAMP_MIN(new_cap, 16);
    map_t new_map = {
            mm_allocate(&kernel_memory_manager, new_cap * sizeof(uint64_t)),
            mm_allocate(&kernel_memory_manager, new_cap * sizeof(uint64_t)),
            0,
            new_cap,
    };
    memset(new_map.keys, 0, new_cap * sizeof(uint64_t));
    for (size_t i = 0; i < map->cap; i++) {
        if (map->keys[i] && map->vals[i]) {
            map_put_uint64_from_uint64(&new_map, map->keys[i], map->vals[i]);
        }
    }
    if(map->keys != NULL) mm_free(&kernel_memory_manager, map->keys);
    if(map->vals != NULL) mm_free(&kernel_memory_manager, map->vals);
    *map = new_map;
}

void map_put_uint64_from_uint64(map_t *map, uint64_t key, uint64_t val) {
    if (!val) {
        return;
    }
    if (2 * map->len >= map->cap) {
        map_grow(map, 2 * map->cap);
    }
    size_t i = (size_t)hash_uint64(key);
    for (;;) {
        i &= map->cap - 1;
        if (!map->keys[i]) {
            map->len++;
            map->keys[i] = key;
            map->vals[i] = val;
            return;
        }
        else if (map->keys[i] == key) {
            map->vals[i] = val;
            return;
        }
        i++;
    }
}

void *map_get(map_t *map, const void *key) {
    return (void *)(uintptr_t)map_get_uint64_from_uint64(map, (uint64_t)(uintptr_t)key);
}

void map_put(map_t *map, const void *key, void *val) {
    map_put_uint64_from_uint64(map, (uint64_t)(uintptr_t)key, (uint64_t)(uintptr_t)val);
}

void *map_get_from_uint64(map_t *map, uint64_t key) {
    return (void *)(uintptr_t)map_get_uint64_from_uint64(map, key);
}

void map_put_from_uint64(map_t *map, uint64_t key, void *val) {
    map_put_uint64_from_uint64(map, key, (uint64_t)(uintptr_t)val);
}

uint64_t map_get_uint64(map_t *map, void *key) {
    return map_get_uint64_from_uint64(map, (uint64_t)(uintptr_t)key);
}

void map_put_uint64(map_t *map, void *key, uint64_t val) {
    map_put_uint64_from_uint64(map, (uint64_t)(uintptr_t)key, val);
}