#ifndef __GWEB_FILE_H__
#define __GWEB_FILE_H__
#include <unistd.h>
#define gweb_fexists(file) (access(file, F_OK))
int gweb_direxists(const char *dirpath);
int gweb_create_dir_recurse(const char *dirpath);
char *gweb_cookie_file();
#endif
