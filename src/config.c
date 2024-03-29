#include "config.h"
#include "gweb_string.h"
#include "hash.h"
#include "hashmap.h"
#include "log.h"
#include <stdio.h>
#include <string.h>

void gweb_config_process_line(const char *line, hashmap_t *hashmap) {

    char *ident, *value;
    ident = malloc(strlen(line) + 1);
    value = malloc(strlen(line) + 1);
    strcpy(ident, "");
    strcpy(value, "");

    char current;
    int delim_found = 1;
    char *buffer = malloc(2);

    // split the line into an identifier and a value
    for (int i = 0; i < strlen(line); i++) {
        current = line[i];
        if (current == '=' && delim_found != 0) {
            delim_found = 0;
        } else if (current == '\n') {
            break;
        } else if (delim_found == 0) {
            buffer[0] = current;
            buffer[1] = '\0';
            value = strcat(value, buffer);
        } else {
            buffer[0] = current;
            buffer[1] = '\0';
            ident = strcat(ident, buffer);
        }
    }

    free(buffer);

    gweb_strstripltw(ident);
    gweb_strstripltw(value);

    int res = hashmap_insert(hashmap, ident, (void *)value);
    if (res != 0) {
        if (res == 1) {
            gweb_log_wrn("failed to add item to hashmap (hashmap is full)");
        } else if (res == -1) {
            gweb_log_wrn(
                "failed to add item to hashmap (no hash function is set)");
        } else {
            gweb_log_wrn("failed to add item to hashmap (unkown reason)");
        }
        free(ident);
        free(value);
    }
}

hashmap_t *gweb_parse_config(const char *filename) {
    gweb_log_msg("parsing configuration file");
    FILE *file = fopen(filename, "r");

    // file opening failed
    if (file == NULL) {
        gweb_log_wrn("failed to open configuration file");
        return NULL;
    }

    size_t lines = 1;
    char current_char = '\0';
    while (feof(file) == 0) {
        size_t res = fread(&current_char, 1, 1, file);
        if (res == 0)
            break;
        if (current_char == '\n')
            lines++;
    }

    // reset file pointer
    fseek(file, 0, SEEK_SET);

    hashmap_t *hashmap = hashmap_new(lines);
    hashmap_set_hashing_algorithm(hashmap, concatenative_modulo_sum_hash);

    char *line = malloc(3);
    strcpy(line, "");
    char buffer;
    // unsigned int count = 0;

    char *tmp = malloc(2);
    strcpy(tmp, "");
    while (feof(file) == 0) {
        size_t res = fread(&buffer, 1, 1, file);
        if (res == 0)
            break;
        if (buffer == '\n') {
            tmp[0] = buffer;
            tmp[1] = '\0';
            line = strcat(line, tmp);
            gweb_config_process_line(line, hashmap);
            strcpy(line, "");
        } else {
            tmp[0] = buffer;
            tmp[1] = '\0';
            line = strcat(line, tmp);
        }
        line = realloc(line, strlen(line) + 2);
        // count++;
    }
    free(tmp);
    if (strlen(line) > 0) {
        gweb_config_process_line(line, hashmap);
    }

    fclose(file);

    return hashmap;
}

/*
 * Parse a boolean string to an integer.
 * Returns:
 * 0: for true
 * 1: for false
 * -1: for anything else
 */
int gweb_parse_bool_str(const char *str) {
    if (gweb_streq(str, "true") == 0) {
        return 0;
    } else if (gweb_streq(str, "false") == 0) {
        return 1;
    } else {
        return -1;
    }
}
