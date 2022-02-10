#ifndef __GWEB_LOG_H__
#define __GWEB_LOG_H__
enum GwebLogLevel {
	GWEB_LOG_ERR = 0,
	GWEB_LOG_WARN = 1,
	GWEB_LOG_MSG = 2
};
typedef struct GwebLogger gweb_logger;
typedef enum GwebLogLevel gweb_log_level;
gweb_logger *gweb_logger_new();
void gweb_log(gweb_logger *logger, const char *text, gweb_log_level level);
void gweb_set_log_level(gweb_logger *logger, gweb_log_level level);
void gweb_logger_destroy(gweb_logger *logger);
#endif
