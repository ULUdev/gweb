#include "hash.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

size_t modulo_sum_hash(const char *str, const size_t map_size) {
    size_t sum = 0;
    for (int i = 0; i < strlen(str); i++) {
        sum += (size_t)str[i];
    }
    return sum % map_size;
}

size_t concatenative_modulo_sum_hash(const char *str, const size_t map_size) {
    size_t sum = 0;
    char prev = 0;
    for (int i = 0; i < strlen(str); i++) {
        uint16_t current = prev << sizeof(char) * 8;
        current = current | str[i];
        sum += (size_t)current;
        prev = str[i];
    }

    return sum % map_size;
}
