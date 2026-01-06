// src/foundation/logger/src/logger.c

#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#define TIME_STR_LENGTH 20

/**
 * \var log_file
 * \brief The file pointer for the log file.
 *
 * This external variable holds the file pointer for the log file. It is used
 * by the logging functions to write messages. The file is opened during
 * `init_logger()` and closed during `close_logger()`. It should not be
 * accessed directly by the user.
 *
 * MUST BE not NULL only after a successful call to init_logger().
 */
static FILE *log_file = NULL;

// make testable
#ifdef DEBUG
FILE * get_log_file() {
	return log_file;
}
void set_log_file(FILE * log_file_for_test) {
	log_file = log_file_for_test;
}
int (*logger_fclose)(FILE *) = fclose;
void set_logger_fclose(int (*logger_fclose_for_test)(FILE *)) {
    logger_fclose = logger_fclose_for_test;
}
#endif // DEBUG

int init_logger(const char *log_path) {
    if ((log_path == NULL) || (log_file != NULL))
        return -1;

    log_file = fopen(log_path, "a");
    if (!log_file)
        return -1;

    return 0;
}

int close_logger() {
#ifdef DEBUG
    if ((log_file == NULL) || (logger_fclose(log_file) == EOF))
#else
    if ((log_file == NULL) || (fclose(log_file) == EOF))
#endif
        return -1;

    log_file = NULL;
    return 0;
}

int log_message(const char *level, const char *format, va_list args) {
    if ((log_file == NULL) || (format == NULL))
        return -1;

    time_t time_now = time(NULL);
    if (time_now == -1)
        return -1;

    struct tm *time_members=localtime(&time_now); // memory area handled by time.h
    if (!time_members)
        return -1;

    char time_str[TIME_STR_LENGTH];
    if (!strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_members))
        return -1;

    fprintf(log_file, "[%s] %s: ", time_str, level);
    vfprintf(log_file, format, args);
    fprintf(log_file, "\n");
    fflush(log_file);
    return 0;
}

int log_info(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int ret = log_message("INFO", format, args);
    va_end(args);
    return ret;
}

int log_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int ret = log_message("ERROR", format, args);
    va_end(args);
    return ret;
}
