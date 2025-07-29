
#ifndef HASHMAP_H__
#define HASHMAP_H__

#include <ustd/array.h>

#define HASHMAP(type_) type_ *
#define HASHMAP_ANY void *

HASHMAP_ANY hashmap_create(
        struct allocator alloc,
        size_t element_size,
        size_t starting_capacity);

void hashmap_destroy(
        struct allocator alloc,
        HASHMAP_ANY *map);

void hashmap_ensure_capacity(
        struct allocator alloc,
        HASHMAP_ANY *map,
        size_t additional_capacity);

size_t hashmap_index_of(
        HASHMAP_ANY map,
        const char *key);

size_t hashmap_set(
        HASHMAP_ANY map,
        const char *key,
        void *value);

size_t hashmap_set_hashed(
        HASHMAP_ANY map,
        u32 hash,
        void *value);

ARRAY(u32) hashmap_keys(
        HASHMAP_ANY map);

#endif
