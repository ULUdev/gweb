#ifndef __GWEB_LOG_H__
#define __GWEB_LOG_H__
extern int gweb_log_level_v;
// enum GwebLogLevel {
// 	GWEB_LOG_ERR = 0,
// 	GWEB_LOG_WARN = 1,
// 	GWEB_LOG_MSG = 2
// };
#define gweb_log_msg(s, ...) if (gweb_log_level_v >= 2) fprintf(stderr, "[gweb MSG] " s "\n", ##__VA_ARGS__)
#define gweb_log_wrn(s, ...) if (gweb_log_level_v >= 1) fprintf(stderr, "[gweb WARNING] " s "\n", ##__VA_ARGS__)
#define gweb_log_err(s, ...) fprintf(stderr, "[gweb ERROR] " s "\n", ##__VA_ARGS__)
// typedef struct GwebLogger gweb_logger;
// typedef enum GwebLogLevel gweb_log_level;
// gweb_logger *gweb_logger_new();
// void gweb_log(gweb_logger *logger, const char *text, gweb_log_level level);
// void gweb_set_log_level(gweb_logger *logger, gweb_log_level level);
// void gweb_logger_destroy(gweb_logger *logger);
void gweb_log_init(int *log_level);
void gweb_log_set(int *log_level, int level);
#endif
