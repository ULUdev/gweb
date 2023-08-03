#ifndef __HASHMAP_H__
#define __HASHMAP_H__
#include <stdlib.h>

typedef struct Hashmap hashmap_t;

hashmap_t *hashmap_new(const size_t size);

/*
 * any hash function takes a `const char *` as the string to hash and a `const
 * size_t` as the size of the invoking hashmap. The returned `size_t` should be
 * less than `size` and bigger or equal to 0 so:
 * hash_function(x, size) >= 0 < size
 */
void hashmap_set_hashing_algorithm(hashmap_t *hashmap, size_t (*func)(const char *, const size_t));

int hashmap_insert(hashmap_t *hashmap, const char *identifier, void *value);
void *hashmap_read(hashmap_t *hashmap, const char *identifier);

void hashmap_destroy(hashmap_t *hashmap);

#endif
