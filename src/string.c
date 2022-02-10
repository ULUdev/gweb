#include <string.h>
#include "string.h"

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
