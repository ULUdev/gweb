#include "file.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include "string.h"
#include "linked_list/linked_list.h"

#define fexists(file) (access(file, F_OK) == 0)

int gweb_create_dir_recurse(const char *dirpath) {
	linked_list_t *dirs = gweb_strsplit(dirpath, '/');
	linked_list_reverse(dirs);
	assert(linked_list_pop(dirs) == NULL);
	char *dir = linked_list_pop(dirs);
	char *dir_buf = malloc(strlen(dir)+2);
	while (dir != NULL) {
		dir_buf = strcat(dir_buf, "/");
		dir_buf = strcat(dir_buf, dir);
		if (!fexists(dir_buf)) mkdir(dir_buf, S_IFDIR);
		realloc(dir_buf, strlen(dir_buf)+strlen(dir)+2);
		dir = linked_list_pop(dirs);
	}
	linked_list_destroy(dirs);
	
	return 0;
}

char *gweb_cookie_file() {
	char *cookiefile = malloc(100);
	char *cookiedir = malloc(100);
	strcpy(cookiefile, gweb_get_homedir());
	strcpy(cookiedir, gweb_get_homedir());
	cookiedir = strcat(cookiedir, "/.local/share/gweb");
	cookiefile = strcat(cookiefile, "/.local/share/gweb/cookie.db");
	if (gweb_create_dir_recurse(cookiedir) != 0) {
		return NULL;
	}

	return cookiefile;
}
