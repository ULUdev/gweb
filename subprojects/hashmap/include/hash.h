#ifndef __HASHMAP_HASH_H__
#define __HASHMAP_HASH_H__
#include <stdlib.h>

size_t modulo_sum_hash(const char *input, const size_t size);
size_t concatenative_modulo_sum_hash(const char *str, const size_t map_size);

#endif
