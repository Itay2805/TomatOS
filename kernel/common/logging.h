#ifndef TOMATKERNEL_LOGGING_H
#define TOMATKERNEL_LOGGING_H

void log_critical(const char* func, const char* fmt, ...) __attribute__ ((format (printf, 2, 3)));;
void log_debug(const char* func, const char* fmt, ...) __attribute__ ((format (printf, 2, 3)));;
void log_warn(const char* func, const char* fmt, ...) __attribute__ ((format (printf, 2, 3)));;
void log_error(const char* func, const char* fmt, ...) __attribute__ ((format (printf, 2, 3)));;
void log_notice(const char* func, const char* fmt, ...) __attribute__ ((format (printf, 2, 3)));;
void log_info(const char* func, const char* fmt, ...) __attribute__ ((format (printf, 2, 3)));;

#define LOG_CRITICAL(fmt, ...) log_critical(__FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) log_debug(__FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) log_warn(__FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) log_error(__FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_NOTICE(fmt, ...) log_notice(__FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) log_info(__FUNCTION__, fmt, ##__VA_ARGS__)

#endif //TOMATKERNEL_LOGGING_H
