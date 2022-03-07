#include "gweb_string.h"
#include "linked_list/linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

#define strdup(str) gweb_strdup(str)

// returns 0 if the strings are equal and 1 if they are different
int gweb_streq(const char *s1, const char *s2) {
    if (strlen(s1) != strlen(s2)) {
        return 1;
    }
    if (strncmp(s1, s2, strlen(s1)) == 0) {
        return 0;
    } else {
        return 1;
    }
}

// check if s1 starts with s2 and return 0 if true and 1 if not true
int gweb_strstartswith(const char *s1, const char *s2) {
    if (strlen(s1) < strlen(s2)) {
        return 1;
    }
    int startswith = strncmp(s1, s2, strlen(s2));
    if (startswith == 0) {
      return 0;
    } else {
      return 1;
    }
}

char *gweb_get_homedir() {
#if _WIN32
    // windows is not implemented
    abort();
#else
    // assuming unix based distribution that sets $HOME
    return getenv("HOME");
#endif
}

linked_list_t *gweb_strsplit(const char *str, const char sep) {

    // create a buffer for storing the current substring until `sep`
    char *buffer = malloc(strlen(str));
    strcpy(buffer, "");

    // create an output linked list
    linked_list_t *list = linked_list_new();
    for (size_t i = 0; i < strlen(str); i++) {
        // filter empty buffer
        if (str[i] == sep) {
            if (strlen(buffer) == 0) {
                continue;
            }
            printf("buffer: %s\n", buffer); // DEBUG
            // add a copy of buffer to the list
            linked_list_push(list, (void *)strdup(buffer));

            // reset the buffer
            strcpy(buffer, "");
        } else {
            char tmp[2];
            tmp[0] = str[i];
            tmp[1] = '\0';
            // add the current character to the buffer
            strcpy(buffer, strcat(buffer, tmp));
        }
    }
    if (strlen(buffer) > 0) {
        linked_list_push(list, (void *)strdup(buffer));
    }
    free(buffer);

    return list;
}

/*
 * strip str of leading and trailing whitespace
 */
void gweb_strstripltw(char *str) {

    while (iswspace(str[0]) != 0) {
        // shift the string one to the left
        for (int i = 1; i <= strlen(str); i++) {
            str[i - 1] = str[i];
        }
    }

    while (iswspace(str[strlen(str) - 1]) != 0) {
        // replace whitespace character with '\0' to terminate the string
        str[strlen(str) - 1] = '\0';
    }
}

/*
 * create a new allocated string based on str that holds the same content of str
 * except that it's stripped of leading and trailing whitespace
 */
char *gweb_astrstripltw(const char *str) {
    char *new = malloc(strlen(str) + 1);
    strcpy(new, str);
    gweb_strstripltw(new);
    return new;
}

char *gweb_strdup(const char *str) {
    if (!str) {
        return NULL;
    }

    char *dup = malloc(strlen(str) + 1);
    strcpy(dup, str);
    return dup;
}
