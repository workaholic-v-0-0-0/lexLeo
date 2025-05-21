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

// make testable
#ifdef DEBUG
#include <stdio.h>
FILE * get_log_file();
void set_log_file(FILE * log_file_for_test);
#endif // DEBUG

// if  RELEASE
//   - get_log_file() is not declared even with file scope
//   - log_file is declared with file scope
// log_file is not NULL only when fopen succeds when called by init_logger

/**
 * @brief Initializes the logger by opening the specified log file.
 *
 * Description :
 *   - Attempts to open the file specified by log_path using fopen(log_path, "a").
 *   - If the logger is already initialized (log_file != NULL), the function does nothing and returns -1.
 *
 * @param log_path The path to the log file.
 * @return 0 on success, -1 otherwise.
 *
 * Preconditions:
 * None. The function accepts any value for log_path (including NULL).
 *
 * Postconditions:
 *   - If log_path == NULL or log_file != NULL
 *       - The function returns -1.
 *       - log_file is not modified.
 *   - Otherwise:
 *       - If the file can be opened in append mode ('a'):
 *           - The function returns 0.
 *           - The logger is initialized (log_file points to the opened file).
 *       - Otherwise (open error, invalid path, permissions, etc.):
 *           - The function returns -1.
 *           - The logger remains uninitialized (log_file stays NULL).
 *
 * Side effects:
 *   - May create the log_path file if it does not exist.
 *   - Allocates a system resource (the FILE*) if successful. It must be freed via close_logger.
 *   - Does nothing if already log_file is not NULL or log_path file exists.
 *
 * Note:
 *   - The user is responsible for calling a close_logger() function to release the resource when logging is complete.
 */
int init_logger(const char *log_path);

/**
 * @brief Logs an informational message to the log file.
 *
 * Description :
 *     This function writes an informational message to the log file. The format
 *     and arguments follow the same syntax as `printf()`.
 *
 * @param format A format string as used by `printf()`.
 * @param  ... The values to format and write.
 * @return 0 on success, -1 otherwise.
 *
 * Preconditions:
 * None.
 *
 * Postconditions:
 *   - If log_file == NULL or fprintf return -1 or ???
 *
 * Side effects:
 *   - Modify the file pointed to by log_file.
 *   - Does nothing if no success.
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
