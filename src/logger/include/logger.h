/**
* \file logger.h
 * \brief Logging utility for recording information and errors in a log file.
 *
 * This file provides the declarations for the logger utility functions, which
 * allow writing informational and error messages to a specified log file. The
 * logger needs to be initialized before use and closed after usage.
 *
 * \author Sylvain Labopin
 * \date 2025-05-17
 * \par Example
 *
 * \code
 * init_logger("app.log");
 * log_info("Application started with PID: %d", getpid());
 * log_error("Failed to open file: %s", "example.txt");
 * close_logger();
 * \endcode
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * @brief Initializes the logger by opening the specified log file.
*
 * This function opens the file at the specified path for appending log messages.
 *
 * @param log_path The path to the log file.
 * @return 0 on success, error code otherwise.
 *
 * Preconditions:
 *   - log_path != NULL
 *   - The function must not be called more than once without a call to deinit_logger in between.
 *
 * Postconditions:
 *   - If the return value is 0, the logger is ready to use.
 *   - On failure, the logger remains uninitialized.
 */
int init_logger(const char *log_path);

/**
 * \brief Logs an informational message to the log file.
 *
 * This function writes an informational message to the log file. The format
 * and arguments follow the same syntax as `printf()`.
 *
 * \param format A format string as used by `printf()`.
 * \param ... The values to format and write.
 */
void log_info(const char *format, ...);

/**
 * \brief Logs an error message to the log file.
 *
 * This function writes an error message to the log file. The format and
 * arguments follow the same syntax as `printf()`.
 *
 * \param format A format string as used by `printf()`.
 * \param ... The values to format and write.
 */
void log_error(const char *format, ...);

/**
 * \brief Closes the log file.
 *
 * This function closes the log file if it was opened. It is important to call
 * this function to release resources after logging is done.
 */
void close_logger();

#endif // LOGGER_H
