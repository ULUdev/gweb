#include <string.h>
#include "gweb_string.h"
#include <stdlib.h>
#include "linked_list/linked_list.h"
#include <stdio.h>

// returns 0 if the strings are equal and 1 if they are different
int gweb_streq(char *s1, char *s2) {
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
int gweb_strstartswith(char *s1, char *s2) {
	if (strlen(s1) < strlen(s2)) {
		return 1;
	}
	int startswith = 1;
	for (int i=0;i<strlen(s2);i++) {
		if (s1[i] == s2[i]) {
			startswith = 0;
		} else {
			startswith = 1;
			break;
		}
	}
	return startswith;
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
	for (size_t i=0;i<strlen(str);i++) {
		// filter empty buffer
		if(str[i] == sep) {
			if (strlen(buffer) == 0) {
				continue;
			}
			printf("buffer: %s\n", buffer); // DEBUG
			// add a copy of buffer to the list
			linked_list_push(list, strdup(buffer));

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
		linked_list_push(list, strdup(buffer));
	}
	free(buffer);

	return list;
}
