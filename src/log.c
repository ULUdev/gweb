#include "log.h"
#include <stdio.h>
#include <stdlib.h>

int gweb_log_level_v;
void gweb_log_init(int *log_level) { *log_level = 0; }
void gweb_log_set(int *log_level, int level) { *log_level = level; }
