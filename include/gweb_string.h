#ifndef __GWEB_STRING_H__
#define __GWEB_STRING_H__
#include "linked_list/linked_list.h"
int gweb_streq(char *s1, char *s2);
int gweb_strstartswith(char *s1, char *s2);
char *gweb_get_homedir();
linked_list_t *gweb_strsplit(const char *str, const char sep);

char *gweb_astrstripltw(const char *str);
void gweb_strstripltw(char *str);
#endif
