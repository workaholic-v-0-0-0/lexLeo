// src/logger/src/logger.c

#include "logger.h"
#include <stdlib.h>

/**
 * \var log_file
 * \brief The file pointer for the log file.
 *
 * This external variable holds the file pointer for the log file. It is used
 * by the logging functions to write messages. The file is opened during
 * `init_logger()` and closed during `close_logger()`. It should not be
 * accessed directly by the user.
 */
static FILE *log_file = NULL;

int init_logger(const char *log_path) {
    if (log_path == NULL) {
        return -1;
    }
    return 0;
}
