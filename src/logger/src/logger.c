// src/logger/src/logger.c

#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
#endif // DEBUG

int init_logger(const char *log_path) {
    if ((log_path != NULL) && (log_file == NULL) && (log_file = fopen(log_path, "a"))) {
        return 0;
    }
    return -1;
}
