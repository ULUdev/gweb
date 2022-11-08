#include "log.h"
#include <stdio.h>
#include <stdlib.h>

int gweb_log_level_v;
// struct GwebLogger {
//     gweb_log_level level;
//     char *text;
// };
//
// gweb_logger *gweb_logger_new() {
//     gweb_logger *logger = malloc(sizeof(gweb_logger));
//     logger->level = GWEB_LOG_ERR;
//     logger->text = "gweb";
//     return logger;
// }
//
// const char *gweb_log_level_text(gweb_log_level level) {
//     switch (level) {
//     case GWEB_LOG_ERR:
//         return " error";
//     case GWEB_LOG_WARN:
//         return " warning";
//     case GWEB_LOG_MSG:
//         return "";
//     default:
//         return "";
//     }
// }
//
// // TODO: add time information to a log
// // Note: level refers to the log level of the message not the logger
// void gweb_log(gweb_logger *logger, const char *text, gweb_log_level level) {
//     if (level <= logger->level) {
//         fprintf(stderr, "[%s%s] %s\n", logger->text,
//         gweb_log_level_text(level),
//                 text);
//     }
// }
//
// void gweb_set_log_level(gweb_logger *logger, gweb_log_level level) {
//     logger->level = level;
// }
//
// void gweb_logger_destroy(gweb_logger *logger) { free(logger); }
void gweb_log_init(int *log_level) { *log_level = 0; }
void gweb_log_set(int *log_level, int level) { *log_level = level; }
