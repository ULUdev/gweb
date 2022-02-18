#include "file.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include "gweb_string.h"
#include "linked_list/linked_list.h"
#include <stdio.h>

#define fexists(file) (access(file, F_OK) == 0)

// returns -1 on fail
int gweb_create_dir_recurse(const char *dirpath) {
	return mkdir(dirpath, 0777);
}

char *gweb_cookie_file() {
	char *cookiefile = malloc(strlen(gweb_get_homedir()) + strlen("/.local/share/gweb/cookie.db") + 1);
	char *cookiedir = malloc(strlen(gweb_get_homedir()) + strlen("/.local/share/gweb") + 1);
	strcpy(cookiefile, gweb_get_homedir());
	strcpy(cookiedir, gweb_get_homedir());
	strcpy(cookiedir, strcat(cookiedir, "/.local/share/gweb"));
	strcpy(cookiefile, strcat(cookiefile, "/.local/share/gweb/cookie.db"));
	int result = 0;
	if (!fexists(cookiedir)) {
		result = gweb_create_dir_recurse(cookiedir);
	}
	free(cookiedir);
	if (result != 0) {
		fprintf(stderr, "failed to create cookiedir\n");
		return NULL;
	}
	return cookiefile;
}
