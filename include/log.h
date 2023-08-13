#ifndef __GWEB_LOG_H__
#define __GWEB_LOG_H__
extern int gweb_log_level_v;
#define gweb_log_msg(s, ...)                                                   \
    if (gweb_log_level_v >= 2)                                                 \
    fprintf(stderr, "[MSG] " s "\n", ##__VA_ARGS__)
#define gweb_log_wrn(s, ...)                                                   \
    if (gweb_log_level_v >= 1)                                                 \
    fprintf(stderr, "[WARNING] " s "\n", ##__VA_ARGS__)
#define gweb_log_err(s, ...) fprintf(stderr, "[ERROR] " s "\n", ##__VA_ARGS__)
#ifdef GWEB_LOG_DEBUG
#define gweb_log_dbg(s, ...)                                                   \
    fprintf(stderr, "[DEBUG (%s:%d)] " s "\n", __FILE__, __LINE__,             \
            ##__VA_ARGS__)
#else
#define gweb_log_dbg(s, ...)
#endif
void gweb_log_init(int *log_level);
void gweb_log_set(int *log_level, int level);
#endif
