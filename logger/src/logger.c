/**
* \file logger.c
 * \brief Implementation of the logging utility functions.
 *
 * This file contains the implementation of functions used for logging informational
 * and error messages to a specified log file. It provides functionalities to initialize
 * the logger, log messages with different severity levels, and close the log file once
 * logging is complete.
 *
 * The logger writes messages with timestamps to the log file, allowing you to track events
 * and errors during the execution of a program.
 *
 * \author Sylvain Labopin
 * \date 2024-09-10
 *
 * \par Example
 * \code
 * init_logger("app.log");
 * log_info("Application started with PID: %d", getpid());
 * log_error("Failed to open file: %s", "example.txt");
 * close_logger();
 * \endcode
 */

#include "logger/logger.h"

#include <stdarg.h>

FILE *log_file = NULL;

void init_logger(const char *log_path) {
    log_file = fopen(log_path, "a");
    if (log_file == NULL) {
        fprintf(stderr, "Failed to open log file\n");
        exit(EXIT_FAILURE);
    }
}

void log_message(const char *level, const char *format, va_list args) {
    if (log_file == NULL) {
        return;
    }
    time_t now = time(NULL);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    fprintf(log_file, "[%s] %s: ", time_str, level);
    vfprintf(log_file, format, args);
    fprintf(log_file, "\n");
    fflush(log_file);
}

void log_info(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_message("INFO", format, args);
    va_end(args);
}

void log_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_message("ERROR", format, args);
    va_end(args);
}

void close_logger() {
    if (log_file != NULL) {
        fclose(log_file);
        log_file = NULL;
    }
}
