#include "file.h"
#include "gweb_string.h"
#include "linked_list/linked_list.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glib/gi18n.h>
#include <dirent.h>
#include <errno.h>

#define fexists(file) gweb_fexists(file)
#define direxists(dir) (gweb_direxists(dir) == 0)

int gweb_direxists(const char *dirpath) {
	DIR *dir = opendir(dirpath);
	if (dir) {
		closedir(dir);
		return 0;
	} else if (ENOENT == errno) {
		return 1;
	} else {
		fprintf(stderr, "failed to access directory for unkown reason!\n");
		return 2;
	}
}

// returns -1 on fail
int gweb_create_dir_recurse(const char *dirpath) {
    return mkdir(dirpath, 0777);
}

char *gweb_cookie_file() {
	char *cookiefile = g_build_filename(g_get_user_data_dir(), "gweb", "cookie.db", NULL);
	char *cookiedir = g_build_filename(g_get_user_data_dir(), "gweb", NULL);
    int result = 0;
    if (!direxists(cookiedir)) {
        result = gweb_create_dir_recurse(cookiedir);
    }
    free(cookiedir);
    if (result != 0) {
        fprintf(stderr, "failed to create cookiedir\n");
        return NULL;
    }
    return cookiefile;
}
