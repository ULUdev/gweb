#include "gweb_string.h"
#include "linked_list/linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

#define strdup(str) gweb_strdup(str)

/*
 * check if s1 and s2 are equal. If so return 0 otherwise return 1
 */
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

/*
 * check if s1 startswith s2. If so return 0 otherwise return 1
 */
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

/*
 * get the homedir of the user. Note: aborts on windows
 */
char *gweb_get_homedir() {
#if _WIN32
    // windows is not implemented
    abort();
#else
    // assuming unix based distribution that sets $HOME
    return getenv("HOME");
#endif
}

/*
 * get all the substrings of str separated by sep and store them in a linked
 * list that is returned
 */
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
 * strip str of trailing whitespace
 */
void gweb_strstriptw(char *str) {
    while (iswspace(str[strlen(str) - 1]) != 0) {
        // replace whitespace character with '\0' to terminate the string
        str[strlen(str) - 1] = '\0';
    }
}

/*
 * strip str of leading whitespace
 */
void gweb_strstriplw(char *str) {
    while (iswspace(str[0]) != 0) {
        // shift the string one to the left
        for (int i = 1; i <= strlen(str); i++) {
            str[i - 1] = str[i];
        }
    }
}

/*
 * strip str of leading and trailing whitespace
 */
void gweb_strstripltw(char *str) {

    // remove leading whitespace
    gweb_strstriplw(str);

    // remove trailing whitespace
    gweb_strstriptw(str);
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

/*
 * duplicate a string. Dynamically allocates the new string on the heap which
 * can be freed by calling free() Note: returns NULL if the input is NULL
 */
char *gweb_strdup(const char *str) {
    if (!str) {
        return NULL;
    }

    char *dup = malloc(strlen(str) + 1);
    strcpy(dup, str);
    return dup;
}

/*
 * abbreviate str to length and add '...' to it if it is to long
 */
void gweb_strabbrev(char *str, const int length) {
    if (strlen(str) > length && length >= 3) {
        str[length - 3] = '.';
        str[length - 2] = '.';
        str[length - 1] = '.';
        str[length] = '\0';
    }
}
char *gweb_straabbrev(const char *str, const int length) {
    char *new_str = gweb_strdup(str);
    gweb_strabbrev(new_str, length);
    return new_str;
}
