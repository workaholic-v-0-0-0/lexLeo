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
 * Description:
 *   - Attempts to open the file specified by log_path using fopen(log_path, "a").
 *   - If the logger is already initialized (log_file != NULL), the function does nothing and returns -1.
 *   - If log_path is NULL, the function does nothing and returns -1.
 *   - On success, the logger is initialized and ready for use.
 *
 * @param log_path The path to the log file to open (may be NULL).
 * @return 0 on success, -1 on error (including already initialized, invalid path, or open failure).
 *
 * Preconditions:
 *   - None. The function accepts any value for log_path (including NULL).
 *
 * Postconditions:
 *   - If log_path == NULL or log_file != NULL:
 *       - The function returns -1.
 *       - log_file is not modified.
 *   - Otherwise:
 *       - If the file can be opened in append mode ("a"):
 *           - The function returns 0.
 *           - log_file points to the opened file.
 *       - Otherwise (open error, invalid path, permissions, etc.):
 *           - The function returns -1.
 *           - log_file remains NULL.
 *
 * Side effects:
 *   - May create the file at log_path if it does not exist.
 *   - Allocates a system resource (the FILE*) on success; must later be released with close_logger().
 *   - No effect if already initialized (log_file != NULL).
 *
 * Note:
 *   - The user is responsible for calling close_logger() to release the resource when logging is complete.
 *   - The function is not thread-safe.
 */
int init_logger(const char *log_path);

/**
 * \brief Closes the log file.
 *
 * Description:
 *   - Attempts to close the file pointed to by log_file using fclose(log_file).
 *   - If the logger is not initialized (log_file == NULL), close_logger does nothing and returns -1.
 *   - If fclose(log_file) returns EOF (error), log_file is left unchanged, and close_logger returns -1.
 *   - If fclose succeeds, the resource pointed to by log_file is released, log_file is set to NULL, and close_logger returns 0.
 *
 * Preconditions:
 *   - None.
 *
 * Postconditions:
 *   - If log_file == NULL:
 *       - The function returns -1.
 *       - log_file remains NULL.
 *   - If log_file != NULL:
 *       - If fclose(log_file) returns EOF:
 *           - The function returns -1.
 *           - log_file still points to the open file (not closed).
 *       - Otherwise (fclose succeeds):
 *           - The function returns 0.
 *           - log_file is set to NULL.
 *           - The log file is properly closed and its resources are released.
 *
 * Side effects:
 *   - May flush buffered output to the log file before closing.
 *   - Closes the underlying file descriptor and releases resources associated with the log file.
 *   - Sets log_file to NULL if the closure is successful.
 *
 * Note:
 *   - The user is responsible for calling close_logger() to release the log file resource when logging is complete.
 *   - After a successful call, log_file is set to NULL and cannot be used until re-initialized.
 */
int close_logger();

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

#endif // LOGGER_H
