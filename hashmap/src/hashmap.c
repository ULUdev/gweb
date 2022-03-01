#include "hashmap.h"
#include "hash.c"
#include <stdlib.h>

int streq(const char *s1, const char *s2) {
    if (strlen(s1) != strlen(s2)) {
        return 1;
    } else {
        return strncmp(s1, s2, strlen(s1));
    }
}

struct HashmapElement {
    char *identifier;
    void *value;
};

struct Hashmap {
    struct HashmapElement **map;
    size_t (*hash_func)(const char *, const size_t);
    size_t size;
};

hashmap_t *hashmap_new(const size_t size) {
    hashmap_t *map = malloc(sizeof(hashmap_t));
    map->map = malloc(sizeof(struct HashmapElement *) * size);

    for (int i = 0; i < size; i++) {
        map->map[i] = NULL;
    }

    map->size = size;
    map->hash_func = NULL;
    return map;
}

void hashmap_set_hashing_algorithm(hashmap_t *hashmap,
                                   size_t (*func)(const char *, const size_t)) {
    hashmap->hash_func = func;
}

int hashmap_insert(hashmap_t *hashmap, const char *identifier, void *value) {
    if (hashmap->hash_func == NULL) {
        return -1;
    }
    struct HashmapElement *new = malloc(sizeof(struct HashmapElement));
    new->identifier = identifier;
    new->value = value;

    size_t hout = (*hashmap->hash_func)(identifier, hashmap->size);

    // handle collision
    if (hashmap->map[hout] != NULL &&
        streq(hashmap->map[hout]->identifier, identifier)) {
        int handled = 1;
        for (int i = hout; i < hashmap->size; i++) {
            if (hashmap->map[i] == NULL) {
                hashmap->map[i] = new;
                handled = 0;
                break;
            }
        }
        if (handled == 1) {
            // no free space
            free(new);
            return 1;
        }
    } else {
        hashmap->map[hout] = new;
    }

    return 0;
}

// function returns NULL if the requested element is not found or if no hashing
// algorithm is set
void *hashmap_read(hashmap_t *hashmap, const char *identifier) {
    if (hashmap->hash_func == NULL) {
        return NULL;
    }

    size_t hout = (*hashmap->hash_func)(identifier, hashmap->size);

    if (hashmap->map[hout] == NULL) {
        return NULL;
    } else if (streq(hashmap->map[hout]->identifier, identifier) == 0) {
        return hashmap->map[hout]->value;
    } else {
        char *sident = hashmap->map[hout]->identifier;
        int handled = 1;
        for (int i = hout; i < hashmap->size; i++) {
            if (streq(sident, identifier) == 0) {
                handled = 0;
                break;
            } else {
                if (hashmap->map[i] != NULL) {
                    sident = hashmap->map[i]->identifier;
                } else {
                    sident = "";
                }
            }
            hout = i;
        }

        if (handled == 0 && hashmap->map[hout]) {
            return hashmap->map[hout]->value;
        } else {
            return NULL;
        }
    }
}

void hashmap_destroy(hashmap_t *hashmap) {
    if (hashmap == NULL) {
        return;
    }
    for (int i = 0; i < hashmap->size; i++) {
        if (hashmap->map[i] != NULL) {
            free(hashmap->map[i]);
        }
    }

    free(hashmap);
}
