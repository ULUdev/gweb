#include "url.h"
#include "log.h"
#include "gweb_string.h"
#include "tabbing.h"
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

#define strstartswith(s1, s2) (gweb_strstartswith(s1, s2) == 0)

char *process_url(char *in) {
    if (!strstartswith(in, "http://") && !strstartswith(in, "https://") &&
        !strstartswith(in, "file://")) {
        if (access(in, F_OK) == 0) {
            char *tmp = realpath(in, NULL);
            char *path = malloc(strlen(tmp) + 8);
            strcpy(path + 7, tmp);
            path[0] = 'f';
            path[1] = 'i';
            path[2] = 'l';
            path[3] = 'e';
            path[4] = ':';
            path[5] = '/';
            path[6] = '/';
            gweb_log_msg("processing url: %s => %s", in, path);
            return path;
        } else if (!strchr(in, '.')) {
            char *query = malloc(strlen(in) + strlen(GWEB_SEARCH_QUERY) + 1);
            strcpy(query, GWEB_SEARCH_QUERY);
            query = strcat(query, in);
            gweb_log_msg("processing url: %s => %s", in, query);
            return query;
        } else {
            char *path = malloc(strlen(in) + 8);
            strcpy(path, "http://");
            strcpy(path + 7, in);
            gweb_log_msg("processing url: %s => %s", in, path);
            return path;
        }
    } else
        return in;
}
