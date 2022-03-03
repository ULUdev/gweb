#ifndef __GWEB_STRING_H__
#define __GWEB_STRING_H__
#include "linked_list/linked_list.h"
int gweb_streq(const char *s1, const char *s2);
int gweb_strstartswith(const char *s1, const char *s2);
char *gweb_get_homedir();
linked_list_t *gweb_strsplit(const char *str, const char sep);
char *gweb_strdup(const char *str);

char *gweb_astrstripltw(const char *str);
void gweb_strstripltw(char *str);
#endif
