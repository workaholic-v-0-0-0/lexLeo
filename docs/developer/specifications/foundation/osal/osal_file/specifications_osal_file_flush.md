@page specifications_osal_file_flush osal_file_ops_t::flush specifications

# Signature

    osal_file_status_t (*flush)(OSAL_FILE *stream);

# Purpose

Flush any buffered write data associated with an open `OSAL_FILE`
through the active OSAL file backend.

# Preconditions

- If `stream != NULL`, then `stream` designates a valid `OSAL_FILE`.
- If `stream != NULL`, then `stream` has been opened with a mode
  compatible with writing.

# Invalid arguments

- `stream` must not be `NULL`.

# Success

- Returns `OSAL_FILE_STATUS_OK`.
- Ensures that buffered write data associated with `stream` has been
  forwarded to the active backend as required by that backend's flush
  operation.

# Failure

- Returns `OSAL_FILE_STATUS_INVALID` if `stream == NULL`.
- Returns a non-OK `osal_file_status_t` representing the backend flush
  failure when a backend flush error occurs.
- Backend flush failures are reported through the portable
  `osal_file_status_t` status domain.

# Ownership

- This operation does not transfer ownership of `stream`.
- The caller retains ownership of `stream` on both success and failure.

# Notes

- `flush()` does not close `stream`.
- `flush()` does not change ownership or lifetime responsibilities for
  `stream`.
- This operation is mainly useful for writable streams whose buffered
  output must be made visible before `close()`.
