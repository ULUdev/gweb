#ifndef __GWEB_CONFIG_H__
#define __GWEB_CONFIG_H__
#include "hashmap.h"
#include "log.h"

hashmap_t *gweb_parse_config(const char *filename, gweb_logger *logger);

int gweb_parse_bool_str(const char *str);

#endif
